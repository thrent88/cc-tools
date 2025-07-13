//
// Created by Thrent on 2025/6/28.
//

#ifndef CC_Tools_STRINGUTILS_H
#define CC_Tools_STRINGUTILS_H

#include <algorithm>
#include <string>
#include <vector>

namespace CC_Tools {
namespace utils {

class StringUtils {
public:
    using StringList = std::vector<std::string>;

    /**
     * 去除行尾含看不见的空格
     * @param str
     * @return
     */
    static std::string trim(const std::string& str);

    /**
     * 字符串分割
     * @param str
     * @param delimiter
     * @return
     */
    static StringList split(const std::string& str, const std::string delimiter);

    /**
     * UTF8转Local
     * @param utf8str utf8字符串
     * @return
     */
    static std::string u8ToLocal(const std::string& utf8str);


    /**
     * 转大写
     * @param s
     * @return
     */
    static std::string upperCase(std::string s);

    /**
     * 判断字符串是否以head开头
     * @param source
     * @param head
     * @return
     */
    static bool startsWith(const std::string& source, std::string head);

    /**
     * 判断字符串是否以tail结尾
     * @param source
     * @param tail
     * @return
     */
    static bool endsWith(const std::string& source, std::string tail);

    /**
     * 检查两个是否等同
     * @param source
     * @param target
     * @return
     */
    static bool equal(const std::string& source, const std::string& target);

    /**
     * @brief 检查字符串是否包含
     * 
     * @param string 
     * @param subString 
     * @return true 
     * @return false 
     */
    static bool contain(const std::string& string, const std::string& subString);

    /**
     * @brief 检查字符串是否为数字
     * 
     * @param str 
     * @return true 
     * @return false 
     */
    static bool isNumber(const std::string& str);
};

}  // namespace utils
}  // namespace CC_Tools

#endif  // CC_Tools_STRINGUTILS_H
