#pragma once
#include <string>
#include <vector>

namespace xhey::camera {

enum class TruePhotoDataType {
    kTimestamp = 0,
    kLocation = 1,
    kSecurityCode = 2,
};

enum class TruePhotoErrorCode {
    kSuccess = 0,
    kTimestampSignatureMismatch = 1,
    kLocationSignatureMismatch = 2,
    kSecurityCodeSignatureMismatch = 3,
    kTimestampCrossCheckFailed = 4,
};

struct TruePhotoInfo {
    TruePhotoDataType type = TruePhotoDataType::kTimestamp;
    std::string data;
    std::string signature;
};

} // namespace xhey::camera
