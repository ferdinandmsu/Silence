#ifdef _WIN32
#include <core/windows/System.h>

namespace silence::impl
{
    std::string username()
    {
        char usernameBuffer[UNLEN + 1];
        DWORD usernameLen = UNLEN + 1;
        GetUserName(usernameBuffer, &usernameLen);
        return std::string(usernameBuffer);
    }
}

#endif