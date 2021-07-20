#pragma once

#include <functional>
#include <utility>
#include <future>

#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/format.h>

#include <sio_client.h>
#include <sio_message.h>
#include <sio_socket.h>

#include <core/Util.h>

namespace silence
{
    class BaseClient
    {
    public:
        using EventFunction = std::function<
            void(std::string const &name,
                 sio::message::ptr const &data,
                 bool hasAck,
                 sio::message::list &ack_resp)>;

        BaseClient(const std::string &url);
        ~BaseClient();

    public:
        void connect();

        void bindEvent(const std::string &event,
                       const EventFunction &callback);

    protected:
        virtual void onConnected(std::string const &nsp);

        virtual void onFailed();

        virtual void onClosed(sio::client::close_reason const &reason);

    private:
        void setIOEvents();

    protected:
        std::string mUrl;
        std::unique_ptr<sio::client> mIO;
        std::mutex mLock;

        std::condition_variable_any mCond;
        bool connectFinished{false};

        sio::socket::ptr mSock;
    };
}