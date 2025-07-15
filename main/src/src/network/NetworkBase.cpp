//
// Created by Thrent on 2025/7/15.
//

#include "../../include/cc_tools/network/NetworkBase.h"

namespace CC_Tools {
namespace network {
NetworkBase::NetworkBase(std::string name): name_(name), id_(idAllocator_.allocate()) {

}
} // network
} // CC_Tools