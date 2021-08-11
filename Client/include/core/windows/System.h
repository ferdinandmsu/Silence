#pragma once
#ifdef _WIN32

#include <iostream>
#include <memory>
#include <array>
#include <vector>
#include <string>
#include <stdexcept>
#include <optional>

#include <Windows.h>
#include <Lmcons.h>
#include <winsock.h>

#pragma comment(lib, "ws2_32.lib")

namespace silence::impl
{
    std::string username();
}

#endif