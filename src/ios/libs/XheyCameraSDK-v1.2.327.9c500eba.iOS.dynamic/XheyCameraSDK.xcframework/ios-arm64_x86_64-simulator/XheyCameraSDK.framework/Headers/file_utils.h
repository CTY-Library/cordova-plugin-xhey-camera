#pragma once

#include <string>

namespace xhey::camera {

class FileUtils {
public:
    static bool FileExists(const std::string &path);

    static size_t GetFileSize(const std::string &path);

    static std::string ReadFileContent(const std::string &path);
};

} // namespace xhey::camera
