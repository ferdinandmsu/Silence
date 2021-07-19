#include <core/linux/System.h>
#ifdef __linux__

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