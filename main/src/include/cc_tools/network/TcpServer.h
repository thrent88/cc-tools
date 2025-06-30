//
// Created by Thrent on 2025/6/28.
//

#ifndef CC_Tools_TCPSERVER_H
#define CC_Tools_TCPSERVER_H

#include <atomic>
#include <memory>
#include <thread>

#include "asio.hpp"
#include "cc_tools/utils/IdAllocator.h"


namespace CC_Tools {
namespace network {

class Session
    : public std::enable_shared_from_this<Session>  // 从enable_shared_from_this继承的类需要使用智能指针管理资源
{
public:
    using MessageHandler = std::function<void(std::shared_ptr<Session>, const std::vector<uint8_t>&, size_t)>;
    using OnConnected = std::function<void(std::shared_ptr<Session>)>;
    using OnDisconnected = std::function<void(Session*)>;

    Session(asio::ip::tcp::socket socket, int id, MessageHandler handler, OnConnected onConnected,
            OnDisconnected onDisconnected);

    ~Session();

    /**
     * 开启
     */
    void start();

    /**
     * 发送数据到会话
     * @param data
     * @param length
     */
    void send_to(const uint8_t* data, size_t length);

    /**
     * 获取对方的IP
     * @return
     */
    std::string getSessionIp();

    /**
     * 获取线程ID
     * @return
     */
    int getSessionId();

protected:
    void do_read();

    void do_write(std::size_t length);

    asio::ip::tcp::socket socket_;
    std::vector<uint8_t>  data_;
    MessageHandler        onMessage_;
    OnConnected           onConnected_;
    OnDisconnected        onDisconnected_;

    int sessionId_;
};

class TcpServer {
public:
    TcpServer(const char* address, short port);

    ~TcpServer();

    void start();

    void release();

    /**
     * 设置会话接收回调
     * @param onMessage
     */
    void setMessageHandler(Session::MessageHandler onMessage, Session::OnConnected onConnected = nullptr,
                           Session::OnDisconnected onDisconnected = nullptr);

private:
    /**
     * 握手回调
     */
    void do_accept();

    utils::IdAllocator allocator_;

    std::shared_ptr<asio::ip::tcp::acceptor> acceptor_;
    std::shared_ptr<asio::ip::tcp::socket>   socket_;
    std::shared_ptr<asio::ip::tcp::endpoint> endpoint;

    Session::MessageHandler onMessage_;
    Session::OnConnected    onConnected_;
    Session::OnDisconnected onDisconnected_;

    std::atomic_bool is_running{ false };
    std::thread      run_thread_;

    asio::io_context io_ctx_;
};

}  // namespace network
}  // namespace CC_Tools

#endif  // CC_Tools_TCPSERVER_H
