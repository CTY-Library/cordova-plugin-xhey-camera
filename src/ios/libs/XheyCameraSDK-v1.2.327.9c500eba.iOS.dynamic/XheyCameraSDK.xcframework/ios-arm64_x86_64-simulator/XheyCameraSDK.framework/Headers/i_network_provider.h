#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <string>

namespace xhey::camera {

class INetworkProvider {
public:
    using ResponseCallback = std::function<void(const std::string &response_body, int32_t http_status_code,
                                                const std::string &error_message)>;

    virtual ~INetworkProvider() = default;

    virtual void AsyncPost(const std::string &url, const std::map<std::string, std::string> &headers,
                           const std::string &body, ResponseCallback callback) = 0;

    virtual void AsyncGet(const std::string &url, const std::map<std::string, std::string> &headers,
                          ResponseCallback callback) = 0;
};

} // namespace xhey::camera
