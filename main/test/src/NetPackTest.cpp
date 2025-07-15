//
// Created by Thrent on 2025/7/15.
//

#include <random>

#include "cc_tools/network/NetPacker.h"
#include "gtest/gtest.h"

using namespace CC_Tools::network;

TEST(NetPackerTest, serialize) {
    std::vector<uint8_t> payload(500000, 0x7A); // 模拟超大数据

    // 打包
    auto packets = NetPacker::pack(payload);

    // 模拟乱序
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(packets.begin(), packets.end(), g);

    // 解包
    NetUnpacker unpacker;
    for (const auto& pkt : packets) {
        unpacker.addPacket(pkt);
    }

    if (unpacker.isComplete()) {
        auto recovered = unpacker.getPayload();
        std::cout << "✅ 解包成功，数据长度: " << recovered.size() << "\n";
    } else {
        std::cout << "❌ 数据不完整，缺少包:\n";
        for (auto idx : unpacker.getMissingIndices()) {
            std::cout << "缺少分片 #" << idx << "\n";
        }
    }
}
