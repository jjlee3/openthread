#pragma once

#include <base/ScopedThread.h>
#include <network/Ipv6.h>
#include "detail/NotifyFun.h"

// service for each client from the server
class ClientService
{
public:
    using socket_t = mstc::network::Ipv6;

    ClientService() {}
    ClientService(socket_t&&, const notify_left_function&);
    ~ClientService();

    ClientService& operator =(ClientService&&);

    bool start();
    bool stop();

protected:
    using thread_t     = mstc::base::ScopedThread;
    using thread_uptr  = std::unique_ptr<thread_t>;
    using thread_csptr = thread_uptr;

    void threadMain();

    SOCKET               s_ = mstc::network::Ipv6::invalid();
    socket_t             sock_;
    thread_csptr         thrd_;
    bool                 stopping_ = true;
    notify_left_function leftFun_  = nullptr;
}; // ClientService