#pragma once

#include <functional>

namespace xhey::camera {

void RunWithPlatformThreadWrapper(std::function<void()> lambda);

#ifdef __APPLE__
void AutoReleasePool(std::function<void()> lambda);
#endif

} // namespace xhey::camera
