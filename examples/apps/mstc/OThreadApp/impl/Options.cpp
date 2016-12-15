#include "detail/pch.h"
#include <limits>
#include "detail/Options.h"

static const char* listenerPort = "listenerPort:";
static const char* clientPort   = "clientPort:";

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

        if (_strnicmp(arg, listenerPort, strlen(listenerPort)) == 0)
        {
            auto p = std::atoi(arg);
            if (p < 0)
            {
                MSTC_THROW_EXCEPTION(PortFailure{});
            }
            else if (p > (std::numeric_limits<decltype(listenerPort_)>::max)())
            {
                MSTC_THROW_EXCEPTION(PortFailure{});
            }
            else
            {
                listenerPort_ = static_cast<decltype(listenerPort_)>(p);
            }
        }
        else if (_strnicmp(arg, clientPort, strlen(clientPort)) == 0)
        {
            auto p = std::atoi(arg);
            if (p < 0)
            {
                MSTC_THROW_EXCEPTION(PortFailure{});
            }
            else if (p > (std::numeric_limits<decltype(clientPort_)>::max)())
            {
                MSTC_THROW_EXCEPTION(PortFailure{});
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