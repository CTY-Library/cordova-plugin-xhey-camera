#pragma once

#include "common/misc_utils.h"
#include "network/network_defs.h"
#include "network/network_manager.h"
#include <mutex>
#include <vector>

namespace xhey {
namespace camera {

class ICryptoProvider;

class EncryptionManager : public Singleton<EncryptionManager> {
public:
    void SetCryptoProvider(ICryptoProvider *crypto_provider);

    std::vector<uint8_t> GetEncryptionKey();

    EncryptionConfig GetEncryptionConfig();

    std::string Encrypt(const std::string &content, const std::vector<uint8_t> &encryption_key);

    std::string Decrypt(const std::string &content, const std::vector<uint8_t> &encryption_key);

private:
    friend class Singleton<EncryptionManager>;

    EncryptionManager();

    ~EncryptionManager();

private:
    static std::vector<uint8_t> GetDefaultEncryptionKey();

private:
    std::mutex encryption_key_mutex_;
    std::vector<uint8_t> encryption_key_;
    ICryptoProvider *crypto_provider_ = nullptr;
};

} // namespace camera
} // namespace xhey
