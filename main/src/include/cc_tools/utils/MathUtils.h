#ifndef CC_TOOLS_MATHUTILS_H
#define CC_TOOLS_MATHUTILS_H

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace CC_Tools {
namespace utils {

class MathUtils {
public:
    /**
     * @brief 去除数组中的百分之highPercentile的最大值
     *
     * @tparam T
     * @tparam typename
     * @param data
     * @param length
     * @param highPercentile [0-1]百分比
     */
    template <typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
    static void reduceMaxValues(T* data, size_t length, float highPercentile) {
        // 参数校验
        if (length == 0) {
            throw std::invalid_argument("reduceMaxValues: length must be > 0");
        }
        if (highPercentile <= 0.0f || highPercentile > 1.0f) {
            throw std::invalid_argument("reduceMaxValues: highPercentile must be in (0,1]");
        }

        // 1. 复制数据到 vector，以便排序
        std::vector<float> buf(data, data + length);

        // 2. 对所有像素值进行升序排序
        std::sort(buf.begin(), buf.end());

        // 3. 计算“前 highPercentile”的最大值下标
        //    e.g. highPercentile=0.99, idx = floor((length-1)*0.99)
        std::size_t idx = static_cast<std::size_t>(std::floor((length - 1) * highPercentile));

        // 4. 确定截断阈值 cap
        float cap = buf[idx];

        // 5. 对原始数据进行 in-place 截断：超过 cap 的值都设为 cap
        for (std::size_t i = 0; i < length; ++i) {
            if (data[i] > cap) {
                data[i] = cap;
            }
        }
    }

    /**
     * @brief 对比度拉神
     *
     * @param data
     * @param length
     * @param tol_low [0-1]，百分比
     * @param tol_high [0-1]，百分比
     * @return std::array<float, 2> 返回[最小值, 最大值]
     */
    template <typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
    static std::array<T, 2> strechlim(T* data, std::size_t length, float tol_low, float tol_high) {
        // 返回结果初始化为 0
        std::array<float, 2> result = { 0.0f, 0.0f };

        // 参数合法性检查
        if (length == 0 || tol_low < 0.0f || tol_low > 1.0f || tol_high < 0.0f || tol_high > 1.0f ||
            tol_low >= tol_high) {
            throw std::runtime_error("length == 0 || tol_low < 0.0f || tol_low > 1.0f || tol_high < 0.0f || tol_high > 1.0f || tol_low >= tol_high");
        }

        // 0. 先遍历一遍，找出 data 数组中的最小值和最大值
        float dataMin = data[0], dataMax = data[0];
        for (size_t i = 1; i < length; ++i) {
            dataMin = std::min(dataMin, data[i]);
            dataMax = std::max(dataMax, data[i]);
        }
        // 防止除 0
        if (dataMax <= dataMin) {
            dataMax = dataMin + 1e-6f;
        }

        // 1. 构建直方图
        const int                nbins = 256;      // MATLAB 默认 256 桶
        std::vector<std::size_t> hist(nbins, 0u);  // 计数直方图

        for (std::size_t i = 0; i < length; ++i) {
            float v = (data[i] - dataMin) / (dataMax - dataMin);
            // 此时 v 本就落在 [0,1] 之间（除了极端数值误差），可视情况再 clamp
            v = std::min(std::max(v, 0.0f), 1.0f - std::numeric_limits<float>::epsilon());

            // 接下来再把 v 映射到直方图桶
            int bin = static_cast<int>(std::floor(v * (nbins - 1) + 0.5f));

            hist[bin]++;
        }

        // 2. 计算累积分布（归一化到 [0,1]）
        std::vector<float> cdf(nbins, 0.0f);
        cdf[0] = static_cast<float>(hist[0]) / length;
        for (int i = 1; i < nbins; ++i) {
            cdf[i] = cdf[i - 1] + static_cast<float>(hist[i]) / length;
        }

        // 3. 找到第一个 >= tol_low 的桶索引 lowBin
        // int lowBin = 0;
        // while (lowBin < nbins && cdf[lowBin] < tol_low)
        //	++lowBin;

        int lowBin = std::lower_bound(cdf.begin(), cdf.end(), tol_low) - cdf.begin();
        int highBin = std::lower_bound(cdf.begin(), cdf.end(), tol_high) - cdf.begin();

        // 4. 找到第一个 >= tol_high 的桶索引 highBin
        // int highBin = 0;
        // while (highBin < nbins && cdf[highBin] < tol_high)
        //	++highBin;

        // 5. 将桶索引转换回像素值
        // result[0] = static_cast<float>(lowBin) / (nbins - 1);
        // result[1] = static_cast<float>(highBin) / (nbins - 1);
        // 修改后的结果映射部分
        result[0] = dataMin + (static_cast<float>(lowBin) / (nbins - 1)) * (dataMax - dataMin);
        result[1] = dataMin + (static_cast<float>(highBin) / (nbins - 1)) * (dataMax - dataMin);
        return result;
    }
};

}  // namespace utils
}  // namespace CC_Tools

#endif  // CC_TOOLS_MATHUTILS_H
