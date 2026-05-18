#pragma once

#include "common/media_types.h"
#include "processor/processor.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace xhey::camera {

class TruePhotoValidateProcessor : public Processor<std::shared_ptr<VideoFrame>> {
public:
    TruePhotoValidateProcessor();
    ~TruePhotoValidateProcessor() override;

    std::shared_ptr<VideoFrame> Process(const std::shared_ptr<VideoFrame> &input) override;

private:
    TruePhotoErrorCode VerifyTimestamp(const TruePhotoInfo &info);
    TruePhotoErrorCode VerifyLocation(const TruePhotoInfo &info);
    TruePhotoErrorCode VerifySecurityCode(const TruePhotoInfo &info);
    TruePhotoErrorCode CrossCheckTimestamps(const std::vector<TruePhotoInfo> &infos);
    int64_t ExtractSecurityCodeTimestampMs(const std::string &security_code);

    /// Maximum allowed difference between any two timestamps: 10 minutes
    static constexpr int64_t kMaxTimestampDiffMs = 10 * 60 * 1000;
};

} // namespace xhey::camera
