#include "asyncserver.h"
#include "../IM_login/IM_login.pb.h"
#include <asio/buffer.hpp>
#include <asio/error.hpp>
#include <asio/error_code.hpp>
#include <asio/impl/read.hpp>
#include <asio/impl/write.hpp>
#include <asio/socket_base.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <hiredis/hiredis.h>
#include <hiredis/read.h>
#include <iostream>
#include <memory>
#include <fmt/core.h>
#include <mysql/mysql.h>
#include <netinet/in.h>
#include <ostream>
#include <system_error>
#include <unistd.h>
#include <vector>

AsyncServer::AsyncServer()
:acceptor_(nullptr){}

AsyncServer* AsyncServer::getInstance()
{
    static AsyncServer obj;
    return &obj;
}

void AsyncServer::init(asio::io_context &ioc,short port)
{
    acceptor_=std::make_unique<tcp::acceptor>(ioc,tcp::endpoint(tcp::v4(),port));
    std::string sql("select count(*) from userinfo;");
    AsyncExecutorPool::getInstance().addMysqlTask(sql,[this](std::shared_ptr<MYSQL> res){
        MYSQL_RES *mres=mysql_store_result(res.get());

        MYSQL_ROW row=mysql_fetch_row(mres);

        if(!row)
            throw std::runtime_error("init fail:count(*)");

        numOfAccount.store(atoi(row[0]));

        mysql_free_result(mres);
    });
    
    doAccept();
}

void AsyncServer::addClient(std::string id,std::shared_ptr<Session> client)
{
    clients_.insert({id,client});
}

std::shared_ptr<Session> AsyncServer::getClient(std::string id)
{
    if(clients_.find(id+"msg")!=clients_.end()){
        return clients_[id+"msg"];
    }
    return nullptr;
}

void AsyncServer::removeClient(std::string id)
{
    if(clients_.count(id)){
        clients_[id]->isConnect_=false;
        clients_.erase(id);
    }
}

void AsyncServer::doAccept()
{
    acceptor_->async_accept(
        [this](std::error_code ec, tcp::socket socket){
            if(!ec){
                std::make_shared<Session>(std::move(socket),this)->startWork();
                doAccept();

            } else {
                std::cerr<<"async_accept_Error:"<<ec.message()<<std::endl;
            }
        });
}

void AsyncServer::destory()
{
    if(acceptor_!=nullptr)
        acceptor_->close();
    clients_.clear();
}

Session::Session(tcp::socket socket,AsyncServer *parent)
:socket_(std::move(socket)),heartbeat_timer_(socket_.get_executor()){
    clientid_="";
    isConnect_=true;
}
Session::~Session()
{
    socket_.close();
    isConnect_=false;
}

void Session::schedule_heartbeat_check()
{
    if(!isConnect_)
        return;
    heartbeat_timer_.expires_after(std::chrono::seconds(heartbeat_interval_));
    
    AsyncExecutorPool::getInstance().addWriteTask([this]{
        PullRespond resp;
        resp.mutable_server_msg()->mutable_heart();
        sendRespondData(resp,CustomType::HEART);
    });

    heartbeat_timer_.async_wait([self=shared_from_this()](const asio::error_code &ec){
        if(ec){
            std::cout<<self->getCurrentAccount()<<" "<<ec.message()<<std::endl;
            return;
        }
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            now - self->last_heartbeat_time_
        ).count();

        if(elapsed>=heartbeat_timeout_){
            AsyncServer::getInstance()->removeClient(self->clientid_);
            return;
        } else {
            self->schedule_heartbeat_check();
        }
    });
}

