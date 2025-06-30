//
// Created by Thrent on 2025/6/28.
//

#include "../../include/cc_tools/network/TcpServer.h"

#define MAX_BUFFER_LENGTH 1024 * 1024 * 500

namespace CC_Tools {
namespace network {

using asio::ip::tcp;

TcpServer::TcpServer(const char* address, short port) {
    this->endpoint = std::make_shared<tcp::endpoint>(asio::ip::make_address(address), port);
    this->acceptor_ = std::make_shared<tcp::acceptor>(this->io_ctx_, *this->endpoint);
    this->socket_ = std::make_shared<tcp::socket>(this->io_ctx_);
}
TcpServer::~TcpServer() {
    release();
}
void TcpServer::start() {
    do_accept();
    if (is_running.exchange(true)) {
        return;
    }
    this->run_thread_ = std::thread([this]() { this->io_ctx_.run(); });
}
void TcpServer::release() {
    if (!is_running.exchange(false))
        return;

    asio::post(io_ctx_, [this]() {
        if (acceptor_ && acceptor_->is_open()) {
            acceptor_->close();
        }
        if (socket_ && socket_->is_open()) {
            socket_->close();
        }
    });

    io_ctx_.stop();
    if (run_thread_.joinable())
        run_thread_.join();
}

bool TcpServer::isRunning() {
    return is_running;
}

void TcpServer::do_accept() {
    acceptor_->async_accept(*socket_, [this](asio::error_code ec)  // 创建一个socket连接
                            {
                                if (!ec) {
                                    int  sessionId = this->allocator_.allocate();
                                    auto newSession = std::make_shared<Session>(
                                        std::move(*socket_), sessionId, onMessage_, onConnected_, onDisconnected_);
                                    newSession->start();
                                }
                                do_accept();
                            });
}

void TcpServer::setMessageHandler(Session::MessageHandler onMessage, Session::OnConnected onConnected,
                                  Session::OnDisconnected onDisconnected) {
    onMessage_ = std::move(onMessage);
    onConnected_ = std::move(onConnected);
    onDisconnected_ = std::move(onDisconnected);
}

Session::Session(asio::ip::tcp::socket socket, int id, MessageHandler handler, OnConnected onConnected,
                 OnDisconnected onDisconnected)
    : socket_(std::move(socket)),
      onMessage_(std::move(handler)),
      onConnected_(std::move(onConnected)),
      onDisconnected_(std::move(onDisconnected)) {
    data_ = std::vector<uint8_t>(1024 * 1024 * 5);
    asio::socket_base::receive_buffer_size option;
    socket_.set_option(asio::socket_base::receive_buffer_size(1024 * 1024 * 5));
    socket_.set_option(asio::socket_base::send_buffer_size(MAX_BUFFER_LENGTH));

    sessionId_ = id;
}

Session::~Session() {
    if (onDisconnected_) {
        onDisconnected_(this);
    }

    if (socket_.is_open()) {
        socket_.close();
    }
}

void Session::start() {
    if (onConnected_) {
        onConnected_(this);
    }
    do_read();
}

void Session::send_to(const uint8_t* data, size_t length) {
    asio::write(socket_, asio::buffer(data, length));
}

void Session::do_read() {
    auto self(shared_from_this());  // 对自己的引用计数+1
    socket_.async_read_some(asio::buffer(data_), [this, self](asio::error_code ec, std::size_t length) {
        if (!ec && length > 0) {
            if (onMessage_) {
                onMessage_(this, data_.data(), length);
            }
            do_read();
        }
    });
}
void Session::do_write(std::size_t length) {
    auto self(shared_from_this());
    asio::async_write(socket_, asio::buffer(data_.data(), length), [this, self](asio::error_code ec, size_t) {
        if (!ec) {
            do_read();
        }
    });
}

std::string Session::getSessionIp() {
    return socket_.remote_endpoint().address().to_string() + ":" + std::to_string(socket_.remote_endpoint().port());
}

int Session::getSessionId() {
    return sessionId_;
}

}  // namespace network
}  // namespace CC_Tools