#include <core/Util.h>

namespace silence
{
    namespace impl
    {
        std::optional<std::string> exec(const char *command)
        {
            std::array<char, 128> buffer;
            std::string result;
#ifdef _WIN32
            std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command, "r"), _pclose);
#else
            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command, "r"), pclose);
#endif
            if (!pipe)
                return {};

            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
                result += buffer.data();
            return result;
        }

        std::vector<fs::path> listDir(const fs::path &path)
        {
            std::vector<fs::path> dirList;
            for (auto &entry : fs::directory_iterator(path))
                dirList.push_back(fs::relative(entry.path()));
            return dirList;
        }

        std::string hostname()
        {
            char hostnameBuffer[500];
            gethostname(hostnameBuffer, 500);
            return std::string(hostnameBuffer);
        }
    }
}