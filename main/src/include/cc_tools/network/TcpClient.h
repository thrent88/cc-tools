//
// Created by Thrent on 2025/6/28.
//

#ifndef CC_Tools_TCPCLIENT_H
#define CC_Tools_TCPCLIENT_H

#include <asio.hpp>
#include <thread>
#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace CC_Tools {
namespace network {

class TcpClient {
public:
    using ReceiveCallback   = std::function<void(const char*, size_t)>;
    using ConnectCallback   = std::function<void()>;
    using DisconnectCallback= std::function<void()>;
    using ErrorCallback     = std::function<void(const asio::error_code&, std::string)>;

    TcpClient(const std::string& remoteHost, uint16_t remotePort);
    ~TcpClient();

    // 禁止拷贝
    TcpClient(const TcpClient&) = delete;
    TcpClient& operator=(const TcpClient&) = delete;

    // 启动 / 释放
    bool launch();
    bool release();

    // 发送数据
    void send(const char* data, size_t length);
    void send(const std::string& str);

    // 获取本地/远程地址
    std::string getLocalIP() const;
    std::string getRemoteIP() const;

    // 设置回调
    void setOnReceive(ReceiveCallback cb)   { on_receive_ = std::move(cb); }
    void setOnConnect(ConnectCallback cb)   { on_connect_ = std::move(cb); }
    void setOnDisconnect(DisconnectCallback cb) { on_disconnect_ = std::move(cb); }
    void setOnError(ErrorCallback cb)       { on_error_ = std::move(cb); }

private:
    void doConnect(const std::string& host, uint16_t port);
    void doRead();
    void close(bool notify = true);
    void handleError(const asio::error_code& ec);

    mutable asio::io_context io_context_;
    asio::ip::tcp::socket socket_;
    asio::ip::tcp::resolver resolver_;
    std::thread runThread_;
    std::vector<char> read_buffer_;

    std::string remote_host_;
    uint16_t remote_port_;

    std::atomic<bool> isRunning_{false};
    std::atomic<bool> isReleased_{false};

    ReceiveCallback on_receive_;
    ConnectCallback on_connect_;
    DisconnectCallback on_disconnect_;
    ErrorCallback on_error_;
};

} // network
} // CC_Tools

#endif //CC_Tools_TCPCLIENT_H
