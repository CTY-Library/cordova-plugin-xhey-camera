#pragma once

namespace xhey::camera {

class LocationUtils {
public:
    // 计算两点间距离（米），使用 Haversine 公式
    // @param lat1 第一个点的纬度
    // @param lon1 第一个点的经度
    // @param lat2 第二个点的纬度
    // @param lon2 第二个点的经度
    // @return 两点间的距离（米）
    static double CalculateDistance(double lat1, double lon1, double lat2, double lon2);
};

} // namespace xhey::camera
