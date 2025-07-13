#include "cc_tools/utils/TimeUtils.h"

namespace CC_Tools {
namespace utils {

TimeUtils::TimePoint TimeUtils::currentTimePoint() {
    return TimeUtils::TimePoint(std::chrono::steady_clock::now());
}

std::string TimeUtils::currentDatetimeStr(const char* format) {
    auto tp = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(tp);
    auto tm = *std::localtime(&time);
    char buf[64];
    strftime(buf, sizeof(buf), format, &tm);
    return std::string(buf);
}

bool TimeUtils::isBefore(const TimePoint& thiz, const TimePoint& isBeforeThis) {
    return thiz < isBeforeThis;
}

}}
