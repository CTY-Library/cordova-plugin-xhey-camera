#pragma once

#include "platform/i_network_provider.h"

namespace xhey::camera {

class IosNetworkProvider : public INetworkProvider {
public:
    IosNetworkProvider();
    ~IosNetworkProvider() override;

    void AsyncPost(const std::string &url, const std::map<std::string, std::string> &headers, const std::string &body,
                   ResponseCallback callback) override;

    void AsyncGet(const std::string &url, const std::map<std::string, std::string> &headers,
                  ResponseCallback callback) override;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace xhey::camera
