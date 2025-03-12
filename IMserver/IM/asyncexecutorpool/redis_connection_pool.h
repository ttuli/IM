#ifndef _REDIS_CONNECTION_POOL_H_
#define _REDIS_CONNECTION_POOL_H_

#include <cstdint>
#include <hiredis/hiredis.h>
#include <string>
#include <condition_variable>
#include <mutex>
#include <queue>

class redisConnectionPool{
public:
    redisConnectionPool(const redisConnectionPool&) = delete;
    redisConnectionPool operator=(const redisConnectionPool&) = delete;

    static redisConnectionPool& getInstance();

    void init(const std::string& host, int port, uint8_t poolSize);
    void destory();

    std::shared_ptr<redisContext> getConnection();

private:
    redisConnectionPool() = default;

    std::queue<redisContext*> connections_;
    std::mutex mutex_;
    std::condition_variable condition_;
};



#endif