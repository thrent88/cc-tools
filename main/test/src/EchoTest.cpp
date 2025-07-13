#include "cc_tools/utils/MathUtils.h"
#include "gtest/gtest.h"


TEST(EchoTest, test) {
    std::vector<float> vec(150, 1.0f);

    CC_Tools::utils::MathUtils::reduceMaxValues<float>(vec.data(), vec.size(), 0.5f);

    CC_Tools::utils::MathUtils::strechlim<float>(vec.data(), vec.size(), 0.0f, 1.0f);
}