void Session::startWork()
{
    auto self=shared_from_this();

    asio::async_read(socket_,asio::buffer(header_,4),
        [this,self](asio::error_code ec,std::size_t length){
            if(!ec){
                if(length<=0)
                    return;
                std::memcpy(&dataSize_, header_.data(), 4);
                dataSize_ = ntohl(dataSize_);

                data_.clear();
                data_.resize(dataSize_);

                asio::async_read(socket_,asio::buffer(data_,dataSize_),
                    [this,self](asio::error_code ec,std::size_t length){
                        if(ec){
                            std::cout<<ec.message()<<std::endl;
                            return;
                        }
                        parseData(dataSize_);
                        startWork();
                    });
                } else {
                    if(ec==asio::error::eof){
                        isConnect_=false;
                        AsyncServer::getInstance()->removeClient(clientid_);
                    }
                }
            });
}

void Session::parseData(std::size_t length)
{
    if(length<=0)
        return;
    std::string message(data_.data(), length);
    PullRequest req;

    if(!req.ParseFromString(message)){
        std::cerr<<"Error:parse fail"<<std::endl;
        return;
    }
    if(req.has_client_id()){
        AsyncServer::getInstance()->addClient(req.client_id().id(),shared_from_this());
        clientid_=req.client_id().id();
        last_heartbeat_time_=std::chrono::steady_clock::now();
        schedule_heartbeat_check();
    }else if(req.has_heart()){
        last_heartbeat_time_=std::chrono::steady_clock::now();
    }else if(req.has_login_request()){
        std::string command=fmt::format("hget {} password", req.login_request().account());

        AsyncExecutorPool::getInstance().addRedisTask(command,[req,this](std::shared_ptr<redisReply> reply){
            PullRespond res;
            if(AsyncServer::getInstance()->getClient(req.login_request().account())!=nullptr){
                res.mutable_lginreg_respond()->set_success(false);
                res.mutable_lginreg_respond()->set_msg("otherlogin");
                sendRespondData(res);
                return;
            }
            if(reply->type==REDIS_REPLY_STRING){
                bool isRight=(strcmp(req.login_request().password().data(), reply->str)==0);
                res.mutable_lginreg_respond()->set_success(isRight);
                res.mutable_lginreg_respond()->set_msg(isRight?"null":"wrong");
                sendRespondData(res);
                return;
            }
            std::string sql=fmt::format("select * from userinfo where account={};", 
                req.login_request().account());
            AsyncExecutorPool::getInstance().addMysqlTask(sql, [this,pwdToCheck=req.login_request().password()](std::shared_ptr<MYSQL> result){
                PullRespond res;
                MYSQL_RES *mres=mysql_store_result(result.get());
                MYSQL_ROW rows=mysql_fetch_row(mres);
                std::string command=fmt::format("hset {} name {} password {} email {} image {} createtime {}",
                                rows[1],rows[2],rows[3],(std::string(rows[4])==""?"\"\"":rows[4]),((rows[5]==NULL||strlen(rows[5])==0)?"\"\"":rows[5]),rows[6]);

                AsyncExecutorPool::getInstance().addRedisTask(command, [](std::shared_ptr<redisReply> res){
                    if(res->type==REDIS_REPLY_ERROR){
                        puts("has_login_request redis");
                        puts(res->str);
                    }
                });

                bool r=(strcmp(rows[3],pwdToCheck.data())==0);
                res.mutable_lginreg_respond()->set_success(r);
                res.mutable_lginreg_respond()->set_msg(r?"null":"wrong");
                mysql_free_result(mres);

                sendRespondData(res);
            });
            
        });

    } else if(req.has_register_request()) {
        std::string sql=fmt::format("insert into userinfo(account,name,password,email,createtime) values({},'{}','{}','{}',CURDATE());",
            1000000000+AsyncServer::numOfAccount.load()+1,
                req.register_request().username(),req.register_request().password(),req.register_request().email());

        std::string command=fmt::format("hset {} name {} password {} email {} image \"\" ",1000000000+AsyncServer::numOfAccount.load()+1,
                req.register_request().username(),req.register_request().password(),req.register_request().email()==""?"\"\"":req.register_request().email());
        AsyncExecutorPool::getInstance().addRedisTask(command, nullptr);

        AsyncExecutorPool::getInstance().addMysqlTask(sql, [this,&req](std::shared_ptr<MYSQL> res){
                PullRespond respond;
                uint64_t affects=mysql_affected_rows(res.get());
                if(affects){
                    int tmp=AsyncServer::numOfAccount.load()+1+1000000000;

                    respond.mutable_lginreg_respond()->set_success(true);
                    respond.mutable_lginreg_respond()->set_msg(fmt::format("{}", tmp));

                    AsyncServer::numOfAccount++;
                } else {
                    respond.mutable_lginreg_respond()->set_success(false);
                    respond.mutable_lginreg_respond()->set_msg("query");
                }
                sendRespondData(respond);

        });
    } else if(req.has_ima_request()){
        std::string sql=fmt::format("update userinfo set image='{}' where account={};",req.ima_request().img_date(),req.ima_request().user_id());
        std::string command=fmt::format("hset {} image {}", req.ima_request().user_id(),req.ima_request().img_date());
        AsyncExecutorPool::getInstance().addRedisTask(command,[](std::shared_ptr<redisReply> res){
            if(res->type==REDIS_REPLY_ERROR)
                puts("has_ima_request redis nil");
            if(res->type==REDIS_REPLY_ERROR)
                puts(res->str);
        });
         AsyncExecutorPool::getInstance().addMysqlTask(sql, [this](std::shared_ptr<MYSQL> res){
                PullRespond respond;
                uint64_t affects=mysql_affected_rows(res.get());
                if(affects){
                    respond.mutable_ima_respond()->set_success(true);
                    respond.mutable_ima_respond()->set_msg("null");
                } else {
                    respond.mutable_ima_respond()->set_success(false);
                    respond.mutable_ima_respond()->set_msg("query");
                }
                sendRespondData(respond);
        });
    }  else if(req.has_search_request()){
        if(req.search_request().st()==SearchRequest_State_FRIEND){
            std::string sql="";
            std::string str = clientid_.substr(0, clientid_.length() - 3);
            if(req.search_request().way()==SearchRequest_Way_BY_ACCOUNT){

                sql=fmt::format("SELECT u.account,u.name FROM userinfo u WHERE u.account = {} "
                    "AND u.account <> {} "
                    "AND NOT EXISTS ("
                    "SELECT 1 FROM friends f "
                    "WHERE ((f.user_id = {} AND f.friend_id = u.account) "
                    "OR (f.friend_id = {} AND f.user_id = u.account)) AND f.status in (0,1,2));",
                    req.search_request().content(),str,str,str);
            } else if(req.search_request().way()==SearchRequest_Way_BY_NAME){

                sql=fmt::format("SELECT u.account,u.name "
                            "FROM userinfo u "
                            "WHERE u.name LIKE '%{}%' " 
                            "AND u.account <> {} " 
                            "AND NOT EXISTS ( "
                            "SELECT 1 FROM friends f "
                            "WHERE ((f.user_id = {} AND f.friend_id = u.account) " 
                            "OR (f.user_id = u.account AND f.friend_id = {})) AND f.status in (0,1,2));",
                            req.search_request().content(),str,str,str);
            }

            AsyncExecutorPool::getInstance().addMysqlTask(sql, [this](std::shared_ptr<MYSQL> res){
                    MYSQL_RES *result=mysql_store_result(res.get());
                    PullRespond respond;
                    if(result==NULL){
                        sendRespondData(respond);
                        return;
                    }
                    SearchRespond* search_resp = respond.mutable_search_respond();
                    int num_fields = mysql_num_fields(result);
                    if(num_fields!=2){
                        std::cerr<<"has_search_request:unright field"<<std::endl;
                        return;
                    }

                    MYSQL_ROW row;
                    while ((row = mysql_fetch_row(result))) {
                        SearchInfo *info=search_resp->add_infos();
                        info->set_account(row[0]);
                        info->set_name(row[1]);
                    }

                    sendRespondData(respond);
                    mysql_free_result(result);
                });

        } else if(req.search_request().st()==SearchRequest_State_GROUD){

        }
    } else if(req.has_add_request()){
        if(req.add_request().st()==AddRequest_State_FRIEND){
            std::string currentAcc=req.add_request().user_id();
            std::string target=req.add_request().friend_id();
            std::string sql=fmt::format("INSERT INTO friends (user_id, friend_id, status,created_at, updated_at) "
                    "VALUES ({}, {}, {},NOW(),NOW()) "
                    "ON DUPLICATE KEY UPDATE " 
                    "status = {}, " 
                    "updated_at = NOW();",currentAcc,target,-1,-1);
            std::string sql2=fmt::format("INSERT INTO friends (user_id, friend_id, status,created_at, updated_at) "
                    "VALUES ({}, {}, {},NOW(),NOW()) "
                    "ON DUPLICATE KEY UPDATE " 
                    "status = {}, " 
                    "updated_at = NOW();", target,currentAcc,0,0);
            std::vector<std::string> sqls;
            sqls.push_back(sql);
            sqls.push_back(sql2);

            AsyncExecutorPool::getInstance().mysqlTransaction(sqls, [this,req](bool result){
                PullRespond resp;
                resp.mutable_add_respond()->set_result(result);
                sendRespondData(resp);
                if(result){
                    std::shared_ptr<Session> tar=AsyncServer::getInstance()->getClient(req.add_request().friend_id());
                    if(tar==nullptr||!tar->isConnect_)
                        return;
                    PullRespond res;
                    res.mutable_server_msg()->mutable_friend_add()->set_user_id(req.add_request().user_id());

                    tar->sendRespondData(res);
                }
            });
        } else if(req.add_request().st()==AddRequest_State_GROUP){

        }
    } else if(req.has_apply_request()){
        std::string applyer=req.apply_request().user_id();
        std::string sql=fmt::format("SELECT u.account,f.updated_at,f.status "
                            "FROM userinfo u "
                            "JOIN friends f ON u.account = f.friend_id "
                            "WHERE f.user_id = '{}' AND f.status != 1 ORDER BY f.updated_at DESC;", applyer);

        AsyncExecutorPool::getInstance().addMysqlTask(sql, [this](std::shared_ptr<MYSQL> res){
                MYSQL_RES *result=mysql_store_result(res.get());
                
                int field=mysql_num_fields(result);
                if(field!=3){
                    std::cerr<<"has_apply_request:unright field"<<std::endl;
                    return;
                }

                PullRespond resp;
                ApplyRespond *aresp=resp.mutable_apply_respond();
                MYSQL_ROW row;
                while((row=mysql_fetch_row(result))){
                    ApplyInfo *info=aresp->add_infos();
                    FriendApply *fapply=info->mutable_friend_apply();
                    fapply->set_user_id(row[0]);
                    info->set_apply_time(row[1]);
                    fapply->set_state(atoi(row[2]));
                }

                sendRespondData(resp);

                mysql_free_result(result);
        });
        
    } else if(req.has_update_rela()){
        bool result=req.update_rela().result();
        std::string target=req.update_rela().friend_id();
        std::string cur=req.update_rela().user_id();
        std::string sql=fmt::format("update friends set status = {} where user_id in ({},{});", 
        result?1:-2,req.update_rela().user_id(),req.update_rela().friend_id());

        AsyncExecutorPool::getInstance().addMysqlTask(sql, [this,target,result,cur](std::shared_ptr<MYSQL> res){
                int affect=mysql_affected_rows(res.get());
                PullRespond rep;
                rep.mutable_add_respond()->set_result(affect!=0);
                sendRespondData(rep);
                std::shared_ptr<Session> tar=AsyncServer::getInstance()->getClient(target);
                if(tar==nullptr||!tar->isConnect_)
                    return;
                rep.mutable_update_rela()->set_result(result);
                rep.mutable_update_rela()->set_user_id(tar->getCurrentAccount());
                rep.mutable_update_rela()->set_friend_id(cur);
                tar->sendRespondData(rep);
        });
    } else if(req.has_user_info()){
        std::string command=fmt::format("hgetall {}",req.user_info().user_id());
        AsyncExecutorPool::getInstance().addRedisTask(command,[this,req](std::shared_ptr<redisReply> res){
            if(res.get()){
                if (res->type == REDIS_REPLY_ARRAY&&res->elements != 0) {
                    PullRespond resp;
                    resp.mutable_user_info()->set_account(req.user_info().user_id());
                    for (size_t i = 0; i < res->elements; i += 2) {
                        if(strcmp(res->element[i]->str, "name")==0){
                            resp.mutable_user_info()->set_name(res->element[i+1]->str);
                        }
                        if(strcmp(res->element[i]->str, "email")==0){
                            resp.mutable_user_info()->set_email(res->element[i+1]->str);
                        }
                        if(strcmp(res->element[i]->str, "image")==0){
                            resp.mutable_user_info()->set_image(res->element[i+1]->str);
                        }
                        if(strcmp(res->element[i]->str, "createtime")==0){
                            resp.mutable_user_info()->set_createtime(res->element[i+1]->str);
                        }
                    }
                    sendRespondData(resp);
                    return;
                }
            }
            std::string sql=fmt::format("select * from userinfo where account = {};", req.user_info().user_id());
            AsyncExecutorPool::getInstance().addMysqlTask(sql, [this](std::shared_ptr<MYSQL> res){
                PullRespond resp;
                MYSQL_RES *result=mysql_store_result(res.get());
                MYSQL_ROW row;
                while((row=mysql_fetch_row(result))){
                    resp.mutable_user_info()->set_account(row[1]);
                    resp.mutable_user_info()->set_name(row[2]);
                    resp.mutable_user_info()->set_email(row[4]);
                    resp.mutable_user_info()->set_image(row[5]==NULL?"\"\"":row[5]);
                    resp.mutable_user_info()->set_createtime(row[6]);
                }
                sendRespondData(resp);
                mysql_free_result(result);
            });
        });

    } else if(req.has_friend_list()){
        std::string sql=fmt::format("select friend_id from friends where user_id = {} and status = 1;", req.friend_list().user_id());
        AsyncExecutorPool::getInstance().addMysqlTask(sql, [this](std::shared_ptr<MYSQL> res){
            PullRespond resp;
            MYSQL_RES *result=mysql_store_result(res.get());
            MYSQL_ROW row;
            while((row=mysql_fetch_row(result))){
                resp.mutable_friend_list()->add_user_id(row[0]);
            }
            sendRespondData(resp);
            mysql_free_result(result);
        });
    } else if(req.has_communication()){
        std::shared_ptr<Session> tar=AsyncServer::getInstance()->getClient(req.communication().target_id());
        if(tar!=nullptr){
            PullRespond res;
            res.mutable_server_msg()->mutable_communication()->set_user_id(req.communication().target_id());
            res.mutable_server_msg()->mutable_communication()->set_target_id(req.communication().user_id());
            res.mutable_server_msg()->mutable_communication()->set_content(req.communication().content());
            res.mutable_server_msg()->mutable_communication()->set_message_id(req.communication().message_id());
            res.mutable_server_msg()->mutable_communication()->set_msg_type(req.communication().msg_type());
            res.mutable_server_msg()->mutable_communication()->set_md5(req.communication().md5());
            tar->sendRespondData(res);
       
            PullRespond resp;
            resp.mutable_communication()->set_message_id(req.communication().message_id());
            resp.mutable_communication()->set_result(true);
            sendRespondData(resp);

        } else {
            std::string message_id=req.communication().message_id();
            std::string sql=fmt::format("insert into offline_messages(message_id,sender_id,receiver_id,message,status,md5_hash)"
                "value('{}',{},{},'{}',{},'{}');", message_id,req.communication().user_id(),
                req.communication().target_id(),req.communication().content(),req.communication().msg_type(),req.communication().md5());
            AsyncExecutorPool::getInstance().addMysqlTask(sql, [this,message_id](std::shared_ptr<MYSQL> res){
                uint64_t resNum=mysql_affected_rows(res.get());
                PullRespond resp;
                resp.mutable_communication()->set_message_id(message_id);
                resp.mutable_communication()->set_result(resNum!=0);
                sendRespondData(resp);
            });
        }
        if(req.communication().msg_type()==2){
            AsyncExecutorPool::getInstance().addMysqlTask(fmt::format("insert IGNORE into images(md5_hash,image_data) "
            "value('{}','{}');", req.communication().md5(),req.communication().content()), nullptr);
        AsyncExecutorPool::getInstance().addRedisTask(fmt::format("set {} {}",req.communication().md5(),req.communication().content()),nullptr);
        }

    } else if(req.has_offline_request()){
        std::string account=req.offline_request().user_id();
        std::string sql=fmt::format("select sender_id,message_id,message,created_at,status,md5_hash from "
            "offline_messages where receiver_id={} order by id asc;", account);

        AsyncExecutorPool::getInstance().addMysqlTask(sql, [this,account](std::shared_ptr<MYSQL> res){
            MYSQL_RES *result=mysql_store_result(res.get());
            PullRespond resp;
            MYSQL_ROW row;
            while((row=mysql_fetch_row(result))){
                offline_info *info=resp.mutable_offline_respond()->add_info();
                info->set_message(row[2]);
                info->set_message_id(row[1]);
                info->set_friend_id(row[0]);
                info->set_timestemp(row[3]);
                info->set_msg_type(atoi(row[4]));
                info->set_md5(row[5]);
            }
            std::string sql=fmt::format("delete from "
                "offline_messages where receiver_id={};", account);
            AsyncExecutorPool::getInstance().addMysqlTask(sql, [this,resp=std::move(resp)](std::shared_ptr<MYSQL> res){
                
                PullRespond toSend=resp;
                sendRespondData(toSend);
            });
        });
    } else if(req.has_image_request()) {
        std::string command=fmt::format("get {}",req.image_request().md5());
        std::string sql=fmt::format("select image_data from images where md5_hash={};", req.image_request().md5());

        AsyncExecutorPool::getInstance().addRedisTask(command, [this,sql](std::shared_ptr<redisReply> reply){
            if(reply&&reply->type!=REDIS_REPLY_NIL){
                PullRespond resp;
                resp.mutable_image_respond()->set_img_data(reply->str);
                sendRespondData(resp);
            } else {
                AsyncExecutorPool::getInstance().addMysqlTask(sql,[this](std::shared_ptr<MYSQL> res){
                    MYSQL_RES *result=mysql_store_result(res.get());
                    MYSQL_ROW row=mysql_fetch_row(result);
                    PullRespond resp;
                    resp.mutable_image_respond()->set_img_data(row[0]==NULL?"":row[0]);
                    sendRespondData(resp);
                });
            }
        });
    } else if (req.has_add_image()) {
    }
}

