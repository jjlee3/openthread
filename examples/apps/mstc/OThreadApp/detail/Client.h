#pragma once

#include <base/ScopedThread.h>
#include <network/Ipv6.h>

struct Options;

class Client
{
public:
    ~Client();

    bool start(const Options&);
    bool stop();
    void wait();

protected:
    static constexpr int msSleepTIME   = 1000;     // milli secs

    using thread_t        = mstc::base::ScopedThread;
    using thread_uptr     = std::unique_ptr<thread_t>;
    using thread_csptr    = thread_uptr;
    using socket_t        = mstc::network::Ipv6;

    void threadMain(const Options&);

    thread_csptr thrd_;
    bool         stopping_ = true;
    socket_t     sock_;
};