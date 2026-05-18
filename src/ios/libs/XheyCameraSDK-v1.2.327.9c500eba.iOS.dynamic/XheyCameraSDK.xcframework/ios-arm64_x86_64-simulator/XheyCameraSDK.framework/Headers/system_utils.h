#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

namespace xhey::camera {

class SystemUtils {
public:
    enum BatteryState { BatteryStateUnknown = 0, BatteryStateUnplugged, BatteryStateCharging, BatteryStateFull };

    static uint64_t GetCPUTime();

    static uint64_t GetEpochTime();

    static uint64_t GetCPUTimeMicros();

    static std::unordered_map<std::string, std::string> GetLocalIPs();

    static std::string GetDocumentPath();

    static std::string GetExecFilePath();

    static std::string GetTimeString(uint64_t ms_epoch, int timezone, bool with_delimiter = true);

    static std::string GetDayString(uint64_t ms_epoch, int timezone);

    static std::string GetHourString(uint64_t ms_epoch, int timezone);

    static std::string GetSystemOSVersion();

    static std::string GetDeviceModel();

    static float GetBatteryLevel();

    static BatteryState GetBatteryState();

    static void PrintDebugLog(const char *log);
    // static bool EnableCrashDump(void(*log_func)(const char*));
};

} // namespace xhey::camera
