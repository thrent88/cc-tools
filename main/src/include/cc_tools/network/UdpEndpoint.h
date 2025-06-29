//
// Created by Thrent on 2025/6/28.
//

#ifndef CC_Tools_UDPENDPOINT_H
#define CC_Tools_UDPENDPOINT_H

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <thread>

#include "asio.hpp"

namespace CC_Tools {
namespace network {

class UdpEndpoint {
public:
    using OnReceiveMessage = std::function<void(char* data, size_t size)>;

    UdpEndpoint(std::string remoteHost, int remotePort);

    ~UdpEndpoint();

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

    /**
     * 绑定
     * @param bindHost
     * @param bindPort
     */
    void bind(std::string bindHost, int bindPort);

    /**
     * 发送数据
     * @param data
     * @param size
     * @return
     */
    size_t send(char* data, size_t size) const;

    /**
     * 向指定主机发送数据
     * @param data
     * @param size
     * @param host
     * @param port
     * @return
     */
    size_t sendTo(char* data, size_t size, std::string host, int port) const;

    /**
     * 设置接收数据的回调
     * @param onReceiveMessage
     */
    void setOnRecvMessage(OnReceiveMessage onReceiveMessage);

    /**
     * 设置接收缓存的大小
     * @param size
     */
    void setReceiveBufferSize(size_t size);

    /**
     * 设置发送缓冲区大小
     * @param size
     */
    void setSendBufferSize(size_t size);

    /**
     * 获取socket
     * @return
     */
    asio::ip::udp::socket* getSocket();

private:
    /**
     * 开始接收数据
     */
    void startRecvData();

    std::string remoteHost_;  // 远程的IP
    int         remotePort_;  // 远程的端口

    std::string bindHost_ = "127.0.0.1";  // 绑定的IP
    int         bindPort_ = 8888;         // 绑定的端口

    OnReceiveMessage onReceiveMessage_;  // 接收到数据的回调

    std::atomic_bool isRunning_ = false;  // 是否正在运行
    std::thread      runThread_;
    std::atomic_bool isRelease_ = false;  // 是否已经释放

    asio::io_context                         ioContext_;
    std::unique_ptr<asio::ip::udp::endpoint> remoteEndpoint_;  // 远程端点
    std::unique_ptr<asio::ip::udp::endpoint> localEndpoint_;   // 本地端点
    std::vector<char>                        recvBuffer_;
    std::unique_ptr<asio::ip::udp::socket>   socket_;
    size_t                                   recvBufferSize_ = 1024;
    size_t                                   sendBufferSize_ = 1024;
};

}  // namespace network
}  // namespace CC_Tools

#endif  // CC_Tools_UDPENDPOINT_H
