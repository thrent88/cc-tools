//
// Created by Thrent on 2025/6/28.
// 路径工具
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
     * 判断文件是否存在
     * @param path
     * @return
     */
    static bool exists(const char* path);

    /**
     * 获取临时目录
     * @return
     */
    static std::string getTempPath();
};

}  // namespace utils
}  // namespace CC_Tools

#endif  // CC_Tools_PATHUTILS_H
