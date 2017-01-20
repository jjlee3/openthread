#include "detail/pch.h"
#include <sstream>
#include <chrono>
#include <map>
#include <list>
#include <base/StringMacros.h>
#include <base/Exception.h>
#include <base/Scope.h>
#include "detail/StdOutput.h"
#include "detail/Log.h"
#include "detail/Listener.h"
#include "detail/Command.h"
#include "detail/Options.h"

Listener::~Listener()
{
    stop();
}

bool
Listener::start(
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
Listener::stop()
{
    if (stopping_) { return false; }

    sock_.close();
    stopping_ = true;
    thrd_.reset();
    return true;
}

void
Listener::wait()
{
    if (!thrd_) { return; }

    if (thrd_->joinable()) { thrd_->join(); }
}

Listener::sockets_t
Listener::accept(int msTimeout)
{
    const auto& timeslice = timeval{ 0, usPollingTIME };

    using socket_list = std::list<socket_t>;
    using buffer_t = std::vector<char>;
    using buffer_map = std::map<SOCKET, buffer_t>;
    using milliseconds = std::chrono::milliseconds;

    auto        beginTime = std::chrono::steady_clock::now();
    auto        sock = static_cast<SOCKET>(sock_);
    fd_set      fds;
    socket_list socksMightBe;
    buffer_map  buffers;
    sockets_t   socks;

    for (; !stopping_;)
    {
        auto endTime = std::chrono::steady_clock::now();
        auto diff = endTime - beginTime;
        auto msDiff = std::chrono::duration_cast<milliseconds>(diff).count();
        if (msDiff >= msTimeout) { break; }

        FD_ZERO(&fds);
        FD_SET(sock, &fds);
        for (const auto& i : socksMightBe)
        {
            FD_SET(static_cast<SOCKET>(i), &fds);
        }

        try
        {
            sock_.select(&fds, nullptr, nullptr, &timeslice);
        }
        catch (mstc::network::Socket::Failure& e)
        {
            if (!e.whats().empty())
            {
                g_out([&e](std::ostream& os)
                {
                    os << e.whats() << std::endl;
                });
            }
        }

        if (FD_ISSET(sock, &fds))
        {
            sockaddr_in6 saddr;
            int len = sizeof(saddr);
            auto sockAccepted = sock_.accept(&saddr, &len);
            if (static_cast<SOCKET>(sockAccepted) != mstc::network::Ipv6::invalid())
            {
                g_log([&sockAccepted](std::ostream& os)
                {
                    os << __LOCA__ " socket accepted " <<
                        static_cast<SOCKET>(sockAccepted) << std::endl;
                });
                socksMightBe.push_back(std::move(sockAccepted));
            }
        }

        auto it = socksMightBe.begin();
        auto ie = socksMightBe.end();
        decltype(ie) eit;

        for (; it != ie; ++it, (eit != ie) ? socksMightBe.erase(eit) : ie)
        {
            eit = ie;
            auto s = static_cast<SOCKET>(*it);
            if (!FD_ISSET(s, &fds)) { continue; }

            auto& buf = buffers[s];
            auto size = static_cast<int>(buf.size());
            if (size < Command::handshakeCommandLEN)
            {
                buf.resize(Command::handshakeCommandLEN);

                int ret = -1;
                try
                {
                    ret = it->recv(&buf[size], Command::handshakeCommandLEN - size, 0);
                }
                catch (...) // a bad guy
                {
                }

                if (ret <= 0) // remove this bad guy
                {
                    g_log([&it](std::ostream& os)
                    {
                        os << __LOCA__ " bad socket " << static_cast<SOCKET>(*it) <<
                            std::endl;
                    });
                    eit = it;
                    continue;
                }

                buf.resize(size + ret);
                if (memcmp(&buf[0], HANDSHAKE_COMMAND, buf.size()) != 0)
                {
                    std::string signature(&buf[0], &buf[buf.size()]);

                    std::ostringstream oss;
                    oss << __LOCA__ " socket rejected " << static_cast<SOCKET>(*it) <<
                        " " << signature;

                    std::string msg = oss.str();
                    g_log([&msg](std::ostream& os)
                    {
                        os << msg << std::endl;
                    });
                    //msg += "\n";
                    //::OutputDebugStringA(msg.c_str());

                    it->shutdown(SD_SEND);
                    it->closesocket();
                    eit = it;
                    continue;
                }

                if (buf.size() < Command::handshakeCommandLEN) { continue; }
            }

            bool ok = (Command::handshakeCommandLEN > 0) ?
                memcmp(&buf[0], HANDSHAKE_COMMAND, Command::handshakeCommandLEN) == 0 :
                true;
            if (ok)
            {
                std::ostringstream oss;

                oss << __LOCA__ " socket connected " << static_cast<SOCKET>(*it);

                std::string msg = oss.str();
                g_log([&msg](std::ostream& os)
                {
                    os << msg << std::endl;
                });
                //msg += "\n";
                //::OutputDebugStringA(msg.c_str());
                // It is my client!
                socks.push_back(socket_t{it->detach()});
            }
            else
            {
                std::string signature(&buf[0], &buf[buf.size()]);

                std::ostringstream oss;
                oss << __LOCA__ " socket rejected " << static_cast<SOCKET>(*it) <<
                    " " << signature;

                std::string msg = oss.str();
                g_log([&msg](std::ostream& os)
                {
                    os << msg << std::endl;
                });
                //msg += "\n";
                //::OutputDebugStringA(msg.c_str());

                it->shutdown(SD_SEND);
                it->closesocket();
            }
            eit = it;
        }

        if (!socks.empty() && socksMightBe.empty()) { break; }
    }

    return socks;
}

void
Listener::threadMain(
    const Options& options)
{
    std::string msg = "Listener::threadMain started";

    g_log([&msg](std::ostream& os)
    {
        os << msg << std::endl;
    });
    //msg += "\n";
    //::OutputDebugStringA(msg.c_str());

    auto exit = mstc::base::scope([]
    {
        std::string msg = "Listener::threadMain ended";

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
        sock_.bind(&options.serverAddr_, sizeof(options.serverAddr_));

        namespace the_thread = std::this_thread;
        using milliseconds = std::chrono::milliseconds;

#ifdef SOCK_TCP
        sock_.listen(SOMAXCONN);

        for (; !stopping_; the_thread::sleep_for(milliseconds(msSleepTIME)),
            sock_.listen(SOMAXCONN))
        {
            auto joinClients = accept(msTIMEOUT);
            if (joinClients.empty()) { continue; }

            clients_.join(std::move(joinClients), [this](SOCKET s)
            {
                left(s);
            });

            left_clients leftClients;

            leftClients_.mutexInvoke([this, &leftClients]()
            {
                leftClients = leftClients_;
                leftClients_.clear();
            });

            clients_.left(leftClients);
        }
#else
        for (; !stopping_; the_thread::sleep_for(milliseconds(1000)))
        {
            char         buf[256];
            sockaddr_in6 clientAddr;
            int          clientLen = sizeof(clientAddr);

            auto len = sock_.recvfrom(buf, _countof(buf), 0,
                reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
            if (len < 0)
            {
                g_log([](std::ostream& os)
                {
                    os << " server receive ERROR" << std::endl;
                });
                continue;
            }

            if (len == 0) { continue; }

            buf[len] = '\0';
            g_log([&buf](std::ostream& os)
            {
                os << " server receive \"" << buf << "\"" << std::endl;
            });

            if (options.serverName_.empty())
            {
                sprintf_s(&buf[len], _countof(buf) - len, " - server got %d chars", len);
            }
            else
            {
                sprintf_s(&buf[len], _countof(buf) - len, " - server '%s' got %d chars",
                    options.serverName_.c_str(), len);
            }
            len = static_cast<int>(strlen(buf));
            sock_.sendto(buf, len, 0,
                reinterpret_cast<sockaddr*>(&clientAddr), clientLen);
        }
#endif
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

void
Listener::left(SOCKET s)
{
    leftClients_.mutexInvoke([this, s]()
    {
        leftClients_.push_back(s);
    });
}