//
// Created by Thrent on 2025/6/28.
//

#ifndef CC_Tools_TCPCLIENT_H
#define CC_Tools_TCPCLIENT_H

#include <asio.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace CC_Tools {
namespace network {

class TcpClient {
public:
    using OnConnect = std::function<void()>;
    using OnReceive = std::function<void(const char* data, size_t length)>;
    using OnError   = std::function<void(const asio::error_code&)>;
    using OnDisconnect = std::function<void()>;

    TcpClient(std::string remoteHost, int remotePort);

    ~TcpClient();

    /**
     * 启动
     * @return
     */
    bool launch();

    /**
     * 释放
     * @return
     */
    bool release();

    void send(const char* data, size_t length);

    /**
     * 获取本地IP
     * @return
     */
    std::string getLocalIP() const;

    /**
     * 获取服务器IP
     * @return
     */
    std::string getRemoteIP() const;

    inline void setOnConnect(OnConnect handler) { on_connect_ = std::move(handler); }
    inline void setOnReceive(OnReceive handler) { on_receive_ = std::move(handler); }
    inline void setOnError(OnError handler)     { on_error_ = std::move(handler); }
    inline void setOnDisconnect(OnDisconnect handler) { on_disconnect_ = std::move(handler); }

private:
    void doRead();
    void handleError(const asio::error_code& ec);
    void close();
    void connect(const std::string& host, uint16_t port);

    asio::io_context io_context_;
    asio::ip::tcp::socket socket_;
    asio::ip::tcp::resolver resolver_;
    std::array<char, 1024> read_buffer_;

    std::atomic_bool isRunning_ = false;  // 是否正在运行
    std::thread      runThread_;
    std::atomic_bool isRelease_ = false;  // 是否已经释放

    OnConnect on_connect_;
    OnReceive on_receive_;
    OnDisconnect on_disconnect_;
    OnError on_error_;
};

} // network
} // CC_Tools

#endif //CC_Tools_TCPCLIENT_H
