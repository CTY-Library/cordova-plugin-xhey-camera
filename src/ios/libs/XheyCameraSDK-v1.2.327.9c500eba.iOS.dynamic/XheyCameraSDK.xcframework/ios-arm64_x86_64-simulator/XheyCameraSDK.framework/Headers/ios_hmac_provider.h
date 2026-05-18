#pragma once

#include "platform/i_hmac_provider.h"

namespace xhey::camera {

/// iOS HMAC 实现，基于 CommonCrypto CCHmac
class IosHmacProvider : public IHmacProvider {
public:
    IosHmacProvider() = default;
    ~IosHmacProvider() override = default;

    std::vector<uint8_t> HmacSha256(const std::vector<uint8_t> &key, const std::vector<uint8_t> &data) override;
};

} // namespace xhey::camera
