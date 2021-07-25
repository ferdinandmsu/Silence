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

        template <typename T>
        T createIO(const fs::path &path, IO option,
                   OP operation)
        {
            if (option == IO::BINARY && (operation == OP::WRITE || operation == OP::READ))
                return T{path, std::ios::binary};
            else if (option == IO::TEXT && (operation == OP::WRITE || operation == OP::READ))
                return T{path};
            else if (option == IO::BINARY && operation == OP::APPEND)
                return T{path, std::ios::ate};
            else
                return T{path, std::ios::app};
        }

        bool write(const fs::path &path,
                   const std::string &content, IO option, OP operation)
        {
            std::ofstream file = createIO<std::ofstream>(path, option, operation);
            if (!file.is_open())
                return false;

            file.write(content.data(), content.size());
            return true;
        }

        std::optional<std::string> read(const fs::path &path,
                                        IO option)
        {
            std::ifstream file = createIO<std::ifstream>(path, option, OP::READ);
            if (!file.is_open())
                return {};

            return std::string((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
        }

        std::shared_ptr<std::string> toBinaryString(const cv::Mat &img)
        {
            //convert to bytes
            std::vector<uchar> imageDataVector;
            cv::imencode(".jpg", img, imageDataVector);
            return std::make_shared<std::string>(
                imageDataVector.begin(), imageDataVector.end());
        }
    }
}