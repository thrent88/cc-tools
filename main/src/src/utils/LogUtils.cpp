#include "cc_tools/utils/LogUtils.h"

#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace CC_Tools {
namespace utils {

std::shared_ptr<spdlog::logger> LogUtils::createLogger(std::string prefix, std::string logDir) {
    std::shared_ptr<spdlog::logger> log;
    auto log_std_output = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    auto log_basic_file_output = std::make_shared<spdlog::sinks::daily_file_sink_mt>(logDir, 0, 0);
    std::string log_pattern = "%Y-%m-%d %H:%M:%S.%e [%L] > %v";

    log_basic_file_output->set_pattern(log_pattern);
    log_std_output->set_pattern("%Y-%m-%d %H:%M:%S.%e [%^%L%$] > %v");

    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(log_std_output);
    sinks.push_back(log_basic_file_output);

    log = std::make_shared<spdlog::logger>(prefix, std::begin(sinks), std::end(sinks));
}

}
}  // namespace CC_Tools
