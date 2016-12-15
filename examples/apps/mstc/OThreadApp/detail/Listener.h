#pragma once

#include <vector>
#include <memory>
#include <base/Protector.h>
#include <base/ScopedThread.h>
#include <network/Ipv6.h>
#include "detail/Clients.h"

struct Options;

class Listener
{
public:
    ~Listener();

    bool start(const Options&);
    bool stop();
    void wait();

protected:
    static constexpr int msSleepTIME   = 500;     // milli secs
    static constexpr int msTIMEOUT     = 60000; // milli secs
    static constexpr int usPollingTIME = 5000 * 1000; // micro secs

    using thread_t        = mstc::base::ScopedThread;
    using thread_uptr     = std::unique_ptr<thread_t>;
    using thread_csptr    = thread_uptr;
    using socket_t        = mstc::network::Ipv6;
    using sockets_t       = std::vector<socket_t>;
    using left_clients    = Clients::left_clients;
    using protect_clients = mstc::base::Protector<left_clients>;

    sockets_t accept(int msTimeout);
    void threadMain(const Options&);
    void left(SOCKET);

    thread_csptr    thrd_;
    bool            stopping_ = true;
    socket_t        sock_;
    Clients         clients_;
    protect_clients leftClients_;
};