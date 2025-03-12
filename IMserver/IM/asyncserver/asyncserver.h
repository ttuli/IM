#ifndef _ASYNC_SERVER_H_
#define _ASYNC_SERVER_H_
#include <array>
#include <asio.hpp>
#include <asio/impl/read.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <cstddef>
#include <memory>
#include <mutex>
#include <mysql/mysql.h>
#include <unordered_map>
#include <vector>
#include "../asyncexecutorpool/async_executor_pool.h"
#include "../IM_login/IM_login.pb.h"

using namespace asio::ip;

class Session;

enum CustomType{
    MESSAGE=0,
    HEART=1
};

typedef struct packageHead_t{
    uint32_t type;
    uint32_t dataSize;
}packageHead;

class AsyncServer {
public:
    AsyncServer(const AsyncServer&)=delete;
    AsyncServer operator=(const AsyncServer&)=delete;

    static AsyncServer* getInstance();

    static std::atomic<uint> numOfAccount;

    void addClient(std::string id,std::shared_ptr<Session> client);
    void removeClient(std::string id);
    std::shared_ptr<Session> getClient(std::string id);

    void init(asio::io_context &ioc,short port);
    void destory();

private:
    AsyncServer();
    ~AsyncServer()=default;

    std::unique_ptr<tcp::acceptor> acceptor_;
    std::mutex mutex_;
    void doAccept();

    std::unordered_map<std::string,std::shared_ptr<Session>> clients_;

};

class Session : public std::enable_shared_from_this<Session>{
    public:
        Session(tcp::socket socket,AsyncServer *parent);
        ~Session();

        void startWork();
        void schedule_heartbeat_check();
        void sendRespondData(PullRespond &dataToSend,CustomType type=CustomType::MESSAGE);
        std::string getCurrentAccount();

        static constexpr int heartbeat_interval_ = 4;   // 每5秒发送一次心跳
        static constexpr int heartbeat_timeout_ = 10;   // 15秒无响应视为超时
        bool isConnect_;

        std::chrono::steady_clock::time_point last_heartbeat_time_;
    private:
        tcp::socket socket_;
        asio::steady_timer heartbeat_timer_;

        std::vector<char> data_;
        std::array<char, 4> header_;  // 4字节头部
        uint32_t dataSize_;
        std::string clientid_;

        void parseData(std::size_t length);
    };

#endif