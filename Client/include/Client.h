#pragma once

#include <BaseClient.h>

namespace silence
{
    class Client : public BaseClient
    {
    public:
        Client(const std::string &url);

    protected:
        void onFailed() final;

        void onClosed(sio::client::close_reason const &reason) final;

    protected:
        sio::message::list
        createObject(const std::map<std::string, sio::message::ptr> &object);

    protected:
        void onCommand(std::string const &name,
                       sio::message::ptr const &data,
                       bool hasAck,
                       sio::message::list &ack_resp);

    protected:
        void whoisEvent();

        void screenshotEvent();

        void killStreamEvent();
        void startStreamEvent();

        void errorEvent(const std::string &event, const std::string &msg);
        void infoEvent(const std::string &info);

    private:
        std::mutex mStreamLocker;
        bool mStreamRunning{false};
    };
}