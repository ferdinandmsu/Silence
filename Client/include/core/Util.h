#pragma once

#include <chrono>
#include <thread>
#include <future>
#include <filesystem>

#ifdef _WIN32
#include <core/windows/System.h>
#include <core/windows/Screenshot.h>
#else
#include <core/linux/System.h>
#include <core/linux/Screenshot.h>
#endif

namespace silence
{
    namespace fs = std::filesystem;

    namespace impl
    {
        std::optional<std::string> exec(const char *command);

        std::vector<fs::path> listdir(const fs::path &path);

        std::string hostname();

        std::shared_ptr<std::string> toBinaryString(const cv::Mat &img);
    }
}