//
// Created by Thrent on 2025/6/28.
//

#include "../../include/cc_tools/network/TcpClient.h"

#define READ_BUFFER_SIZE 8192  // 8KB

namespace CC_Tools {
namespace network {


TcpClient::TcpClient(const std::string& remoteHost, uint16_t remotePort)
    : socket_(io_context_), resolver_(io_context_),
      remote_host_(remoteHost), remote_port_(remotePort),
      read_buffer_(READ_BUFFER_SIZE)
{
    doConnect(remoteHost, remotePort);
}

TcpClient::~TcpClient() {
    if (!isReleased_) {
        release();
    }
}

bool TcpClient::launch() {
    if (isReleased_) {
        throw std::runtime_error("launch error: endpoint is already released");
    }
    if (isRunning_) {
        return true;
    }

    runThread_ = std::thread([this]() {
        try {
            io_context_.run();
        } catch (const std::exception& e) {
            if (on_error_) {
                asio::post(io_context_, [cb=on_error_, msg=std::string(e.what())]() {
                    cb(asio::error_code(), msg.c_str());
                });
            }
        }
    });

    isRunning_ = true;
    return true;
}

bool TcpClient::release() {
    if (!isRunning_ || isReleased_) {
        return true;
    }

    asio::post(io_context_, [self=this]() {
        self->close();
    });

    if (runThread_.joinable()) {
        io_context_.stop();
        runThread_.join();
    }

    io_context_.restart(); // 允许重复 launch
    isReleased_ = true;
    isRunning_ = false;
    return true;
}

void TcpClient::doConnect(const std::string& host, uint16_t port) {
    auto self = this;
    resolver_.async_resolve(host, std::to_string(port),
        [this, self](const asio::error_code& ec, asio::ip::tcp::resolver::results_type results) {
            if (ec) {
                handleError(ec);
                return;
            }

            asio::async_connect(socket_, results,
                [this, self](const asio::error_code& ec, const asio::ip::tcp::endpoint&) {
                    if (ec) {
                        handleError(ec);
                        return;
                    }

                    if (on_connect_) {
                        on_connect_();
                    }
                    doRead();
                });
        });
}

void TcpClient::send(const char* data, size_t length) {
    if (isReleased_ || !socket_.is_open()) return;

    auto buffer = std::make_shared<std::vector<char>>(data, data + length);
    auto self = this;
    asio::async_write(socket_, asio::buffer(*buffer),
        [this, self, buffer](const asio::error_code& ec, std::size_t /*length*/) {
            if (ec) {
                handleError(ec);
            }
        });
}

void TcpClient::send(const std::string& str) {
    send(str.data(), str.size());
}

std::string TcpClient::getLocalIP() const {
    if (socket_.is_open()) {
        asio::error_code ec;
        auto endpoint = socket_.local_endpoint(ec);
        if (!ec) {
            return endpoint.address().to_string() + ":" + std::to_string(endpoint.port());
        }
        throw std::runtime_error(ec.message());
    }
    throw std::runtime_error("socket is not open");
}

std::string TcpClient::getRemoteIP() const {
    if (socket_.is_open()) {
        asio::error_code ec;
        auto endpoint = socket_.remote_endpoint(ec);
        if (!ec) {
            return endpoint.address().to_string() + ":" + std::to_string(endpoint.port());
        }
        throw std::runtime_error(ec.message());
    }
    throw std::runtime_error("socket is not open");
}

void TcpClient::doRead() {
    auto self = this;
    socket_.async_read_some(asio::buffer(read_buffer_),
        [this, self](const asio::error_code& ec, std::size_t length) {
            if (ec) {
                handleError(ec);
                return;
            }

            if (on_receive_) {
                on_receive_(read_buffer_.data(), length);
            }
            doRead();
        });
}

void TcpClient::close(bool notify) {
    asio::error_code ec;
    if (socket_.is_open()) {
        socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
        socket_.close(ec);
    }

    if (notify && on_disconnect_) {
        on_disconnect_();
    }
}

void TcpClient::handleError(const asio::error_code& ec) {
    if (ec == asio::error::eof || ec == asio::error::connection_reset || ec == asio::error::connection_aborted) {
        if (on_disconnect_) {
            on_disconnect_();
        }
    } else {
        if (on_error_) {
            on_error_(asio::error_code(), ec.message());
        }
    }
    close(false);
}

}  // namespace network
}  // namespace CC_Tools
