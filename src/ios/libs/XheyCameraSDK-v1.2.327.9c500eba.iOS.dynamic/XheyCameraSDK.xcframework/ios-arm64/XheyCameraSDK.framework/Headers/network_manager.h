#pragma once

#include "common/misc_utils.h"
#include "common/optional.h"
#include "encrypt/aes_util.h"
#include "network_defs.h"
#include "platform/i_crypto_provider.h"
#include "platform/i_network_provider.h"
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace xhey {
namespace camera {

// 加密配置结构体
struct EncryptionConfig {
    using EncryptFunction = std::function<std::string(const std::string &)>;
    using DecryptFunction = std::function<std::string(const std::string &)>;

    EncryptFunction encrypt_func;
    DecryptFunction decrypt_func;
    bool enabled;

    EncryptionConfig(bool e = true) : enabled(e) {}

    EncryptionConfig(EncryptFunction encrypt, DecryptFunction decrypt, bool e = true)
        : encrypt_func(encrypt), decrypt_func(decrypt), enabled(e) {}
};

class NetworkManager : public Singleton<NetworkManager> {
public:
    friend class Singleton<NetworkManager>;

    bool Initialize(std::unique_ptr<INetworkProvider> network_provider, std::unique_ptr<ICryptoProvider> crypto_provider);

    void Cleanup();

    template <typename T>
    void GetRequestByPath(const std::string &path, NetworkCallback<T> callback) {
        GetRequestByPath(path, {}, callback, Optional<EncryptionConfig>::Null());
    }

    template <typename T>
    void GetRequestByPath(const std::string &path, const std::map<std::string, std::string> &headers, NetworkCallback<T> callback,
                          const Optional<EncryptionConfig> &encryption_config = Optional<EncryptionConfig>::Null());

    template <typename T>
    void PostRequestByPath(const std::string &path, const json &post_data, NetworkCallback<T> callback) {
        PostRequestByPath(path, post_data, {}, callback, Optional<EncryptionConfig>::Null());
    }

    template <typename T>
    void PostRequestByPath(const std::string &path, const std::string &post_data, NetworkCallback<T> callback) {
        PostRequestByPath(path, post_data, {}, callback, Optional<EncryptionConfig>::Null());
    }

    template <typename T>
    void PostRequestByPath(const std::string &path, const json &post_data, const std::map<std::string, std::string> &headers,
                           NetworkCallback<T> callback,
                           const Optional<EncryptionConfig> &encryption_config = Optional<EncryptionConfig>::Null());

    template <typename T>
    void PostRequestByPath(const std::string &path, const std::string &post_data,
                           const std::map<std::string, std::string> &headers, NetworkCallback<T> callback,
                           const Optional<EncryptionConfig> &encryption_config = Optional<EncryptionConfig>::Null());

    void GetRequestRaw(const std::string &url, const std::map<std::string, std::string> &headers,
                       std::function<void(const NetworkResponse<std::string> &)> callback,
                       const Optional<EncryptionConfig> &encryption_config = Optional<EncryptionConfig>::Null());

    void PostRequestRaw(const std::string &url, const std::string &post_data, const std::map<std::string, std::string> &headers,
                        std::function<void(const NetworkResponse<std::string> &)> callback,
                        const Optional<EncryptionConfig> &encryption_config = Optional<EncryptionConfig>::Null());

    template <typename T>
    void GetRequest(const std::string &url, const std::map<std::string, std::string> &headers, NetworkCallback<T> callback,
                    const Optional<EncryptionConfig> &encryption_config = Optional<EncryptionConfig>::Null());

    template <typename T>
    void PostRequest(const std::string &url, const json &post_data, const std::map<std::string, std::string> &headers,
                     NetworkCallback<T> callback,
                     const Optional<EncryptionConfig> &encryption_config = Optional<EncryptionConfig>::Null());

    template <typename T>
    void PostRequest(const std::string &url, const std::string &post_data, const std::map<std::string, std::string> &headers,
                     NetworkCallback<T> callback,
                     const Optional<EncryptionConfig> &encryption_config = Optional<EncryptionConfig>::Null());

    void SetTimeout(int timeout_seconds);

    void SetUserAgent(const std::string &user_agent);

    void SetEncryptionEnabled(bool enabled);

    ICryptoProvider *GetCryptoProvider() const { return crypto_provider_.get(); }

private:
    NetworkManager();
    ~NetworkManager();

    void ExecuteRequestRaw(const std::string &url, const std::string &method, const std::string &post_data,
                           const std::map<std::string, std::string> &headers,
                           std::function<void(const std::string &, int32_t code, Error error)> callback,
                           const Optional<EncryptionConfig> &encryption_config);

    template <typename T>
    void ExecuteRequest(const std::string &url, const std::string &method, const std::string &post_data,
                        const std::map<std::string, std::string> &headers, NetworkCallback<T> callback,
                        const Optional<EncryptionConfig> &encryption_config);

    std::string BuildFullUrl(const std::string &path);

    std::string EncryptRequestData(const std::string &data, const EncryptionConfig &config);

    std::string DecryptResponseData(const std::string &encrypted_data, const EncryptionConfig &config);

private:
    bool initialized_;
    int timeout_seconds_;

    std::string base_url_;
    std::string user_agent_;

    EncryptionConfig default_encryption_config_;

    std::unique_ptr<INetworkProvider> network_provider_;
    std::unique_ptr<ICryptoProvider> crypto_provider_;

    std::mutex mutex_;
};

} // namespace camera
} // namespace xhey
