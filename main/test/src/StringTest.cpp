//
// Created by Thrent on 2025/6/28.
//

#include "gtest/gtest.h"
#include "iostream"
#include "cc_tools/utils/StringUtils.h"

using namespace CC_Tools::utils;

TEST(StringUtilsTest, u8StringToLocal) {
    std::string str = "UTF8字符串";

    std::cout << str << std::endl;
    std::cout << StringUtils::u8ToLocal(str.c_str()) << std::endl;
}
