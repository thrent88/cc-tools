//
// Created by Thrent on 2025/6/28.
//

#include "../../include/cc_tools/network/TcpClient.h"

namespace CC_Tools {
namespace network {

TcpClient::TcpClient(std::string remoteHost, int remotePort) : socket_(io_context_), resolver_(io_context_) {
    connect(remoteHost, remotePort);
}

TcpClient::~TcpClient() {
    if (!isRelease_) {
        this->release();
    }
}

bool TcpClient::launch() {
    if (isRelease_) {
        throw std::runtime_error("launch error: endpoint is already release");
    }

    if (isRunning_) {
        return true;
    }

    this->runThread_ = std::thread([this]() { this->io_context_.run(); });
    isRunning_ = true;
    return true;
}

bool TcpClient::release() {
    if (!isRunning_ || isRelease_) {
        return true;
    }

    asio::post(this->io_context_, [this]() { close(); });

    if (this->runThread_.joinable()) {
        this->runThread_.join();
    }

    isRelease_ = true;
    isRunning_ = false;
    return true;
}

void TcpClient::connect(const std::string& host, uint16_t port) {
    auto self = this;
    resolver_.async_resolve(host, std::to_string(port),
                            [this, self](const asio::error_code& ec, asio::ip::tcp::resolver::results_type results) {
                                if (ec) {
                                    handleError(ec);
                                    return;
                                }

                                asio::async_connect(
                                    socket_, results,
                                    [this, self](const asio::error_code& ec, const asio::ip::tcp::endpoint&) {
                                        if (ec) {
                                            handleError(ec);
                                            return;
                                        }

                                        if (on_connect_)
                                            on_connect_();
                                        doRead();
                                    });
                            });
}

void TcpClient::send(const char* data, size_t length) {
    auto              self = this;
    std::vector<char> dataVec(data, data + length);
    asio::async_write(socket_, asio::buffer(dataVec), [this, self](const asio::error_code& ec, std::size_t /*length*/) {
        if (ec) {
            handleError(ec);
        }
    });
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
    socket_.async_read_some(asio::buffer(read_buffer_), [this, self](const asio::error_code& ec, std::size_t length) {
        if (ec) {
            handleError(ec);
            return;
        }

        if (on_receive_) {
            std::vector<char> data(read_buffer_.begin(), read_buffer_.begin() + length);
            on_receive_(data.data(), data.size());
        }

        doRead();
    });
}

void TcpClient::close() {
    asio::error_code ec;
    socket_.close(ec);
    if (ec && on_error_)
        on_error_(ec);
}

void TcpClient::handleError(const asio::error_code& ec) {
    if (ec == asio::error::eof || ec == asio::error::connection_reset || ec == asio::error::connection_aborted) {
        // 远程关闭连接
        if (on_disconnect_) on_disconnect_();
    } else {
        // 其他错误
        if (on_error_) on_error_(ec);
    }

    close();  // 始终尝试关闭 socket
}

}  // namespace network
}  // namespace CC_Tools
