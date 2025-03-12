#include "networkmanager.h"
#include "friendAgroup/friendlistmodel.h"
#include "ChatState/chatlistmodel.h"
#include "imagemanager.h"
#include "sqlitemanager.h"
#include <QQueue>
#include <string>
#include <QtEndian>
#include <QDate>
#include <QMediaPlayer>
#include <QAudioOutput>

NetWorkManager::NetWorkManager(QObject *parent)
    : QObject{parent}
{
    currentAccount_="";
    init();
}

QString NetWorkManager::addTask(Task task)
{
    QString id=QDateTime::currentDateTime().toString("hh:mm:ss");
    task.id_=id;
    taskQueue_.enqueue(task);
    if(msgSocket_->state()==QAbstractSocket::UnconnectedState)
        msgSocket_->connectToHost(SERVER_ADDR,PORT);
    execTask();
    return id;
}

bool NetWorkManager::removeTask(QString id)
{
    if(id=="top"&&taskQueue_.size()){
        taskQueue_.dequeue();
        return true;
    }
    for(int i=0;i<taskQueue_.size();++i){
        if(taskQueue_.at(i).id_==id){
            taskQueue_.removeAt(i);
            return true;
        }
    }
    return false;
}

QByteArray NetWorkManager::mergeData(PullRequest &data)
{
    std::string sdata;
    data.SerializeToString(&sdata);

    QByteArray header;
    QDataStream stream(&header,QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream<<((quint32)sdata.size());

    return header+QByteArray::fromStdString(sdata);
}

User NetWorkManager::getInfo(QString id)
{
    if(cache_.contains(id)){
        return cache_[id];
    }
    return User();
}

void NetWorkManager::setInfo(QString id, User u)
{
    cache_[id]=u;
    emit updateInfoSig(id);
}

void NetWorkManager::setCurrentTargetAccount(QString account)
{
    currentTargetAccount_=account;
}

void NetWorkManager::setIsUnVisible(bool value)
{
    isUnVisible_=value;
}

void NetWorkManager::addChatDiaModel(QString id,QSharedPointer<ChatDialogModel> model)
{
    chatDiaModelList_[id]=model;
}

void NetWorkManager::removeChatDiaModel(QString id)
{
    chatDiaModelList_.remove(id);
}

void NetWorkManager::resetAllModel()
{
    for(auto i:chatDiaModelList_){
        i->resetAll();
    }
}

void NetWorkManager::addOfflineMsg(QString id, QSharedPointer<message> info)
{
    offlineMsg_[id].append(info);
}

QList<QSharedPointer<message>> NetWorkManager::getOfflineMsg(QString id)
{
    QList<QSharedPointer<message>> list=offlineMsg_[id];
    offlineMsg_.remove(id);
    return list;
}

void NetWorkManager::execTask()
{
    if(msgSocket_->state()!=QAbstractSocket::ConnectedState
        ||taskQueue_.empty()||taskQueue_.front().fetch_)
        return;
    taskQueue_.head().fetch_=true;

    msgSocket_->write(taskQueue_.front().data_);
}

void NetWorkManager::sendClientId(QString account)
{
    PullRequest request;
    request.mutable_client_id()->set_id(account.toStdString()+"msg");
    std::string sdata;
    request.SerializeToString(&sdata);


    QByteArray header;
    QDataStream stream(&header,QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream<<((quint32)sdata.size());

    if(msgSocket_->state()==QAbstractSocket::ConnectedState){
        msgSocket_->write(header+QByteArray::fromStdString(sdata));
    }
}

NetWorkManager::~NetWorkManager()
{
    if(msgSocket_)
        msgSocket_->abort();
}

NetWorkManager *NetWorkManager::getInstance()
{
    static NetWorkManager obj;
    return &obj;
}

void NetWorkManager::init()
{
    msgSocket_=new QTcpSocket(this);
    connect(msgSocket_,&QTcpSocket::connected,[=]{
        if(currentAccount_!="")
            sendClientId(currentAccount_);
        taskQueue_.clear();
        emit initResult(true);
        execTask();
    });
    connect(msgSocket_,&QTcpSocket::disconnected,[=]{
        emit initResult(false);
        if(!taskQueue_.empty())
            taskQueue_.head().fetch_=false;
    });
    connect(msgSocket_,&QTcpSocket::errorOccurred,[=](QAbstractSocket::SocketError err){
        if(msgSocket_->state()!=QAbstractSocket::ConnectedState&&msgSocket_->state()!=QAbstractSocket::ConnectingState)
            msgSocket_->connectToHost(SERVER_ADDR,PORT);
    });
    connect(msgSocket_,&QTcpSocket::readyRead,this,&NetWorkManager::handleRespond);
    msgSocket_->connectToHost(SERVER_ADDR,PORT);
}

void NetWorkManager::handleRespond()
{
    if(msgSocket_->bytesAvailable()<sizeof(packageHead))
        return;

    if(tempTask_.receiveDataSize_==0){
        QByteArray header=msgSocket_->read(sizeof(packageHead));
        const char* data = header.data();
        tempTask_.type_= qFromBigEndian<uint32_t>(reinterpret_cast<const uchar*>(data));
        tempTask_.receiveDataSize_ = qFromBigEndian<uint32_t>(reinterpret_cast<const uchar*>(data+4));
    }
    qint64 availbleByte=msgSocket_->bytesAvailable();
    qint64 resByte=tempTask_.receiveDataSize_-tempTask_.byteArray_.size();

    tempTask_.byteArray_+=msgSocket_->read((availbleByte>=resByte?resByte:availbleByte));

    if(tempTask_.byteArray_.size()==tempTask_.receiveDataSize_){
        processRespond();
    }
}

void NetWorkManager::processRespond()
{
    if(tempTask_.type_==CustomType::HEART){
        PullRequest req;
        req.mutable_heart();
        if(req.has_heart()){
            QByteArray data=NetWorkManager::getInstance()->mergeData(req);
            msgSocket_->write(data);
        }
    } else {
        if(taskQueue_.isEmpty()){
            PullRespond resp;
            resp.ParseFromString(tempTask_.byteArray_.toStdString());
            if(resp.has_server_msg()){
                const ServerMsg &msg=resp.server_msg();
                if(msg.has_friend_add()){
                    applyInfo::applyType type=applyInfo::applyType::VERIFY;

                    QString currentDate=QDate::currentDate().toString("yyyy-MM-dd");
                    QString lastDate=FriendApplyModel::getInstance()->getLastTime();
                    if(currentDate!=lastDate)
                        lastDate=currentDate;

                    FriendApplyModel::getInstance()->removeRow(0);
                    FriendApplyModel::getInstance()->waitForInfomation(QString::fromStdString(msg.friend_add().user_id()),applyInfo::VERIFY);
                    FriendApplyModel::getInstance()->addAtHead(QSharedPointer<applyInfo>::create(lastDate));
                } else if(msg.has_communication()){
                    QString uid=QString::fromStdString(msg.communication().user_id());
                    QString fid=QString::fromStdString(msg.communication().target_id());
                    QString message_id=QString::fromStdString(msg.communication().message_id());
                    message::msgType type=(message::msgType)(msg.communication().msg_type());
                    QString md5=QString::fromStdString(msg.communication().md5());

                    User u=NetWorkManager::getInstance()->getInfo(fid);
                    QString showedMsg=(type==message::IMAGE?md5:QString::fromStdString(msg.communication().content()));

                    if(type==message::IMAGE){
                        ImageManager::getInstance()->addImageCache(md5,QByteArray::fromStdString(msg.communication().content()));
                    }

                    QSharedPointer<message> info=QSharedPointer<message>::create(showedMsg,fid,u.name_,
                            type,QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"),message::OTHER,message_id);
                    if(chatDiaModelList_.contains(fid)){
                        chatDiaModelList_[fid]->addRow(-1,info);
                    } else {
                        offlineMsg_[fid].append(info);
                    }
                    ChatListModel::getinstance()->adjustToTop(fid,type==message::IMAGE?"[图片]":QString::fromStdString(msg.communication().content()),QDateTime::currentDateTime().toString("yyyy-MM-dd"),fid!=currentTargetAccount_);
                    SqliteManager::getInstance()->batchWrite(info,fid);

                    if(fid!=currentTargetAccount_||isUnVisible_){
                        QMediaPlayer *player=new QMediaPlayer;
                        QAudioOutput *output=new QAudioOutput;
                        output->setVolume(80);
                        player->setSource(QUrl("tipVoice.mp3"));

                        connect(player,&QMediaPlayer::playingChanged,[player,output]{
                            if(!player->isPlaying()){
                                player->deleteLater();
                                output->deleteLater();
                            }
                        });
                        player->setAudioOutput(output);
                        player->play();
                    }
                }
            } else if(resp.has_update_rela()){
                QString uid=QString::fromStdString(resp.update_rela().user_id());
                QString fid=QString::fromStdString(resp.update_rela().friend_id());
                bool result=resp.update_rela().result();

                applyInfo::applyType ty=(result?applyInfo::applyType::ACCEPT:applyInfo::applyType::REJECT);

                FriendApplyModel::getInstance()->updateState(fid,ty);
                if(result){
                    FriendListModel::getInstance()->waitForInfomation(fid);
                    ChatListModel::getinstance()->waitForInfomation(fid,"","");
                }
            }
        } else {
            if(taskQueue_.front().callback_!=nullptr){
                auto dataPtr = std::make_shared<QByteArray>(tempTask_.byteArray_);
                taskQueue_.front().callback_(*dataPtr);
            }
            taskQueue_.dequeue();
        }
    }
    tempTask_.byteArray_.clear();
    tempTask_.receiveDataSize_=0;
    execTask();
}
