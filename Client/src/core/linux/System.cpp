#ifdef __linux__
#include <core/linux/System.h>

namespace silence
{
    namespace impl
    {
        std::string username()
        {
            return std::string(getlogin());
        }
    }
}

#endif