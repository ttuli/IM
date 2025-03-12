#include "redis_connection_pool.h"
#include <cstdint>
#include <hiredis/hiredis.h>
#include <memory>
#include <mutex>

redisConnectionPool& redisConnectionPool::getInstance()
{
    static redisConnectionPool obj;
    return obj;
}

void redisConnectionPool::init(const std::string& host, int port, uint8_t poolSize)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!connections_.empty()) {
        throw std::runtime_error("Connection pool already initialized");
    }

    for(uint8_t i=0;i<poolSize;++i){
        redisContext *connection=redisConnect(host.c_str(), port);
        if(connection==nullptr||connection->err){
            if(connection)
                throw std::runtime_error("Failed to connect to Redis:" 
                    + std::string(connection->errstr));
            else
                throw std::runtime_error("Failed to allocate Redis context");
        }

        connections_.push(connection);
    }
}

void redisConnectionPool::destory()
{
    std::lock_guard<std::mutex> lock(mutex_);
    while (connections_.size()) {
        redisContext *sock=connections_.front();
        connections_.pop();
        redisFree(sock);
    }
}

std::shared_ptr<redisContext> redisConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(mutex_);
    while(connections_.empty()){
        condition_.wait(lock);
    }
    redisContext *conn=connections_.front();
    connections_.pop();

    return std::shared_ptr<redisContext>(conn,
            [this](redisContext *conn){
                std::lock_guard<std::mutex> lock(mutex_);
                connections_.push(conn);
                condition_.notify_one();
            });
}