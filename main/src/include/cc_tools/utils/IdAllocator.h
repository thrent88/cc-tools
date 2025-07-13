//
// Created by Thrent on 2025/6/29.
// ID 分配器
//

#ifndef CC_Tools_IDALLOCATOR_H
#define CC_Tools_IDALLOCATOR_H

#include <queue>
#include <mutex>
#include <unordered_set>

namespace CC_Tools {
namespace utils {

/**
 * @brief ID 分配器
 * 
 */
class IdAllocator {
public:
    explicit IdAllocator(int startId = 0);

    // 分配一个唯一 ID
    int allocate();

    // 回收一个 ID
    void release(int id);

private:
    int nextId_;
    std::priority_queue<int, std::vector<int>, std::greater<>> freeIds_;
    std::unordered_set<int> allocated_;
    std::mutex mutex_;
};

} // utils
} // CC_Tools

#endif //CC_Tools_IDALLOCATOR_H
