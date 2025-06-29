//
// Created by Thrent on 2025/6/28.
//

#include "../../include/cc_tools/utils/PathUtils.h"

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
std::string PathUtils::getFileName(const char* path, bool withExtension) {
    if (!fs::exists(path)) {
        throw std::runtime_error("FileUtils::getFileName: File does not exist: " + std::string(path));
    }

    if (withExtension) {
        return fs::path(path).filename().string();
    }
    return fs::path(path).stem().string();
}

std::string PathUtils::getAbsolutePath(const char* path) {
    return fs::absolute(path).string();
}

bool PathUtils::exists(const char* path) {
    return fs::exists(path);
}
}  // namespace utils
}  // namespace CC_Tools
