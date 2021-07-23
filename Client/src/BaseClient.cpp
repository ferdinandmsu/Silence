#include <BaseClient.h>

#ifdef WINDOWS
#define BIND_EVENT(IO, EV, FN)             \
    do                                     \
    {                                      \
        socket::event_listener_aux l = FN; \
        IO->on(EV, l);                     \
    } while (0)

#else
#define BIND_EVENT(IO, EV, FN) \
    IO->on(EV, FN)
#endif

namespace silence
{
    BaseClient::BaseClient(const std::string &url)
        : mUrl(url), mIO(new sio::client())
    {
        mSocket = mIO->socket();
    }

    BaseClient::~BaseClient()
    {
        mSocket->off_all();
        mSocket->off_error();
    }

    void BaseClient::connect()
    {
        mIO->connect(mUrl);
        mLock.lock();
        if (!connectFinished)
        {
            mCond.wait(mLock);
        }
        mLock.unlock();
    }

    void BaseClient::bindEvent(const std::string &event,
                               const EventFunction &callback)
    {
        BIND_EVENT(mSocket, event, callback);
    }

    void BaseClient::onConnected(std::string const &nsp)
    {
        mLock.lock();
        mCond.notify_all();
        connectFinished = true;
        mLock.unlock();
    }

    void BaseClient::onFailed()
    {
        exit(-1);
    }

    void BaseClient::onClosed(sio::client::close_reason const &reason)
    {
        exit(0);
    }

    void BaseClient::setIOEvents()
    {
        mIO->set_socket_open_listener(std::bind(&BaseClient::onConnected, this, std::placeholders::_1));
        mIO->set_close_listener(std::bind(&BaseClient::onClosed, this, std::placeholders::_1));
        mIO->set_fail_listener(std::bind(&BaseClient::onFailed, this));
    }
}