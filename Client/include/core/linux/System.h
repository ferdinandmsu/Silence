#pragma once
#ifdef __linux__

#include <iostream>
#include <memory>
#include <array>
#include <vector>
#include <string>
#include <stdexcept>
#include <optional>

#include <unistd.h>

namespace silence
{
    namespace impl
    {
        inline std::string username()
        {
            return std::string(getlogin());
        }
    }
}

#endif