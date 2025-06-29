//
// Created by Thrent on 2025/6/28.
//

#include "../../include/cc_tools/utils/StringUtils.h"

#if _MSVC_LANG >= 201703L || __cplusplus >= 201703L && defined(__has_include)
// ^ Supports MSVC prior to 15.7 without setting /Zc:__cplusplus to fix __cplusplus
// _MSVC_LANG works regardless. But without the switch, the compiler always reported 199711L:
// https://blogs.msdn.microsoft.com/vcblog/2018/04/09/msvc-now-correctly-reports-__cplusplus/
#if __has_include( \
    <filesystem>)  // Two stage __has_include needed for MSVC 2015 and per https://gcc.gnu.org/onlinedocs/cpp/_005f_005fhas_005finclude.html
#define GHC_USE_STD_FS

// Old Apple OSs don't support std::filesystem, though the header is available at compile
// time. In particular, std::filesystem is unavailable before macOS 10.15, iOS/tvOS 13.0,
// and watchOS 6.0.
#ifdef __APPLE__
#include <Availability.h>
// Note: This intentionally uses std::filesystem on any new Apple OS, like visionOS
// released after std::filesystem, where std::filesystem is always available.
// (All other __<platform>_VERSION_MIN_REQUIREDs will be undefined and thus 0.)
#if __MAC_OS_X_VERSION_MIN_REQUIRED && __MAC_OS_X_VERSION_MIN_REQUIRED < 101500 ||   \
    __IPHONE_OS_VERSION_MIN_REQUIRED && __IPHONE_OS_VERSION_MIN_REQUIRED < 130000 || \
    __TV_OS_VERSION_MIN_REQUIRED && __TV_OS_VERSION_MIN_REQUIRED < 130000 ||         \
    __WATCH_OS_VERSION_MAX_ALLOWED && __WATCH_OS_VERSION_MAX_ALLOWED < 60000
#undef GHC_USE_STD_FS
#endif
#endif
#endif
#endif

#ifdef GHC_USE_STD_FS
#include <filesystem>
namespace fs = std::filesystem;
#else
#include "filesystem.hpp"
namespace fs = ghc::filesystem;
#endif

namespace CC_Tools {
namespace utils {

std::string StringUtils::trim(const std::string& str) {
    std::string blanks("\f\v\r\t\n ");
    std::string input(str);
    input.erase(0, input.find_first_not_of(blanks));
    input.erase(input.find_last_not_of(blanks) + 1);
    return input;
}

StringUtils::StringList StringUtils::split(const std::string& str, const std::string delimiter) {
    std::vector<std::string> result;
    std::string              tmp_str = str;
    size_t                   pos = 0;
    while ((pos = tmp_str.find(delimiter)) != std::string::npos) {
        result.push_back(tmp_str.substr(0, pos));
        tmp_str.erase(0, pos + delimiter.length());
    }
    result.push_back(tmp_str);
    return result;
}

std::string StringUtils::u8ToLocal(const std::string& utf8str) {
    return fs::u8path(utf8str).string();
}

std::string StringUtils::upperCase(const std::string s) {
    std::string result(s);
    std::transform(s.begin(), s.end(), result.begin(),
                   [](unsigned char c) -> unsigned char { return std::toupper(c); });
    return result;
}

bool StringUtils::startsWith( const std::string& source, std::string head) {
    return head.size() <= source.size() && source.compare(0, head.size(), head) == 0;
}

bool StringUtils::endsWith(const std::string& source, std::string tail) {
    return tail.size() <= source.size() && source.compare(source.size() - tail.size(), tail.size(), tail) == 0;
}

bool StringUtils::equal(const std::string& source, const std::string& target) {
    return source.compare(target) == 0;
}

}  // namespace utils
}  // namespace CC_Tools