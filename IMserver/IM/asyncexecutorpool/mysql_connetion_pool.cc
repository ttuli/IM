#include "mysql_connetion_pool.h"
#include <cstdint>
#include <mutex>
#include <mysql/mysql.h>

MySQLConnectionPool& MySQLConnectionPool::getInstance()
{
    static MySQLConnectionPool instance;
    return instance;
}

std::shared_ptr<MYSQL> MySQLConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(mutex_);
    while(connections_.empty()){
        condition_.wait(lock);
    }

    MYSQL *connection=connections_.front();
    connections_.pop();

    return std::shared_ptr<MYSQL>(connection,
        [this](MYSQL* conn){
            std::lock_guard<std::mutex> lock(mutex_);
            connections_.push(conn);
            condition_.notify_one();
    });
}

void MySQLConnectionPool::destroy()
{
    std::lock_guard<std::mutex> lock(mutex_);
    while(!connections_.empty()){
        MYSQL *sock=connections_.front();
        connections_.pop();
        mysql_close(sock);
    }
}

void MySQLConnectionPool::Initialize(const char* host, 
        const char* user,
        const char* password, 
        const char* database,
        short port, uint8_t pool_size)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!connections_.empty()) {
        throw std::runtime_error("Connection pool already initialized");
    }

    for(uint8_t i=0;i<pool_size;++i){
        MYSQL* connection = mysql_init(nullptr);

        if (!connection) 
            throw std::runtime_error("Failed to initialize MySQL connection");
            
        if(!mysql_real_connect(connection, host, user, 
            password, database, port, nullptr, 0)){
            
            throw std::runtime_error("Failed to connect to MySQL: " +
                            std::string(mysql_error(connection)));
        }

        connections_.push(connection);
    }

}
