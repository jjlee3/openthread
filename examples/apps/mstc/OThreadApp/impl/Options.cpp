#include "detail/pch.h"
#include <limits>
#include "detail/Options.h"

static const char* listenerName = "listenerName:";
static const char* listenerIP   = "listenerIP:";
static const char* listenerPort = "listenerPort:";
static const char* clientIP     = "clientIP:";
static const char* clientPort   = "clientPort:";

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
        if (_strnicmp(arg, listenerName, strlen(listenerName)) == 0)
        {
            listenerName_ = arg + strlen(listenerName);
        }
        else if (_strnicmp(arg, listenerIP, strlen(listenerIP)) == 0)
        {
            const char* terminator = nullptr;
            ::RtlIpv6StringToAddressA(arg + strlen(listenerIP),
                &terminator, &listenerAddr_.sin6_addr);
        }
        else if (_strnicmp(arg, listenerPort, strlen(listenerPort)) == 0)
        {
            auto p = std::atoi(arg + strlen(listenerPort));
            if (p < 0)
            {
                MSTC_THROW_EXCEPTION(PortFailure{"listener port"});
            }
            else if (p > (std::numeric_limits<decltype(listenerPort_)>::max)())
            {
                MSTC_THROW_EXCEPTION(PortFailure{"listener port"});
            }
            else
            {
                listenerPort_ = static_cast<decltype(listenerPort_)>(p);
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

    listenerAddr_.sin6_port = ::htons(listenerPort_);
//  listenerAddr_.sin6_port = listenerPort_;
    clientAddr_.sin6_port   = ::htons(clientPort_);
//  clientAddr_.sin6_port   = clientPort_;
}