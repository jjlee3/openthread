#include "detail/pch.h"
#include <limits>
#include "detail/Options.h"

static const char* serverName = "serverName:";
static const char* serverIP   = "serverIP:";
static const char* serverPort = "serverPort:";
static const char* clientIP   = "clientIP:";
static const char* clientPort = "clientPort:";

Options g_options;

void
Options::parse(
    int    argc,
    char** argv)
{
    if (argc < 1)
    {
        MSTC_THROW_EXCEPTION(ParseFailure{"Arguments too few"});
    }

    for (int i = 1; i < argc; ++i)
    {
        auto arg = argv[i];
        if (*arg != '/' && *arg != '-') { continue; }

        ++arg;
        if (_strnicmp(arg, serverName, strlen(serverName)) == 0)
        {
            serverName_ = arg + strlen(serverName);
        }
        else if (_strnicmp(arg, serverIP, strlen(serverIP)) == 0)
        {
            const char* terminator = nullptr;
            ::RtlIpv6StringToAddressA(arg + strlen(serverIP),
                &terminator, &serverAddr_.sin6_addr);
        }
        else if (_strnicmp(arg, serverPort, strlen(serverPort)) == 0)
        {
            auto p = std::atoi(arg + strlen(serverPort));
            if (p < 0)
            {
                MSTC_THROW_EXCEPTION(PortFailure{"listener port"});
            }
            else if (p > (std::numeric_limits<decltype(serverPort_)>::max)())
            {
                MSTC_THROW_EXCEPTION(PortFailure{"listener port"});
            }
            else
            {
                serverPort_ = static_cast<decltype(serverPort_)>(p);
            }
        }
        else if (_strnicmp(arg, clientIP, strlen(clientIP)) == 0)
        {
            const char* terminator = nullptr;
            ::RtlIpv6StringToAddressA(arg + strlen(clientIP),
                &terminator, &clientAddr_.sin6_addr);
        }
        else if (_strnicmp(arg, clientPort, strlen(clientPort)) == 0)
        {
            auto p = std::atoi(arg + strlen(clientPort));
            if (p < 0)
            {
                MSTC_THROW_EXCEPTION(PortFailure{"client port"});
            }
            else if (p > (std::numeric_limits<decltype(clientPort_)>::max)())
            {
                MSTC_THROW_EXCEPTION(PortFailure{"client port"});
            }
            else
            {
                clientPort_ = static_cast<decltype(clientPort_)>(p);
            }
        }
        else
        {
        }
    }

    serverAddr_.sin6_port = ::htons(serverPort_);
    clientAddr_.sin6_port   = ::htons(clientPort_);
}