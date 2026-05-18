#pragma once

#include "misc_utils.h"
#include <mutex>
#include <string>

namespace xhey::camera {

class UserInfo : public Singleton<UserInfo> {
public:
    std::string GetDeviceId() {
        std::lock_guard<std::mutex> lock(device_id_mutex_);
        return device_id_;
    }

    void SetDeviceId(const std::string &deviceId) {
        std::lock_guard<std::mutex> lock(device_id_mutex_);
        device_id_ = deviceId;
    }

private:
    friend class Singleton<UserInfo>;

    UserInfo() = default;
    ~UserInfo() = default;

private:
    std::mutex device_id_mutex_;
    std::string device_id_;
};

} // namespace xhey::camera
