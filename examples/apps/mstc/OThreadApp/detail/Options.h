#pragma once

#include <ws2ipdef.h>
#include <mstcpip.h>
#include <base/Exception.h>

struct Options
{
    EXCEPT_BASE   (Failure);
    EXCEPT_DERIVED(PortFailure , Failure);
    EXCEPT_DERIVED(ParseFailure, Failure);

    void parse(int argc, char** argv);

    static constexpr unsigned short DEF_LISTENER_PORT = 51000; // 0xC738
    static constexpr unsigned short DEF_CLIENT_PORT   = 51100; // 0xC79C

    sockaddr_in6   listenerAddr_ = { AF_INET6 };
    unsigned short listenerPort_ = DEF_LISTENER_PORT;

    sockaddr_in6   clientAddr_   = { AF_INET6 };
    unsigned short clientPort_   = DEF_CLIENT_PORT;
};

extern Options g_options;