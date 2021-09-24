#include <core/Util.h>

namespace silence::impl {
    std::optional<std::string> exec(const char *command) {
        std::array<char, 128> buffer{};
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

    std::string hostname() {
        char hostnameBuffer[500];
        gethostname(hostnameBuffer, 500);
        return hostnameBuffer;
    }

    std::shared_ptr<std::string> toBinaryString(const cv::Mat &img) {
        std::vector<uchar> imageDataVector;
        cv::imencode(".jpg", img, imageDataVector);
        return std::make_shared<std::string>(
                imageDataVector.begin(), imageDataVector.end());
    }
}