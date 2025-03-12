#include "asyncserver/asyncserver.h"
#include "asyncexecutorpool/mysql_connetion_pool.h"
#include "asyncexecutorpool/async_executor_pool.h"
#include "asyncexecutorpool/redis_connection_pool.h"
#include <asio/io_context.hpp>
#include <csignal>
#include <cstdlib>
#include <fmt/core.h>
#include <iostream>
#define PORT 50051

std::atomic<uint> AsyncServer::numOfAccount=0;
asio::io_context io_context;

void stopServer(int signal)
{
    AsyncExecutorPool::getInstance().destory();
    MySQLConnectionPool::getInstance().destroy();
    redisConnectionPool::getInstance().destory();
    AsyncServer::getInstance()->destory();
    io_context.stop();
    puts("");
    exit(0);
}

int main()
{
    signal(SIGINT,stopServer);
    signal(SIGTERM, stopServer);
    try {
        redisConnectionPool::getInstance().init("47.96.69.27",6379, 1);
        MySQLConnectionPool::getInstance().Initialize("47.96.69.27", 
            "koung", "zihang623", "IM", 3306, 2);
        AsyncExecutorPool::getInstance().Initialize(2);
        AsyncServer::getInstance()->init(io_context, PORT);
        puts("server start");
        io_context.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        stopServer(0);
    }

    return 0;
}