#pragma once

#include <ws2ipdef.h>
#include <mstcpip.h>
#include <string>
#include <base/Exception.h>

struct Options
{
    EXCEPT_BASE   (Failure);
    EXCEPT_DERIVED(PortFailure , Failure);
    EXCEPT_DERIVED(ParseFailure, Failure);

    void parse(int argc, char** argv);

    static constexpr unsigned short DEF_SERVER_PORT = 51000; // 0xC738
    static constexpr unsigned short DEF_CLIENT_PORT = 51100; // 0xC79C

    std::string    serverName_;

    sockaddr_in6   serverAddr_ = { AF_INET6 };
    unsigned short serverPort_ = DEF_SERVER_PORT;

    sockaddr_in6   clientAddr_ = { AF_INET6 };
    unsigned short clientPort_ = DEF_CLIENT_PORT;
};

extern Options g_options;