#pragma once

#include "common/misc_utils.h"
#include "platform/i_hmac_provider.h"
#include <mutex>
#include <string>
#include <vector>

namespace xhey {
namespace camera {

/// HMAC-SHA256 签名管理器
/// 启动时由平台层注册 IHmacProvider 实现：
///   - iOS: IosHmacProvider (CommonCrypto)
///   - Android: AndroidHmacProvider (javax.crypto.Mac via JNI)
class SignatureManager : public Singleton<SignatureManager> {
public:
    /// 注册平台 HMAC 实现（启动时调用一次）
    void SetHmacProvider(IHmacProvider *hmac_provider);

    /// 生成签名：HMAC-SHA256(key, data) → base64
    std::string GenerateSignature(const std::string &data);

    /// 验证签名：重算 HMAC 并与给定签名比较
    bool VerifySignature(const std::string &data, const std::string &signature);

    /// 获取当前密钥（调试用）
    std::vector<uint8_t> GetKey() const;

private:
    friend class Singleton<SignatureManager>;

    SignatureManager();
    ~SignatureManager();

    /// 计算 HMAC
    std::vector<uint8_t> ComputeHmac(const std::vector<uint8_t> &data);

    /// 生成 32 字节随机密钥
    bool GenerateRandomKey();

    /// 常量时间比较，防止 timing attack
    static bool ConstantTimeEqual(const std::vector<uint8_t> &a, const std::vector<uint8_t> &b);

private:
    IHmacProvider *hmac_provider_ = nullptr;
    std::vector<uint8_t> key_;
    mutable std::mutex mutex_;
};

} // namespace camera
} // namespace xhey
