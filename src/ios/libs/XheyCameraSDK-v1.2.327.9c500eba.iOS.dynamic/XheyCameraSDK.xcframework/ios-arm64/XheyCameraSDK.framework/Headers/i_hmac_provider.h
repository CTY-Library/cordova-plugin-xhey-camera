#pragma once

#include <cstdint>
#include <vector>

namespace xhey::camera {

/// 平台 HMAC 计算接口
/// iOS: CommonCrypto CCHmac
/// Android: javax.crypto.Mac (通过 JNI 回调)
class IHmacProvider {
public:
    virtual ~IHmacProvider() = default;

    /// HMAC-SHA256 计算
    /// @param key  密钥字节
    /// @param data 待签名数据字节
    /// @return HMAC 摘要（32 字节）
    virtual std::vector<uint8_t> HmacSha256(const std::vector<uint8_t> &key, const std::vector<uint8_t> &data) = 0;
};

} // namespace xhey::camera
