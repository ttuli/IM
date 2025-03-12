#include "async_executor_pool.h"
#include "redis_connection_pool.h"
#include <cstdint>
#include <fmt/core.h>
#include <hiredis/hiredis.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <mysql/mysql.h>
#include <vector>


 void AsyncExecutorPool::Initialize(uint8_t thrd_num)
 {
    stop_=false;
    for(uint8_t i=0;i<thrd_num;++i){
        workers_.emplace_back(
            &AsyncExecutorPool::WorkerThread, this);
    }
}

AsyncExecutorPool& AsyncExecutorPool::getInstance()
{
    static AsyncExecutorPool obj;
    return obj;
}

void AsyncExecutorPool::destory()
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        stop_ = true;
    }
    condition_.notify_all();
    for (auto& worker : workers_) {
        worker.join();
    }
}

void AsyncExecutorPool::addMysqlTask(const std::string& sql, MySQLCallback callback)
{
    std::lock_guard<std::mutex> lock(mutex_);

    tasks_.push({TaskType::QUERY,
    [callback,sql]{
        auto connection=MySQLConnectionPool::getInstance().getConnection();
        if(mysql_query(connection.get(), sql.data())){
            std::cerr << "MySQL query failed: " << mysql_error(connection.get()) << std::endl;
            if(sql.size()<200)
                std::cout<<sql<<std::endl;
            return;
        }

        if(callback!=nullptr)
            callback(connection);
    }});

    condition_.notify_one();
}

void AsyncExecutorPool::addWriteTask(WriteCallback callback)
{
    if(callback==nullptr)
        return;
    std::lock_guard<std::mutex> lock(mutex_);

    tasks_.push({TaskType::SEND,callback});

    condition_.notify_one();
}

void AsyncExecutorPool::mysqlTransaction(std::vector<std::string> sqls,mysqlTransactionResult callback)
{
    std::lock_guard<std::mutex> lock(mutex_);

    tasks_.push({TaskType::QUERY,
    [callback,sqls]{
        bool success=true;
        auto connection=MySQLConnectionPool::getInstance().getConnection();
        mysql_autocommit(connection.get(),false);

        for(int i=0;i<sqls.size();++i){
            if(mysql_query(connection.get(),sqls[i].data())){
                success=false;
                std::cerr << "mysqlTransaction: " << mysql_error(connection.get()) << std::endl;
                break;
            }
        }

        if (success) {
            mysql_commit(connection.get());
        } else {
            mysql_rollback(connection.get());
        }

        mysql_autocommit(connection.get(),true);
        if(callback!=nullptr)
            callback(success);
    }});

    condition_.notify_one();
}

void AsyncExecutorPool::addRedisTask(const std::string& command, RedisCallback callback)
{
    std::lock_guard<std::mutex> lock(mutex_);

    tasks_.push({TaskType::QUERY,[command,callback]{
        auto connection=redisConnectionPool::getInstance().getConnection();
        redisReply *reply=(redisReply*)redisCommand(connection.get(), command.c_str());

        if(callback!=nullptr)
            callback(std::shared_ptr<redisReply>(reply,freeReplyObject));
        else{
            freeReplyObject(reply);
        }
    }});

    condition_.notify_one();
}

void AsyncExecutorPool::WorkerThread()
{
    while(1){
        task_t task;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock,[this]{return stop_||!tasks_.empty();});
            if(stop_&&tasks_.empty())
                return;
            task=tasks_.front();
            tasks_.pop();
        }
        task.callback();
    }
}