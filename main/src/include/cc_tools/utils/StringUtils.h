//
// Created by Thrent on 2025/6/28.
//

#ifndef CC_Tools_STRINGUTILS_H
#define CC_Tools_STRINGUTILS_H

#include <string>

namespace CC_Tools {
namespace utils {

class StringUtils {
public:
    /**
     * UTF8转Local
     * @param utf8str utf8字符串
     * @return
     */
    static std::string u8ToLocal(const char* utf8str);
};

} // utils
} // CC_Tools

#endif //CC_Tools_STRINGUTILS_H
