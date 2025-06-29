//
// Created by Thrent on 2025/6/29.
//

#include "../../include/cc_tools/utils/IdAllocator.h"

namespace CC_Tools {
namespace utils {

IdAllocator::IdAllocator(int startId) : nextId_(startId) {}

int IdAllocator::allocate() {
    std::lock_guard<std::mutex> lock(mutex_);
    int                         id;
    if (!freeIds_.empty()) {
        id = freeIds_.top();
        freeIds_.pop();
    } else {
        id = nextId_++;
    }
    allocated_.insert(id);
    return id;
}

void IdAllocator::release(int id) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (allocated_.erase(id)) {
        freeIds_.push(id);
    }
}
}  // namespace utils
}  // namespace CC_Tools