void Session::sendRespondData(PullRespond &dataToSend,CustomType type)
{
    if(clientid_!=""&&!isConnect_)
        return;   
    std::string sdata;
    if(!dataToSend.SerializeToString(&sdata)){
        puts("sendRespondData SerializeToString error");
    }

    std::vector<char> buffer(sizeof(packageHead) + sdata.size());
    uint32_t dataSize = static_cast<uint32_t>(sdata.size());
    dataSize = htonl(dataSize);
    packageHead head;
    head.dataSize=dataSize;
    head.type=htonl(static_cast<uint32_t>(type));

    std::memcpy(buffer.data(), &head, sizeof(packageHead));
    std::memcpy(buffer.data() + sizeof(packageHead), sdata.data(), sdata.size());

    auto bufferPtr=std::make_shared<std::vector<char>>(std::move(buffer));

    asio::async_write(socket_,asio::buffer(*bufferPtr),[this,bufferPtr](asio::error_code ec,std::size_t bytes_transferred){
        if(ec){
            std::cout<<"sendRespondData "<<ec.message()<<std::endl;
        }
    });
    
}


std::string Session::getCurrentAccount()
{
    std::string result="";
    if(clientid_!="")
        result=clientid_.substr(0,clientid_.size()-3);
    return result;
}

