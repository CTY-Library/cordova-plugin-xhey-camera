#pragma once

#include "common/error.h"
#include "common/json.hpp"
#include "common/misc_utils.h"
#include "common/optional.h"
#include "log/log.h"
#include "utils/time_utils.h"
#include <functional>
#include <string>
#include <vector>

namespace xhey::camera {
using json = nlohmann::json;
}

namespace xhey::camera {

// ServerTimeModel 结构体
struct ServerTimeModel {
    // 时间单位毫秒
    int64_t timestamp;
    bool in_china;
    bool stop_device;
    int error_code;

    ServerTimeModel() : timestamp(0), in_china(true), stop_device(false), error_code(0) {}

    // JSON序列化
    void FromJson(const json &j) {
        try {
            // 使用get方法，提供默认值，避免out_of_range异常
            timestamp = j.value("timestamp", 0);
            in_china = j.value("in_china", false);
            stop_device = j.value("stopDevice", false);
            error_code = j.value("errorCode", 0);
        } catch (const json::parse_error &e) {
            timestamp = 0;
            in_china = false;
            stop_device = false;
            error_code = 0;
            XLOG_ERROR("ServerTimeModel FromJson failed: %s", e.what());
        } catch (const json::exception &e) {
            timestamp = 0;
            in_china = false;
            stop_device = false;
            error_code = 0;
            XLOG_ERROR("ServerTimeModel FromJson failed: %s", e.what());
        } catch (const std::exception &e) {
            // 如果解析失败，使用默认值
            timestamp = 0;
            in_china = false;
            stop_device = false;
            error_code = 0;
            XLOG_ERROR("ServerTimeModel FromJson failed: %s", e.what());
        } catch (...) {
            timestamp = 0;
            in_china = false;
            stop_device = false;
            error_code = 0;
            XLOG_ERROR("ServerTimeModel FromJson failed: unknown exception");
        }
    }

    // JSON反序列化
    json ToJson() const {
        json j;
        j["timestamp"] = timestamp;
        j["in_china"] = in_china;
        j["stopDevice"] = stop_device;
        j["errorCode"] = error_code;
        return j;
    }

    bool Valid() const { return timestamp > 0; }

    std::string GetFormattedTime() const { return TimeUtils::FormatTime(timestamp); }
};

struct EncryptionModel {
    int status;
    std::string msg;
    std::string model_config;

    void FromJson(const json &j) {
        try {
            if (j.contains("status")) {
                status = j["status"].get<int>();
            }
            if (j.contains("msg")) {
                msg = j["msg"].get<std::string>();
            }
            if (j.contains("model_config")) {
                model_config = j["model_config"].get<std::string>();
            }
        } catch (...) {
            status = 0;
            msg = "";
            model_config = "";
            XLOG_ERROR("EncryptionModel FromJson failed");
        }
    }

    json ToJson() const {
        json j;
        j["status"] = status;
        j["msg"] = msg;
        j["model_config"] = model_config;
        return j;
    }

    bool Valid() const { return status == 0; }
};

struct TokenModel {
    int status = 0;
    std::string msg;
    std::string token;
    int64_t expire_at = 0;

    void FromJson(const json &j) {
        try {
            if (j.contains("status")) {
                status = j["status"].get<int>();
            }
            if (j.contains("msg")) {
                msg = j["msg"].get<std::string>();
            }
            if (j.contains("token")) {
                token = j["token"].get<std::string>();
            }
            if (j.contains("expireAt")) {
                expire_at = j["expireAt"].get<int64_t>();
            }
        } catch (...) {
            status = 0;
            msg = "";
            token = "";
            expire_at = 0;
            XLOG_ERROR("TokenModel FromJson failed");
        }
    }

    json ToJson() const {
        json j;
        j["status"] = status;
        j["msg"] = msg;
        j["token"] = token;
        j["expire_at"] = expire_at;
        return j;
    }

    bool Valid() const { return status == 0; }
};

// 地址项结构体
struct AddressItem {
    std::string address;
    std::string distance;
    std::string distanceNumeric;
    std::string from;
    std::string lat;
    std::string lng;
    std::string locationID;
    std::string name;
    std::string originTab;
    std::string originType;
    std::string specialTip;
    std::string typecode;

    AddressItem() = default;

