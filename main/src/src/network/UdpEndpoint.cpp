//
// Created by Thrent on 2025/6/28.
//

#include "../../include/cc_tools/network/UdpEndpoint.h"

#include <utility>

namespace CC_Tools {
namespace network {

UdpEndpoint::UdpEndpoint(std::string remoteHost, int remotePort)
    : remoteHost_(std::move(remoteHost)), remotePort_(remotePort) {
    this->recvBuffer_ = std::vector<char>(recvBufferSize_);

    // 设置本地端点
    localEndpoint_ = std::make_unique<asio::ip::udp::endpoint>(asio::ip::make_address(bindHost_), bindPort_);
    this->socket_ = std::make_unique<asio::ip::udp::socket>(ioContext_, *localEndpoint_);

    // 设置远程端点
    remoteEndpoint_ = std::make_unique<asio::ip::udp::endpoint>(asio::ip::make_address(remoteHost_), remotePort_);

    asio::socket_base::receive_buffer_size option(recvBufferSize_);
    socket_->set_option(option);
    this->socket_->set_option(asio::socket_base::broadcast(true));

    startRecvData();
}

UdpEndpoint::~UdpEndpoint() {
    if (!isRelease_) {
        this->release();
    }
}

bool UdpEndpoint::launch() {
    if (isRelease_) {
        throw std::runtime_error("launch error: endpoint is already release");
    }

    if (isRunning_) {
        return true;
    }

    this->runThread_ = std::thread([this]() { this->ioContext_.run(); });
    isRunning_ = true;
    return true;
}

bool UdpEndpoint::release() {
    if (!isRunning_ || isRelease_) {
        return true;
    }

    asio::post(this->ioContext_, [this]() {
        this->socket_->shutdown(asio::socket_base::shutdown_both);
        this->socket_->close();
        this->ioContext_.stop();
    });

    if (this->runThread_.joinable()) {
        this->runThread_.join();
    }

    isRelease_ = true;
    isRunning_ = false;
    return true;
}

void UdpEndpoint::bind(std::string bindHost, int bindPort) {
    if (isRunning_) {
        throw std::runtime_error("bind error: endpoint is running");
    }

    this->bindHost_ = std::move(bindHost);
    this->bindPort_ = bindPort;

    localEndpoint_ = std::make_unique<asio::ip::udp::endpoint>(asio::ip::make_address(bindHost_), bindPort_);
}

size_t UdpEndpoint::send(char* data, size_t size) const {
    return this->socket_->send_to(asio::buffer(data, size), *this->remoteEndpoint_);
}

size_t UdpEndpoint::sendTo(char* data, size_t size, std::string host, int port) const {
    asio::ip::udp::endpoint endpoint(asio::ip::make_address(host), port);
    return this->socket_->send_to(asio::buffer(data, size), endpoint);
}

void UdpEndpoint::setOnRecvMessage(OnReceiveMessage onReceiveMessage) {
    this->onReceiveMessage_ = std::move(onReceiveMessage);
}

void UdpEndpoint::setReceiveBufferSize(size_t size) {
    this->recvBuffer_ = std::vector<char>(size);
    this->recvBufferSize_ = size;
    asio::socket_base::receive_buffer_size size_option(recvBufferSize_);
    socket_->set_option(size_option);
}

void UdpEndpoint::setSendBufferSize(size_t size) {
    this->sendBufferSize_ = size;
    asio::socket_base::send_buffer_size size_option(sendBufferSize_);
    socket_->set_option(size_option);
}

asio::ip::udp::socket* UdpEndpoint::getSocket() {
    return socket_.get();
}

void UdpEndpoint::startRecvData() {
    this->socket_->async_receive_from(asio::buffer(recvBuffer_), *this->remoteEndpoint_,
                                      [this](std::error_code ec, std::size_t bytes_transferred) {
                                          if (!ec) {
                                              if (onReceiveMessage_) {
                                                  onReceiveMessage_(recvBuffer_.data(), bytes_transferred);
                                              }
                                          }
                                          startRecvData();
                                      });
}
}  // namespace network
}  // namespace CC_Tools