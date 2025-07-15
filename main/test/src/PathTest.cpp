//
// Created by Thrent on 2025/6/28.
//

#include "gtest/gtest.h"
#include "iostream"
#include "cc_tools/utils/PathUtils.h"
#include "cc_tools/utils/StringUtils.h"

using namespace CC_Tools::utils;

TEST(PathTest, getFileName) {
    std::string filePath = TEST_ZH_FILE;

    std::string fileName = PathUtils::getFileName(filePath.c_str());
    std::string fileName2 = PathUtils::getFileName(filePath.c_str(), false);

    std::cout << "fileName: " << fileName << std::endl;
    std::cout << "fileName2: " << fileName2 << std::endl;
}

TEST(PathTest, getExist) {
    bool exist = PathUtils::exists(StringUtils::u8ToLocal(TEST_ZH_FILE).c_str());

    EXPECT_TRUE(exist);
}

TEST(PathTest, getTempPath) {
    std::string tempPath = PathUtils::getTempPath();

    std::cout << "tempPath: " << tempPath << std::endl;
}