    void FromJson(const json &j) {
        try {
            address = j.value("address", "");
            distance = j.value("distance", "");
            distanceNumeric = j.value("distanceNumeric", "");
            from = j.value("from", "");
            lat = j.value("lat", "");
            lng = j.value("lng", "");
            locationID = j.value("locationID", "");
            name = j.value("name", "");
            originTab = j.value("originTab", "");
            originType = j.value("originType", "");
            specialTip = j.value("specialTip", "");
            typecode = j.value("typecode", "");
        } catch (const json::exception &e) {
            XLOG_ERROR("AddressItem FromJson failed: %s", e.what());
        } catch (...) {
            XLOG_ERROR("AddressItem FromJson failed: unknown exception");
        }
    }

    json ToJson() const {
        json j;
        j["address"] = address;
        j["distance"] = distance;
        j["distanceNumeric"] = distanceNumeric;
        j["from"] = from;
        j["lat"] = lat;
        j["lng"] = lng;
        j["locationID"] = locationID;
        j["name"] = name;
        j["originTab"] = originTab;
        j["originType"] = originType;
        j["specialTip"] = specialTip;
        j["typecode"] = typecode;
        return j;
    }
};

// 大位置项结构体
struct LargePositionItem {
    int id = 0;
    std::string name;

    LargePositionItem() = default;

    void FromJson(const json &j) {
        try {
            id = j.value("id", 0);
            name = j.value("name", "");
        } catch (const json::exception &e) {
            XLOG_ERROR("LargePositionItem FromJson failed: %s", e.what());
        } catch (...) {
            XLOG_ERROR("LargePositionItem FromJson failed: unknown exception");
        }
    }

    json ToJson() const {
        json j;
        j["id"] = id;
        j["name"] = name;
        return j;
    }
};

// 位置详情结构体
struct LocationDetail {
    std::string city;
    std::string country;
    std::string district;
    std::string from;
    std::string poi;
    std::string province;
    std::string township;
    std::string typeCode;

    LocationDetail() = default;

    void FromJson(const json &j) {
        try {
            city = j.value("city", "");
            country = j.value("country", "");
            district = j.value("district", "");
            from = j.value("from", "");
            poi = j.value("poi", "");
            province = j.value("province", "");
            township = j.value("township", "");
            typeCode = j.value("typeCode", "");
        } catch (const json::exception &e) {
            XLOG_ERROR("LocationDetail FromJson failed: %s", e.what());
        } catch (...) {
            XLOG_ERROR("LocationDetail FromJson failed: unknown exception");
        }
    }

    json ToJson() const {
        json j;
        j["city"] = city;
        j["country"] = country;
        j["district"] = district;
        j["from"] = from;
        j["poi"] = poi;
        j["province"] = province;
        j["township"] = township;
        j["typeCode"] = typeCode;
        return j;
    }
};

// 聚合地址结构体
struct PolymerizeAddress {
    std::string address;
    std::string distance;
    std::string distanceNumeric;
    std::string from;
    std::string lat;
    std::string lng;
    std::string locationID;
    std::string name;
    std::string originFrom;
    std::string originType;
    std::string specialTip;
    std::string typecode;

    PolymerizeAddress() = default;

    void FromJson(const json &j) {
        try {
            address = j.value("address", "");
            distance = j.value("distance", "");
            distanceNumeric = j.value("distanceNumeric", "");
            from = j.value("from", "");
            lat = j.value("lat", "");
            lng = j.value("lng", "");
            locationID = j.value("locationID", "");
            name = j.value("name", "");
            originFrom = j.value("originFrom", "");
            originType = j.value("originType", "");
            specialTip = j.value("specialTip", "");
            typecode = j.value("typecode", "");
        } catch (const json::exception &e) {
            XLOG_ERROR("PolymerizeAddress FromJson failed: %s", e.what());
        } catch (...) {
            XLOG_ERROR("PolymerizeAddress FromJson failed: unknown exception");
        }
    }

    json ToJson() const {
        json j;
        j["address"] = address;
        j["distance"] = distance;
        j["distanceNumeric"] = distanceNumeric;
        j["from"] = from;
        j["lat"] = lat;
        j["lng"] = lng;
        j["locationID"] = locationID;
        j["name"] = name;
        j["originFrom"] = originFrom;
        j["originType"] = originType;
        j["specialTip"] = specialTip;
        j["typecode"] = typecode;
        return j;
    }
};

// 建议结构体
struct Suggest {
    double distance = 0.0;
    std::string fingerId;
    int level = 0;
    std::string location;

