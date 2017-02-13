#include "detail/pch.h"
#include <iostream>
#include "detail/StdOutput.h"
#include "detail/Log.h"
#include "detail/Client.h"
#include "detail/Options.h"

Client::~Client()
{
    stop();
}

bool
Client::start(
    const Options& options)
{
    if (!stopping_) { return false; }

    stopping_ = false;
    thrd_ = std::make_unique<thread_t>([&options, this]()
    {
        threadMain(options);
    });
    return true;
}

bool
Client::stop()
{
    if (stopping_) { return false; }

    sock_.close();
    stopping_ = true;
    thrd_.reset();
    return true;
}

void
Client::wait()
{
    if (!thrd_) { return; }

    if (thrd_->joinable()) { thrd_->join(); }
}

void
Client::threadMain(
    const Options& options)
{
    std::string msg = "Client::threadMain started";

    g_log([&msg](std::ostream& os)
    {
        os << msg << std::endl;
    });
    //msg += "\n";
    //::OutputDebugStringA(msg.c_str());

    auto exit = mstc::base::scope([]
    {
        std::string msg = "Client::threadMain ended";

        g_log([&msg](std::ostream& os)
        {
            os << msg << std::endl;
        });
        //msg += "\n";
        //::OutputDebugStringA(msg.c_str());
    });

    try
    {
        sock_.closesocket();
#ifdef SOCK_TCP
        sock_.socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
#else
        sock_.socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
#endif
        auto optval = 1;
        sock_.setsockopt(IPPROTO_IPV6, IPV6_V6ONLY,
            reinterpret_cast<const char*>(&optval), sizeof(optval));
        sock_.setsockopt(SOL_SOCKET, SO_REUSEADDR,
            reinterpret_cast<const char*>(&optval), sizeof(optval));
        sock_.bind(&options.clientAddr_, sizeof(options.clientAddr_));

        namespace the_thread = std::this_thread;
        using milliseconds = std::chrono::milliseconds;

#ifdef SOCK_TCP
        sock_.connect(&options.serverAddr_, sizeof(options.serverAddr_));
#else
        sockaddr_in6 listenerAddr = options.serverAddr_;
        int          listenerLen  = sizeof(listenerAddr);
#endif

        std::string line;

        for (; !stopping_; the_thread::sleep_for(milliseconds(msSleepTIME)))
        {
            if (!std::getline(std::cin, line)) { break; }

            auto len = static_cast<int>(line.size());
            char buf[256];
#ifdef SOCK_TCP
            sock_.send(line.c_str(), len, 0);

            len = sock_.recv(buf, sizeof(buf), 0);
#else
            sock_.sendto(line.c_str(), len, 0,
                reinterpret_cast<const sockaddr*>(&listenerAddr), listenerLen);

            len = sock_.recvfrom(buf, _countof(buf), 0,
                reinterpret_cast<sockaddr*>(&listenerAddr), &listenerLen);
#endif
            if (len < 0)
            {
                g_log([](std::ostream& os)
                {
                    os << "client receive ERROR" << std::endl;
                });
                continue;
            }

            if (len == 0) { continue; }

            buf[len] = '\0';
            g_out([&buf](std::ostream& os)
            {
               os << " client receive \"" << buf << "\" from server" << std::endl;
            });
        }
    }
    catch (mstc::base::Exception& e)
    {
        std::string msg = e.diagnose();

        g_log([&msg](std::ostream& os)
        {
            os << msg << std::endl;
        });
        //msg += "\n";
        //::OutputDebugStringA(msg.c_str());
    }
    catch (std::exception& e)
    {
        std::string msg = e.what();

        g_log([&msg](std::ostream& os)
        {
            os << msg << std::endl;
        });
        //msg += "\n";
        //::OutputDebugStringA(msg.c_str());
    }
    catch (...)
    {
        std::string msg = __FUNCTION__ " unknown exception";

        g_log([&msg](std::ostream& os)
        {
            os << msg << std::endl;
        });
        //msg += "\n";
        //::OutputDebugStringA(msg.c_str());
    }
}