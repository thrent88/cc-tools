//
// Created by Thrent on 2025/6/29.
//

#include "cc_tools/network/TcpClient.h"
#include "cc_tools/network/TcpServer.h"
#include "cc_tools/network/UdpEndpoint.h"
#include "gtest/gtest.h"

using namespace CC_Tools::network;

TEST(CCNetworkTest, UdpEndpoint) {
    UdpEndpoint endpoint("127.0.0.1", 1234);
    endpoint.bind("127.0.0.1", 1235);

    int count = 0;
    while (true) {
        if (count > 10) {
            break;
        }
        std::string msg = "hello";
        endpoint.send(msg.data(), msg.size());
        ++count;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

TEST(CCNetworkTest, TCPServer) {
    TcpServer server("127.0.0.1", 1234);
    server.setMessageHandler(
        [](std::shared_ptr<Session> session, const std::vector<uint8_t>& data, size_t size) {
            std::cout << "recv:" << data.size() << std::endl;
            session->send_to(data.data(), size);
        },
        [](std::shared_ptr<Session> session) { std::cout << "connected:" << session->getSessionIp() << std::endl; },
        [](Session* session) { std::cout << "disconnected:" << session->getSessionIp() << std::endl; });
    server.start();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

TEST(CCNetworkTest, TCPClient) {
    TcpClient client("127.0.0.1", 1234);
    client.setOnReceive([](const char* data, size_t length) {
        std::cout << "recv:" << length << std::endl;
        std::cout << data << std::endl;
    });

    client.setOnDisconnect([&]() {
        std::cout << client.getLocalIP() << "disconnected" << std::endl;
    });

    client.launch();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::vector<char> dataChar(5050, 0);
    client.send(dataChar.data(), dataChar.size());
    client.send(dataChar.data(), dataChar.size());
    client.send(dataChar.data(), dataChar.size());

    while (1) {

    }
}