    Suggest() = default;

    void FromJson(const json &j) {
        try {
            distance = j.value("distance", 0.0);
            fingerId = j.value("fingerId", "");
            level = j.value("level", 0);
            location = j.value("location", "");
        } catch (const json::exception &e) {
            XLOG_ERROR("Suggest FromJson failed: %s", e.what());
        } catch (...) {
            XLOG_ERROR("Suggest FromJson failed: unknown exception");
        }
    }

    json ToJson() const {
        json j;
        j["distance"] = distance;
        j["fingerId"] = fingerId;
        j["level"] = level;
        j["location"] = location;
        return j;
    }
};

// 地址响应结构体
struct AddressResponse {
    std::vector<AddressItem> addressList;
    int errorCode = 0;
    std::string largeposition;
    std::vector<LargePositionItem> largepositionList;
    LocationDetail locationDetail;
    int minChooseDistance = 0;
    PolymerizeAddress polymerizeAdress;
    std::string recommended;
    int recommendedID = 0;
    std::string smallposition;
    Suggest suggest;

    AddressResponse() = default;

    void FromJson(const json &j) {
        try {
            errorCode = j.value("errorCode", 0);
            largeposition = j.value("largeposition", "");
            recommended = j.value("recommended", "");
            recommendedID = j.value("recommendedID", 0);
            smallposition = j.value("smallposition", "");
            minChooseDistance = j.value("minChooseDistance", 0);

            // 解析 addressList
            if (j.contains("addressList") && j["addressList"].is_array()) {
                addressList.clear();
                for (const auto &item : j["addressList"]) {
                    AddressItem addressItem;
                    addressItem.FromJson(item);
                    addressList.push_back(addressItem);
                }
            }

            // 解析 largepositionList
            if (j.contains("largepositionList") && j["largepositionList"].is_array()) {
                largepositionList.clear();
                for (const auto &item : j["largepositionList"]) {
                    LargePositionItem positionItem;
                    positionItem.FromJson(item);
                    largepositionList.push_back(positionItem);
                }
            }

            // 解析 locationDetail
            if (j.contains("locationDetail") && j["locationDetail"].is_object()) {
                locationDetail.FromJson(j["locationDetail"]);
            }

            // 解析 polymerizeAdress
            if (j.contains("polymerizeAdress") && j["polymerizeAdress"].is_object()) {
                polymerizeAdress.FromJson(j["polymerizeAdress"]);
            }

            // 解析 suggest
            if (j.contains("suggest") && j["suggest"].is_object()) {
                suggest.FromJson(j["suggest"]);
            }
        } catch (const json::exception &e) {
            XLOG_ERROR("AddressResponse FromJson failed: %s", e.what());
        } catch (...) {
            XLOG_ERROR("AddressResponse FromJson failed: unknown exception");
        }
    }

    json ToJson() const {
        json j;
        j["errorCode"] = errorCode;
        j["largeposition"] = largeposition;
        j["recommended"] = recommended;
        j["recommendedID"] = recommendedID;
        j["smallposition"] = smallposition;
        j["minChooseDistance"] = minChooseDistance;

        // 序列化 addressList
        json addressListJson = json::array();
        for (const auto &item : addressList) {
            addressListJson.push_back(item.ToJson());
        }
        j["addressList"] = addressListJson;

        // 序列化 largepositionList
        json largepositionListJson = json::array();
        for (const auto &item : largepositionList) {
            largepositionListJson.push_back(item.ToJson());
        }
        j["largepositionList"] = largepositionListJson;

        j["locationDetail"] = locationDetail.ToJson();
        j["polymerizeAdress"] = polymerizeAdress.ToJson();
        j["suggest"] = suggest.ToJson();

        return j;
    }

    bool Valid() const { return errorCode == 0; }
};

// 模板化的响应结构体
template <typename T>
struct NetworkResponse {
    NetworkResponse(int32_t code, const std::string &response_data) : success_(true), code_(code) { FromJson(response_data); }

