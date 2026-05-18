#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace xhey {
namespace camera {

class ICryptoProvider;

class AesUtil {
public:
    AesUtil(ICryptoProvider *crypto_provider, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);

    AesUtil(ICryptoProvider *crypto_provider);

    void SetKey(const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);

    std::vector<uint8_t> GetCurrentKey() const;

    std::vector<uint8_t> GetCurrentIv() const;

    // AES加密 (string -> Base64 encoded string)
    std::string Encrypt(const std::string &message);

    // AES解密 (Base64 encoded string -> string)
    std::string Decrypt(const std::string &encrypted_message);

    // AES加密 (raw bytes)
    std::vector<uint8_t> EncryptRaw(const std::vector<uint8_t> &data);

    // AES解密 (raw bytes)
    std::vector<uint8_t> DecryptRaw(const std::vector<uint8_t> &data);

private:
    ICryptoProvider *crypto_provider_;
    std::vector<uint8_t> key_;
    std::vector<uint8_t> iv_;

    static const std::vector<uint8_t> kDefaultKey;
    static const std::vector<uint8_t> kDefaultIv;
};

} // namespace camera
} // namespace xhey
