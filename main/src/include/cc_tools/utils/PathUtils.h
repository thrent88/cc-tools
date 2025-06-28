//
// Created by Thrent on 2025/6/28.
//

#ifndef CC_Tools_PATHUTILS_H
#define CC_Tools_PATHUTILS_H

#include <string>

namespace CC_Tools {
namespace utils {

class PathUtils {
public:

    /**
     * 获取文件名
     * @param path
     * @param withExtension 是否携带后缀
     * @return
     */
    static std::string getFileName(const char* path, bool withExtension = true);

    /**
     * 获取文件绝对路径
     * @param path
     * @return
     */
    static std::string getAbsolutePath(const char* path);

    /**
     * 获取程序运行时路径
     * @param argv0 需要输入argv[0]
     * @return
     */
    static std::string getRuntimePath(const char* argv0);

    /**
     * 判断文件是否存在
     * @param path
     * @return
     */
    static bool exists(const char* path);
};

} // utils
} // CC_Tools

#endif //CC_Tools_PATHUTILS_H
