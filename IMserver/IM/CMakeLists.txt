cmake_minimum_required (VERSION 3.10)

project(IM)


add_executable(server
    server.cc
    asyncexecutorpool/async_executor_pool.cc
    IM_login/IM_login.pb.cc
    asyncserver/asyncserver.cc
    asyncexecutorpool/mysql_connetion_pool.cc
    asyncexecutorpool/redis_connection_pool.cc
)

include_directories(
    asyncexecutorpool
    IM_login
    asyncserver
)

find_package(Protobuf REQUIRED)
find_package(fmt REQUIRED)
set(MYSQL_INCLUDE_DIR /usr/include/mysql)   # MySQL 头文件路径
set(MYSQL_LIBRARY /usr/lib/x86_64-linux-gnu/libmysqlclient.so)

set(REDIS_INCLUDE_DIR /usr/include/hiredis)
set(REDIS_LIBRARY /usr/lib/x86_64-linux-gnu/libhiredis.so)


include_directories(${MYSQL_INCLUDE_DIR})
include_directories(${REDIS_INCLUDE_DIR})

target_link_libraries(server PUBLIC
    protobuf
    fmt
)

target_link_libraries(server PUBLIC ${MYSQL_LIBRARY})
target_link_libraries(server PUBLIC ${REDIS_LIBRARY})
