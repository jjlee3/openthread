#include "detail/pch.h"
#include <sstream>
#include <base/Exception.h>
#include <base/Scope.h>
#include "detail/Log.h"
#include "detail/ClientService.h"
#include "detail/StdOutput.h"
#include "detail/Options.h"

ClientService::ClientService(
    socket_t&&                  sock,
    const notify_left_function& leftFun) :
    s_{static_cast<SOCKET>(sock)}, sock_{std::move(sock)}, leftFun_{leftFun}
{
}

ClientService::~ClientService()
{
    stop();
}

ClientService&
ClientService::operator =(
    ClientService&& rhs)
{
    s_ = std::move(rhs.s_);
    rhs.s_ = socket_t::invalid();
    sock_ = std::move(rhs.sock_);
    thrd_ = std::move(rhs.thrd_);
    stopping_ = std::move(rhs.stopping_);
    rhs.stopping_ = true;
    leftFun_ = std::move(rhs.leftFun_);
    return *this;
}

bool
ClientService::start()
{
    if (!stopping_) { return false; }

    stopping_ = false;
    thrd_ = std::make_unique<thread_t>([this]()
    {
        threadMain(g_options);
    });
    return true;
}

bool
ClientService::stop()
{
    if (stopping_) { return false; }

    stopping_ = true;
    thrd_.reset();
    return true;
}

void
ClientService::threadMain(
    const Options& options)
{
    std::string msg = "ClientService::threadMain ended";

    g_log([&msg](std::ostream& os)
    {
        os << msg << std::endl;
    });
    //msg += "\n";
    //::OutputDebugStringA(msg.c_str());

    auto exit = mstc::base::scope([this]
    {
        if (leftFun_) { leftFun_(s_); }

        std::string msg = "ClientService::threadMain ended";

        g_log([&msg](std::ostream& os)
        {
            os << msg << std::endl;
        });
        //msg += "\n";
        //::OutputDebugStringA(msg.c_str());
    });

    for (; !stopping_;)
    {
        try
        {
            char buf[256];
            auto len = sock_.recv(buf, sizeof(buf), 0);
            if (len <= 0) { continue; }

            buf[len] = '\0';
            g_out([&buf](std::ostream& os)
            {
               os << " server receive \"" << buf << "\" from client" << std::endl;
            });

            if (options.listenerName_.empty())
            {
                sprintf_s(&buf[len], _countof(buf) - len, " - server got %d chars", len);
            }
            else
            {
                sprintf_s(&buf[len], _countof(buf) - len, " - server '%s' got %d chars",
                    options.listenerName_.c_str(), len);
            }
            len = static_cast<int>(strlen(buf));
            sock_.send(buf, len, 0);
        }
        catch (mstc::network::Socket::Error& e)
        {
            std::string msg = e.diagnose();

            g_log([&msg](std::ostream& os)
            {
                os << msg << std::endl;
            });
            //msg += "\n";
            //::OutputDebugStringA(msg.c_str());
            break;
        }
        catch (mstc::network::Socket::Failure& e)
        {
            std::string msg = e.diagnose();

            g_log([&msg](std::ostream& os)
            {
                os << msg << std::endl;
            });
            //msg += "\n";
            //::OutputDebugStringA(msg.c_str());
            break;
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
}