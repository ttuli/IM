#ifndef _ASYNC_EXECUTOR_POOL_H_
#define _ASYNC_EXECUTOR_POOL_H_

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <mysql/mysql.h>
#include <hiredis/hiredis.h>
#include <thread>
#include <vector>
#include "mysql_connetion_pool.h"


using MySQLCallback = std::function<void(std::shared_ptr<MYSQL>)>;
using mysqlTransactionResult = std::function<void(bool)>;
using RedisCallback = std::function<void(std::shared_ptr<redisReply>)>;
using WriteCallback = std::function<void()>;

class AsyncExecutorPool{
public:
    AsyncExecutorPool(const AsyncExecutorPool&) = delete;
    AsyncExecutorPool& operator=(const AsyncExecutorPool&) = delete;

    void Initialize(uint8_t thrd_num);
    void destory();
    static AsyncExecutorPool& getInstance();

    void addMysqlTask(const std::string& sql, MySQLCallback callback);
    void addRedisTask(const std::string& command, RedisCallback callback);
    void addWriteTask(WriteCallback callback);

    void mysqlTransaction(std::vector<std::string> sqls,mysqlTransactionResult callback);

private:
    AsyncExecutorPool() = default;

    enum class TaskType { QUERY,SEND};

    typedef struct task_s{
        TaskType type;
        std::function<void()> callback;
    }task_t;

    void WorkerThread();

    std::vector<std::thread> workers_;
    std::queue<task_t> tasks_;
    std::mutex mutex_;
    std::condition_variable condition_;

    bool stop_;
};

#endif