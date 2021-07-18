#pragma once

#include <core/Util.h>
#include <core/IOClient.h>

namespace silence
{
    class Client : public IOClient
    {
    public:
        Client(const std::string &url);

    protected:
        void onConnected(std::string const &nsp) final;

        void onFailed() final;

        void onClosed(sio::client::close_reason const &reason) final;

    protected:
        void onCommand(std::string const &name,
                       sio::message::ptr const &data,
                       bool hasAck,
                       sio::message::list &ack_resp);
    };
}