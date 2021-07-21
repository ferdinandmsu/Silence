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
        void onCommand(std::string const &name,
                       sio::message::ptr const &data,
                       bool hasAck,
                       sio::message::list &ack_resp);

    protected:
        sio::message::list
        createJSObject(const std::map<std::string, sio::message::ptr> &object);

    protected:
        void whoisEvent();
        void screenshotEvent();
        void liveStreamEvent();
        void unknownEvent(const std::string &event);
    };
}