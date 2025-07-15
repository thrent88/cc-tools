//
// Created by Thrent on 2025/7/15.
//

#ifndef CC_Tools_NETPACKAGE_H
#define CC_Tools_NETPACKAGE_H

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// ========== 平台检测 ==========
#if defined(_WIN32) || defined(_WIN64)
    #include <Winsock2.h>
    #pragma comment(lib, "Ws2_32.lib")
#elif defined(__APPLE__) || defined(__MACH__)
    #include <libkern/OSByteOrder.h>
    #include <arpa/inet.h>
#elif defined(__linux__)
    #include <endian.h>
    #include <arpa/inet.h>
#else
    #error "Unsupported platform"
#endif

#if defined(_MSC_VER)
// MSVC
#define PACKED_STRUCT(name) __pragma(pack(push, 1)) struct name __pragma(pack(pop))
#elif defined(__GNUC__) || defined(__clang__)
// GCC or Clang
#define PACKED_STRUCT(name) struct __attribute__((packed)) name
#else
#error "Unsupported compiler"
#endif

namespace CC_Tools {
namespace network {

// ========== 网络数据打包器 ==========
class NetPacker {
public:
    using ByteVector = std::vector<uint8_t>;
    using NetPackages = std::vector<ByteVector>;

    static constexpr size_t MTU = 1500;  // 最大传输单元

    typedef PACKED_STRUCT(NetPackageHeader) {
        uint16_t sof;                   // 包头标识
        uint64_t totalPayloadSize;      // 数据段总长度
        uint32_t totalPackageNum;       // 分片总数
        uint32_t currentPackageNum;     // 当前分片号
        uint32_t currentPayloadSize;    // 当前分片长度

        NetPackageHeader()
            : sof(0xCE9A), totalPayloadSize(0),
              totalPackageNum(0), currentPackageNum(0), currentPayloadSize(0) {}
    }
    NetPackageHeader;

    static NetPackages pack(const ByteVector& payload) {
        if (payload.empty()) {
            throw std::runtime_error("payload size error: must be > 0");
        }

        size_t headerSize = sizeof(NetPackageHeader);
        size_t maxPayloadSize = MTU - headerSize;

        uint32_t totalPackages = static_cast<uint32_t>((payload.size() + maxPayloadSize - 1) / maxPayloadSize);

        NetPackages packages;
        packages.reserve(totalPackages);

        for (uint32_t i = 0; i < totalPackages; ++i) {
            size_t offset = i * maxPayloadSize;
            size_t remaining = payload.size() - offset;
            size_t currentPayloadSize = (remaining > maxPayloadSize) ? maxPayloadSize : remaining;

            NetPackageHeader header;
            header.totalPayloadSize   = to_network64(payload.size());
            header.totalPackageNum    = to_network32(totalPackages);
            header.currentPackageNum  = to_network32(i);
            header.currentPayloadSize = to_network32(currentPayloadSize);

            ByteVector packet(headerSize + currentPayloadSize);
            serialize(header, packet.data());
            std::memcpy(packet.data() + headerSize, payload.data() + offset, currentPayloadSize);

            packages.push_back(std::move(packet));
        }

        return packages;
    }

private:
    static uint32_t to_network32(uint32_t v) { return htonl(v); }
    static uint64_t to_network64(uint64_t v) {
    #if defined(_WIN32)
        return htonll(v);
    #elif defined(__APPLE__)
        return OSSwapHostToBigInt64(v);
    #else
        return htobe64(v);
    #endif
    }
    static void serialize(const NetPackageHeader& s, uint8_t* buffer) {
        std::memcpy(buffer, &s, sizeof(NetPackageHeader));
    }
};

// ========== 网络数据解包器 ==========
class NetUnpacker {
public:
    using ByteVector = std::vector<uint8_t>;

    NetUnpacker() : totalSize(0), totalPackages(0), receivedCount(0) {}

    /**
     * 添加一个分片包（支持乱序）
     */
    void addPacket(const ByteVector& packet) {
        if (packet.size() < headerSize()) {
            throw std::runtime_error("invalid packet size");
        }

        NetPacker::NetPackageHeader header;
        deserialize(packet.data(), header);

        if (header.sof != 0xCE9A) {
            throw std::runtime_error("invalid packet header");
        }

        if (totalSize == 0) {
            totalSize = from_network64(header.totalPayloadSize);
            totalPackages = from_network32(header.totalPackageNum);
            buffer.resize(totalSize);
            receivedIndices.clear();
            receivedCount = 0;
        }

        uint32_t currentIndex = from_network32(header.currentPackageNum);
        size_t currentSize = from_network32(header.currentPayloadSize);
        size_t offset = currentIndex * maxPayloadSize();

        if (offset + currentSize > totalSize) {
            throw std::runtime_error("payload out of range");
        }

        // 检查是否重复包
        if (receivedIndices.insert(currentIndex).second) {
            std::memcpy(buffer.data() + offset,
                        packet.data() + headerSize(),
                        currentSize);
            ++receivedCount;
        }
    }

    /**
     * 检查是否已接收完所有分片
     */
    bool isComplete() const {
        return receivedCount == totalPackages;
    }

    /**
     * 获取完整数据
     */
    ByteVector getPayload() const {
        if (!isComplete()) {
            throw std::runtime_error("payload is not complete");
        }
        return buffer;
    }

    /**
     * 获取当前已接收分片索引
     */
    std::unordered_set<uint32_t> getReceivedIndices() const {
        return receivedIndices;
    }

    /**
     * 获取缺失的分片索引
     */
    std::vector<uint32_t> getMissingIndices() const {
        std::vector<uint32_t> missing;
        for (uint32_t i = 0; i < totalPackages; ++i) {
            if (receivedIndices.count(i) == 0) {
                missing.push_back(i);
            }
        }
        return missing;
    }

private:
    size_t totalSize;
    uint32_t totalPackages;
    uint32_t receivedCount;
    ByteVector buffer;
    std::unordered_set<uint32_t> receivedIndices;

    static size_t headerSize() { return sizeof(NetPacker::NetPackageHeader); }
    static size_t maxPayloadSize() { return NetPacker::MTU - headerSize(); }

    static uint64_t from_network64(uint64_t v) {
    #if defined(_WIN32)
        return ntohll(v);
    #elif defined(__APPLE__)
        return OSSwapBigToHostInt64(v);
    #else
        return be64toh(v);
    #endif
    }
    static uint32_t from_network32(uint32_t v) { return ntohl(v); }

    static void deserialize(const uint8_t* buffer, NetPacker::NetPackageHeader& s) {
        std::memcpy(&s, buffer, sizeof(NetPacker::NetPackageHeader));
    }
};

}  // namespace network
}  // namespace CC_Tools

#endif  // CC_Tools_NETPACKAGE_H
