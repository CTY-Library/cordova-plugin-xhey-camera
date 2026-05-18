#pragma once

#include "platform/i_crypto_provider.h"

namespace xhey::camera {

class IosCryptoProvider : public ICryptoProvider {
public:
    IosCryptoProvider() = default;
    ~IosCryptoProvider() override = default;

    std::vector<uint8_t> AesEncrypt(const std::vector<uint8_t> &data, const std::vector<uint8_t> &key,
                                    const std::vector<uint8_t> &iv) override;

    std::vector<uint8_t> AesDecrypt(const std::vector<uint8_t> &data, const std::vector<uint8_t> &key,
                                    const std::vector<uint8_t> &iv) override;
};

} // namespace xhey::camera
