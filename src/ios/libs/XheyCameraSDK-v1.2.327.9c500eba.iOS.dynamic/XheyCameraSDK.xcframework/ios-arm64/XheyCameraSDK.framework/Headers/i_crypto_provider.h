#pragma once

#include <cstdint>
#include <vector>

namespace xhey::camera {

class ICryptoProvider {
public:
    virtual ~ICryptoProvider() = default;

    /// AES-CBC 加密（PKCS7 padding），操作原始字节，不含 Base64/Gzip
    virtual std::vector<uint8_t> AesEncrypt(const std::vector<uint8_t> &data, const std::vector<uint8_t> &key,
                                            const std::vector<uint8_t> &iv) = 0;

    /// AES-CBC 解密（PKCS7 padding）
    virtual std::vector<uint8_t> AesDecrypt(const std::vector<uint8_t> &data, const std::vector<uint8_t> &key,
                                            const std::vector<uint8_t> &iv) = 0;
};

} // namespace xhey::camera
