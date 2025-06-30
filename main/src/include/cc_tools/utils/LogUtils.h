#ifndef CC_TOOLS_LOGUTILS_H
#define CC_TOOLS_LOGUTILS_H

#include "spdlog/spdlog.h"

namespace CC_Tools {
namespace utils {

class LogUtils {
public:

    /**
     * @brief 创建日志器
     * 
     * @param prefix 文件前缀
     * @param logDir 日志存储目录
     * @return std::shared_ptr<spdlog::logger> 
     */
    static std::shared_ptr<spdlog::logger> createLogger(std::string prefix, std::string logDir);
};

}
}

#endif
