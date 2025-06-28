//
// Created by Thrent on 2025/6/28.
//

#include "gtest/gtest.h"
#include "iostream"
#include "cc_tools/utils/PathUtils.h"
#include "cc_tools/utils/StringUtils.h"

using namespace CC_Tools::utils;

TEST(PathTest, getFileName) {
    std::string filePath = TEST_FILE;

    std::string fileName = PathUtils::getFileName(filePath.c_str());
    std::string fileName2 = PathUtils::getFileName(filePath.c_str(), false);

    std::cout << "fileName: " << fileName << std::endl;
    std::cout << "fileName2: " << fileName2 << std::endl;
}

TEST(PathTest, getExist) {
    bool exist = PathUtils::exists(StringUtils::u8ToLocal("E:\\Cpp\\CPP\\cc-tools\\main\\test\\script\\中文径ts").c_str());

    std::cout << "exist: " << exist << std::endl;
}
