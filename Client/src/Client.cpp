#include <Client.h>

namespace silence
{
    Client::Client(const std::string &url)
        : IOClient(url)
    {
        // bind events
        IOClient::bindEvent("command",
                            std::bind(&Client::onCommand,
                                      this, std::placeholders::_1,
                                      std::placeholders::_2,
                                      std::placeholders::_3, std::placeholders::_4));
    }

    void Client::onConnected(std::string const &nsp)
    {
        // important
        IOClient::onConnected(nsp);
    }

    void Client::onFailed()
    {
        std::cout << "sio failed" << std::endl;
        exit(-1);
    }

    void Client::onClosed(sio::client::close_reason const &reason)
    {
        std::cout << "sio closed" << std::endl;
        exit(0);
    }

    void Client::onCommand(std::string const &name,
                           sio::message::ptr const &data,
                           bool hasAck,
                           sio::message::list &ack_resp)
    {
        auto commandObject = data->get_map();
        std::string command = commandObject["command"]->get_string();

        if (command == "screenshot")
        {
                }
    }
}