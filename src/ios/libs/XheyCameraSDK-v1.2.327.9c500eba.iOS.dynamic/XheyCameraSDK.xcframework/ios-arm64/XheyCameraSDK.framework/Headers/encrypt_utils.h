#pragma once

#include <vector>

namespace xhey::camera {

class EncryptUtils {
public:
    static std::vector<uint8_t> XorEncrypt(const std::vector<uint8_t> &data, const std::vector<uint8_t> &key);

    static std::vector<uint8_t> XorEncrypt(const char *data, size_t dataLen, const char *key, size_t keyLen);

    static std::vector<uint8_t> XorEncrypt(const char *data, const char *key);
};

} // namespace xhey::camera
