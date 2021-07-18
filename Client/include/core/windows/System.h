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

namespace silence
{
    namespace impl
    {
        inline std::string username()
        {
            char usernameBuffer[UNLEN + 1];
            DWORD usernameLen = UNLEN + 1;
            GetUserName(usernameBuffer, &usernameLen);
            return std::string(usernameBuffer);
        }
    }
}

#endif