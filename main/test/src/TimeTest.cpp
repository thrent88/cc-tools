#include <gtest/gtest.h>
#include "cc_tools/utils/TimeUtils.h"

TEST(TimeTest, TimeTest) {

    std::string datetime = CC_Tools::utils::TimeUtils::currentDatetimeStr();
    std::cout << "currentDatetimeStr: " << datetime << std::endl;

}
