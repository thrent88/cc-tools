#ifndef CC_TOOLS_TIMEUTILS_H
#define CC_TOOLS_TIMEUTILS_H

#include <chrono>


namespace CC_Tools {
namespace utils {

class TimeUtils {
public:
    using TimePoint = std::chrono::steady_clock::time_point;

    /**
     * @brief 获取当前时间点
     * 
     * @return TimePoint 
     */
    static TimePoint currentTimePoint();

    /**
     * @brief 获取当前时间的字符串
     * 
     * @param format 
     * @return std::string 
     */
    static std::string currentDatetimeStr(const char* format = "%Y%m%d %H%M%S");

    /**
     * @brief 检查这个thiz是否在isBeforeThis之前
     * 
     * @param thiz 
     * @param isBeforeThis 
     * @return true 
     * @return false 
     */
    static bool isBefore(const TimePoint& thiz, const TimePoint& isBeforeThis);

};

}}

#endif // CC_TOOLS_TIMEUTILS_H
