#include <core/windows/System.h>
#ifdef _WIN32

namespace silence
{
    namespace impl
    {
        std::string username()
        {
            char usernameBuffer[UNLEN + 1];
            DWORD usernameLen = UNLEN + 1;
            GetUserName(usernameBuffer, &usernameLen);
            return std::string(usernameBuffer);
        }
    }
}

#endif