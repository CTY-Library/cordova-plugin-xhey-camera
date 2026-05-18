#pragma once

#include "common/misc_utils.h"
#include "common/result.h"
#include "network/network_defs.h"
#include "network/retry_manager.h"
#include "utils/location_utils.h"
#include <functional>

namespace xhey::camera {

class AddressManager final : public Singleton<AddressManager> {
public:
    void RequestAddress(double latitude, double longitude, std::function<void(const Result<std::string> &address)> callback);

    std::string GetAddress();

    AddressResponse GetAddressResponse();

    void Cancel();

private:
    friend class Singleton<AddressManager>;

private:
    AddressManager();
    ~AddressManager();

    void RequestAddressImpl();
    void OnRequestAddress(const NetworkResponse<std::string> &response);

private:
    std::mutex mutex_;
    std::string address_;
    AddressResponse address_response_;
    double latitude_ = 0;
    double longitude_ = 0;
    std::function<void(const Result<std::string> &address)> request_address_callback_;
    std::shared_ptr<RetryManager> retry_manager_;
    bool is_requesting_ = false;
    double last_request_latitude_ = 0;
    double last_request_longitude_ = 0;
};

} // namespace xhey::camera
