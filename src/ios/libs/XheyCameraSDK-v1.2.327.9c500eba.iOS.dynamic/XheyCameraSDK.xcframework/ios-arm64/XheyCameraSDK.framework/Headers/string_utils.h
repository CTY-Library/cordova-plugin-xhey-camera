#pragma once

#include <string>
#include <vector>

namespace xhey::camera {

namespace detail {
// double 一般不需要 base，保持原样
inline double safe_double_c(const char *str, bool *valid) {
    if (!str || *str == '\0') {
        if (valid)
            *valid = false;
        return 0.0;
    }
    char *end = nullptr;
    errno = 0;
    double val = std::strtod(str, &end);
    bool ok = (end != str && *end == '\0' && errno == 0);
    if (valid)
        *valid = ok;
    return ok ? val : 0.0;
}

inline int64_t safe_int64_c(const char *str, bool *valid, int base = 10) {
    if (!str || *str == '\0') {
        if (valid)
            *valid = false;
        return 0;
    }
    char *end = nullptr;
    errno = 0;
    long long val = std::strtoll(str, &end, base);
    bool ok = (end != str && *end == '\0' && errno == 0);
    if (valid)
        *valid = ok;
    return ok ? static_cast<int64_t>(val) : 0;
}

inline uint64_t safe_uint64_c(const char *str, bool *valid, int base = 10) {
    if (!str || *str == '\0') {
        if (valid)
            *valid = false;
        return 0;
    }

    char *end = nullptr;
    errno = 0;
    unsigned long long val = std::strtoull(str, &end, base);
    bool ok = (end != str && *end == '\0' && errno == 0);
    if (valid)
        *valid = ok;
    return ok ? static_cast<uint64_t>(val) : 0;
}

inline int32_t safe_int32_c(const char *str, bool *valid, int base = 10) {
    if (!str || *str == '\0') {
        if (valid)
            *valid = false;
        return 0;
    }
    char *end = nullptr;
    errno = 0;
    long val = std::strtol(str, &end, base);
    bool ok = (end != str && *end == '\0' && errno == 0);
    if (valid)
        *valid = ok;
    return ok ? static_cast<int32_t>(val) : 0;
}

inline uint32_t safe_uint32_c(const char *str, bool *valid, int base = 10) {
    if (!str || *str == '\0') {
        if (valid)
            *valid = false;
        return 0;
    }

    char *end = nullptr;
    errno = 0;
    unsigned long val = std::strtoul(str, &end, base);
    bool ok = (end != str && *end == '\0' && errno == 0);
    if (valid)
        *valid = ok;
    return ok ? static_cast<uint32_t>(val) : 0;
}

} // namespace detail

class StringUtils {
public:
    // 去除字符串开头和结尾的空白字符
    static std::string Trim(const std::string &str);

    // 只去除字符串开头的空白字符
    static std::string TrimStart(const std::string &str);

    // 只去除字符串结尾的空白字符
    static std::string TrimEnd(const std::string &str);

    // 去除字符串结尾的指定字符
    static std::string TrimEnd(const std::string &str, char ch);

    // 去除字符串开头的指定字符
    static std::string TrimStart(const std::string &str, char ch);

    static void Trim(std::string &str);

    static void TrimStart(std::string &str);

    static void TrimEnd(std::string &str);

    static void Trim(std::string &str, char ch);

    static void TrimStart(std::string &str, char ch);

    static std::string ToLower(const std::string &str);

    static std::string ToUpper(const std::string &str);

    static std::vector<uint8_t> HexStringToBytes(const std::string &hex);

    static std::vector<std::string> Split(const std::string &str, char delimiter);

    //    template <typename T>
    //    static T SafeNumber(const std::string &str, bool *valid = nullptr, int base = 10) {
    //        if constexpr (std::is_same_v<T, double>) {
    //            return detail::safe_double_c(str.c_str(), valid);
    //        } else if constexpr (std::is_same_v<T, int64_t>) {
    //            return detail::safe_int64_c(str.c_str(), valid, base);
    //        } else if constexpr (std::is_same_v<T, int32_t>) {
    //            return detail::safe_int32_c(str.c_str(), valid, base);
    //        } else if constexpr (std::is_same_v<T, uint32_t>) {
    //            return detail::safe_uint32_c(str.c_str(), valid, base);
    //        } else if constexpr (std::is_same_v<T, uint64_t>) {
    //            return detail::safe_uint64_c(str.c_str(), valid, base);
    //        } else {
    //            static_assert(sizeof(T) == 0, "SafeNumber: unsupported type");
    //        }
    //    }

    template <typename T>
    struct SafeNumberImpl;

    // double
    template <>
    struct SafeNumberImpl<double> {
        static double run(const std::string &str, bool *valid, int) { return detail::safe_double_c(str.c_str(), valid); }
    };

    // int64_t
    template <>
    struct SafeNumberImpl<int64_t> {
        static int64_t run(const std::string &str, bool *valid, int base) {
            return detail::safe_int64_c(str.c_str(), valid, base);
        }
    };

    // int32_t
    template <>
    struct SafeNumberImpl<int32_t> {
        static int32_t run(const std::string &str, bool *valid, int base) {
            return detail::safe_int32_c(str.c_str(), valid, base);
        }
    };

    // uint32_t
    template <>
    struct SafeNumberImpl<uint32_t> {
        static uint32_t run(const std::string &str, bool *valid, int base) {
            return detail::safe_uint32_c(str.c_str(), valid, base);
        }
    };

    // uint64_t
    template <>
    struct SafeNumberImpl<uint64_t> {
        static uint64_t run(const std::string &str, bool *valid, int base) {
            return detail::safe_uint64_c(str.c_str(), valid, base);
        }
    };

    // 统一入口
    template <typename T>
    static T SafeNumber(const std::string &str, bool *valid = nullptr, int base = 10) {
        return SafeNumberImpl<T>::run(str, valid, base);
    }

    static bool Contains(const std::string &str, const std::string &sub_str) { return str.find(sub_str) != std::string::npos; }

    static bool StartsWith(const std::string &str, const std::string &prefix) { return str.find(prefix) == 0; }

    static bool EndsWith(const std::string &str, const std::string &suffix) {
        return str.find(suffix) == str.size() - suffix.size();
    }

    // Base64编码
    static std::string Base64Encode(const std::vector<unsigned char> &data);

    // Base64解码
    static std::vector<unsigned char> Base64Decode(const std::string &encoded_string);
};


} // namespace xhey::camera
