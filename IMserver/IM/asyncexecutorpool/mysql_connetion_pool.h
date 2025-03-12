#ifndef _MYSQL_CONNECTION_POOL_H_
#define _MYSQL_CONNECTION_POOL_H_

#include <cstdint>
#include <mysql/mysql.h>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

class MySQLConnectionPool{
public:
    MySQLConnectionPool(const MySQLConnectionPool&) = delete;
    MySQLConnectionPool& operator=(const MySQLConnectionPool&) = delete;

    static MySQLConnectionPool& getInstance();

    void Initialize(const char* host, const char* user,
                  const char* password, const char* database,
                  short port, uint8_t pool_size);

    std::shared_ptr<MYSQL> getConnection();

    void destroy();

private:
    MySQLConnectionPool() = default;


    std::queue<MYSQL*> connections_;
    std::mutex mutex_;
    std::condition_variable condition_;
};

#endif