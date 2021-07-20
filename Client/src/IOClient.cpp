#include <IOClient.h>

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
    IOClient::IOClient(const std::string &url)
        : mUrl(url), mIO(new sio::client())
    {
        mSock = mIO->socket();
    }

    IOClient::~IOClient()
    {
        mIO->socket()->off_all();
        mIO->socket()->off_error();
    }

    void IOClient::connect()
    {
        mIO->connect(mUrl);
        mLock.lock();
        if (!connectFinished)
        {
            mCond.wait(mLock);
        }
        mLock.unlock();
    }

    void IOClient::bindEvent(const std::string &event,
                             const EventFunction &callback)
    {
        BIND_EVENT(mIO->socket(), event, callback);
    }

    void IOClient::onConnected(std::string const &nsp)
    {
        mLock.lock();
        mCond.notify_all();
        connectFinished = true;
        mLock.unlock();
    }

    void IOClient::onFailed()
    {
        exit(-1);
    }

    void IOClient::onClosed(sio::client::close_reason const &reason)
    {
        exit(0);
    }

    void IOClient::setIOEvents()
    {
        mIO->set_socket_open_listener(std::bind(&IOClient::onConnected, this, std::placeholders::_1));
        mIO->set_close_listener(std::bind(&IOClient::onClosed, this, std::placeholders::_1));
        mIO->set_fail_listener(std::bind(&IOClient::onFailed, this));
    }
}