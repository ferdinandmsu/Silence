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
        Client(const std::string &url);
        ~Client();

        void connect();

    protected:
        void onConnected(const std::string &nsp);

        void onFailed();

        void onClosed(sio::client::close_reason const &reason);

    protected:
        sio::message::list
        createObject(const std::map<std::string, sio::message::ptr> &object);

    protected:
        void onGreeting(std::string const &name,
                        sio::message::ptr const &data,
                        bool hasAck,
                        sio::message::list &ack_resp);

        void onCommand(std::string const &name,
                       sio::message::ptr const &data,
                       bool hasAck,
                       sio::message::list &ack_resp);

    protected:
        void screenshotEvent();

        void killStreamEvent();
        void startStreamEvent();

        void errorEvent(const std::string &event, const std::string &msg);
        void infoEvent(const std::string &info);

    private:
        std::string mUrl;
        std::unique_ptr<sio::client> mIO;
        std::mutex mLock;
        std::mutex mStreamLocker;

        std::condition_variable_any mCond;
        bool connectFinished{false};
        bool mStreamRunning{false};

        sio::socket::ptr mSocket;
    };
}