    NetworkResponse(int32_t code, const Error &error) : error_(error), success_(false), code_(code) {}

    const json ToJson() const {
        json j;
        if (success_) {
            j["data"] = data_.Value().ToJson();
            j["code"] = code_;
            j["msg"] = msg_;
        } else {
            j["code"] = error_.Code();
            j["msg"] = error_.Message();
        }
        j["toast_msg"] = toast_msg_;
        return j;
    }

    T &Data() { return data_.Value(); }

    const T &Data() const { return data_.Value(); }

    bool Success() const { return success_; }

    Error &Error() { return error_; }

    const class Error &Error() const { return error_; }

private:
    // 解析JSON数据到模板类型
    void FromJson(const std::string &response_data) {
        if (response_data.empty()) {
            success_ = false;
            error_ = MAKE_ERROR(kErrorDomainNetwork, kErrorCodeJsonParseFailed, "响应数据为空");
            return;
        }

        // 检查响应数据是否为有效的JSON格式
        if (!IsValidJson(response_data)) {
            success_ = false;
            error_ = MAKE_ERROR(kErrorDomainNetwork, kErrorCodeJsonParseFailed, "响应数据格式无效: " + response_data);
            return;
        }

        // 尝试解析JSON（不抛异常）
        json j = json::parse(response_data, nullptr, /* allow_exceptions = */ false);

        if (j.is_discarded()) {
            success_ = false;
            error_ = MAKE_ERROR(kErrorDomainNetwork, kErrorCodeJsonParseFailed, "JSON解析失败");
            return;
        }

        try {
            // 解析顶层字段
            code_ = j.value("code", 0);
            msg_ = j.value("msg", "");
            toast_msg_ = j.value("toast_msg", "");

            // 解析data字段
            if (j.contains("data") && !j["data"].is_null()) {
                T data;
                data.FromJson(j["data"]);
                data_ = data;
                success_ = true;
            } else {
                success_ = false;
                error_ = MAKE_ERROR(kErrorDomainNetwork, code_, msg_);
            }

        } catch (const json::exception &e) {
            success_ = false;
            error_ = MAKE_ERROR(kErrorDomainNetwork, kErrorCodeJsonParseFailed, "JSON处理异常: " + std::string(e.what()));
        } catch (const std::exception &e) {
            success_ = false;
            error_ = MAKE_ERROR(kErrorDomainNetwork, kErrorCodeJsonParseFailed, "未知异常: " + std::string(e.what()));
        } catch (...) {
            success_ = false;
            error_ = MAKE_ERROR(kErrorDomainNetwork, kErrorCodeJsonParseFailed, "未知异常: " + response_data);
        }
    }

    // 检查字符串是否为有效的JSON格式
    bool IsValidJson(const std::string &str) {
        if (str.empty()) {
            return false;
        }

        // 简单的JSON格式检查
        std::string trimmed = str;
        // 去除前后空白字符
        trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
        trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);

        if (trimmed.empty()) {
            return false;
        }

        // 检查是否以 { 或 [ 开头
        if (trimmed[0] != '{' && trimmed[0] != '[') {
            return false;
        }

        return true;
    }

private:
    class Error error_;

    Optional<T> data_;
    bool success_ = false;
    int code_ = 0;
    std::string msg_ = "";
    std::string toast_msg_ = "";
};

// 为std::string特化，直接返回原始数据
template <>
struct NetworkResponse<std::string> {
    int code;
    std::string msg;
    std::string toast_msg;
    std::string error_message;
    std::string data;
    Error error;

    NetworkResponse(int32_t c, const std::string &response_data) : code(c) { FromJson(response_data); }

    NetworkResponse(int32_t c, const Error &error) : error(error), code(c) {}

    bool Success() const { return error.IsOK(); }

    const std::string &Data() const { return data; }

    void FromJson(const std::string &response_data) {
        if (response_data.empty()) {
            return;
        }
        data = response_data;
    }

    json ToJson() const {
        json j;
        j["code"] = code;
        j["msg"] = msg;
        j["toast_msg"] = toast_msg;
        j["data"] = data;
        return j;
    }
};

// 网络请求回调函数类型
template <typename T>
using NetworkCallback = std::function<void(const NetworkResponse<T> &)>;

} // namespace xhey::camera
