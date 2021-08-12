#ifdef __linux__

#include <core/linux/System.h>

namespace silence::impl {
    std::string username() {
        return getlogin();
    }
}

#endif