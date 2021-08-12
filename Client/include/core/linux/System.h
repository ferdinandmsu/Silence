#pragma once
#ifdef __linux__

#include <iostream>
#include <memory>
#include <array>
#include <vector>
#include <string>
#include <stdexcept>
#include <optional>
#include <fstream>
#include <filesystem>

#include <unistd.h>

namespace silence::impl {
    std::string username();

    void persist();
}

#endif