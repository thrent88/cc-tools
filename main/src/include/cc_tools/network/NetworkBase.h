//
// Created by Thrent on 2025/7/15.
//

#ifndef CC_Tools_NETWORKBASE_H
#define CC_Tools_NETWORKBASE_H

#include <asio.hpp>
#include <string>
#include <functional>

#include "cc_tools/utils/IdAllocator.h"

namespace CC_Tools {
namespace network {

class NetworkBase {
public:
    using OnError = std::function<void(int ec, std::string why)>;
    using OnRecvMessage = std::function<void()>;

    NetworkBase(std::string name);

    virtual bool launch();

    virtual void shutdown();

protected:
    std::string        name_;  // 基础名称
    int                id_;    // 基础ID
    utils::IdAllocator idAllocator_;    // ID分配器
};

}  // namespace network
}  // namespace CC_Tools

#endif  // CC_Tools_NETWORKBASE_H
