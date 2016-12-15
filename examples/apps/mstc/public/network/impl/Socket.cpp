#include "pch/pch.h"
#include <network/Socket.h>

mstc::network::Socket::Socket(Socket&& rhs) noexcept :
    Socket{std::move(rhs.res_)}
{
}

mstc::network::Socket::Socket(R&& res) noexcept :
    res_{std::move(res)}
{
    res = invalid();
}

mstc::network::Socket::~Socket()
{
    close();
}

mstc::network::Socket&
mstc::network::Socket::operator =(
    R&& res) noexcept
{
    attach(std::move(res));
    return *this;
}

mstc::network::Socket&
mstc::network::Socket::operator =(
    Socket&& rhs) noexcept
{
    if (this == &rhs) { return *this; }

    *this = std::move(rhs.res_);
    rhs.res_ = invalid();
    return *this;
}

mstc::network::Socket::R
mstc::network::Socket::get() const noexcept
{
    return res_;
}

void
mstc::network::Socket::attach(
    R&& res) noexcept
{
    close();
    res_ = res;
    res = invalid();
    return;
}

mstc::network::Socket::R
mstc::network::Socket::detach() noexcept
{
    auto res = res_;
    res_ = invalid();
    return res;
}

int
mstc::network::Socket::close() noexcept
{
    if (res_ == invalid()) { return 0; }

    auto ret =::closesocket(res_);
    res_ = invalid();
    return ret;
}

void
mstc::network::Socket::swap(
    mstc::network::Socket& rhs) noexcept
{
    using std::swap;

    swap(res_, rhs.res_);
}

void
mstc::network::Socket::socket(
    int addrFamily,
    int type,
    int protocol)
{
    R s = ::socket(addrFamily, type, protocol);

    if (s != invalid())
    {
        attach(std::move(s));
    }
    else
    {
        auto err = ::WSAGetLastError();
        MSTC_THROW_EXCEPTION(SocketError(err));
    }
}

void
mstc::network::Socket::bind(
    const sockaddr* saddr,
    int             len) const
{
    auto ret = ::bind(res_, saddr, len);
    if (!ret) { return; }

    if (ret == SOCKET_ERROR)
    {
        auto err = ::WSAGetLastError();
        MSTC_THROW_EXCEPTION(BindError(err));
    }
    else
    {
        MSTC_THROW_EXCEPTION(BindFailure{});
    }
}

void
mstc::network::Socket::listen(
    int backlog) const
{
    auto ret = ::listen(res_, backlog);
    if (!ret) { return; }

    if (ret == SOCKET_ERROR)
    {
        auto err = ::WSAGetLastError();
        MSTC_THROW_EXCEPTION(ListenError(err));
    }
    else
    {
        MSTC_THROW_EXCEPTION(ListenFailure{});
    }
}

void
mstc::network::Socket::ioctl(
    long           cmd,
    unsigned long* arg) const
{
    auto ret = ::ioctlsocket(res_, cmd, arg);
    if (!ret) { return; }

    if (ret == SOCKET_ERROR)
    {
        auto err = ::WSAGetLastError();
        MSTC_THROW_EXCEPTION(IoctlError(err));
    }
    else
    {
        MSTC_THROW_EXCEPTION(IoctlFailure{});
    }
} 

void mstc::network::Socket::select(
    fd_set*        readfds,
    fd_set*        writefds,
    fd_set*        exceptfds,
    const timeval* timeout) const
{
    auto nfds = 0;

    if (readfds != nullptr)
    {
        if (static_cast<int>(readfds->fd_count) > nfds)
        {
            nfds = readfds->fd_count;
        }
    }
    if (writefds != nullptr)
    {
        if (static_cast<int>(writefds->fd_count) > nfds)
        {
            nfds = writefds->fd_count;
        }
    }
    if (exceptfds != nullptr)
    {
        if (static_cast<int>(exceptfds->fd_count) > nfds)
        {
            nfds = exceptfds->fd_count;
        }
    }
    ++nfds;

    auto ret = ::select(nfds, readfds, writefds, exceptfds, timeout);
    if (ret >= 0) { return; }

    if (ret == SOCKET_ERROR)
    {
        auto err = ::WSAGetLastError();
        MSTC_THROW_EXCEPTION(SelectError(err));
    }
    else
    {
        MSTC_THROW_EXCEPTION(SelectFailure{});
    }
}

mstc::network::Socket
mstc::network::Socket::accept(
    sockaddr* saddr,
    int*      len) const
{
    auto s = ::accept(res_, saddr, len);
    if (s != invalid())
    {
        return Socket(std::move(s));
    }
    else
    {
        auto err = ::WSAGetLastError();
        MSTC_THROW_EXCEPTION(AcceptError(err));
    }
}

void
mstc::network::Socket::connect(
    const sockaddr* saddr,
    int             len) const
{
    auto ret = ::connect(res_, saddr, len);
    if (!ret) { return; }

    if (ret == SOCKET_ERROR)
    {
        auto err = ::WSAGetLastError();
        MSTC_THROW_EXCEPTION(ConnectError(err));
    }
    else
    {
        MSTC_THROW_EXCEPTION(ConnectFailure{});
    }
}

int
mstc::network::Socket::send(
    const void* buf,
    int         len,
    int         flags) const
{
    auto ret = ::send(res_, static_cast<const char*>(buf), len, flags);
    if (ret >= 0) { return ret; }

    if (ret == SOCKET_ERROR)
    {
        auto err = ::WSAGetLastError();
        MSTC_THROW_EXCEPTION(SendError(err));
    }
    else
    {
        MSTC_THROW_EXCEPTION(SendFailure{});
    }
}

int
mstc::network::Socket::sendExact(
    const void* buf,
    int         len,
    int         flags) const
{
    for (int l = 0; l < len;)
    {
        auto ret = send(static_cast<const char*>(buf) +  l, len - l, flags);
        if (!ret) { MSTC_THROW_EXCEPTION(SendFailure{}); }

        l += ret;
    }

    return len;
}

int
mstc::network::Socket::sendto(
    const void*     buf,
    int             len,
    int             flags,
    const sockaddr* to,
    int             tolen) const
{
    auto ret = ::sendto(res_, static_cast<const char*>(buf), len, flags,
        to, tolen);
    if (ret >= 0) { return ret; }

    if (ret == SOCKET_ERROR)
    {
        auto err = ::WSAGetLastError();
        MSTC_THROW_EXCEPTION(SendToError(err));
    }
    else
    {
        MSTC_THROW_EXCEPTION(SendToFailure{});
    }
}

int
mstc::network::Socket::recv(
    void* buf,
    int   len,
    int   flags) const
{
    auto ret = ::recv(res_, static_cast<char*>(buf), len, flags);
    if (ret >= 0) { return ret; }

    if (ret == SOCKET_ERROR)
    {
        auto err = ::WSAGetLastError();
        MSTC_THROW_EXCEPTION(RecvError(err));
    }
    else
    {
        MSTC_THROW_EXCEPTION(RecvFailure{});
    }
}

int
mstc::network::Socket::recvExact(
    void* buf,
    int   len,
    int   flags) const
{
    for (int l = 0; l < len;)
    {
        auto ret = recv(static_cast<char*>(buf) +  l, len - l, flags);
        if (!ret) { MSTC_THROW_EXCEPTION(RecvFailure{}); }

        l += ret;
    }

    return len;
}

int
mstc::network::Socket::recvfrom(
    void*     buf,
    int       len,
    int       flags,
    sockaddr* from,
    int*      fromlen) const
{
    auto ret = ::recvfrom(res_, static_cast<char*>(buf), len, flags,
        from, fromlen);
    if (ret >= 0) { return ret; }

    if (ret == SOCKET_ERROR)
    {
        auto err = ::WSAGetLastError();
        MSTC_THROW_EXCEPTION(RecvFromError(err));
    }
    else
    {
        MSTC_THROW_EXCEPTION(RecvFromFailure{});
    }
}

void
mstc::network::Socket::getsockname(
    sockaddr* saddr,
    int*      len) const
{
    auto ret = ::getsockname(res_, saddr, len);
    if (!ret) { return; }

    if (ret == SOCKET_ERROR)
    {
        auto err = ::WSAGetLastError();
        MSTC_THROW_EXCEPTION(GetSockNameError(err));
    }
    else
    {
        MSTC_THROW_EXCEPTION(GetSockNameFailure{});
    }
}

void
mstc::network::Socket::setsockopt(
    int         level,
    int         optname,
    const void* optval,
    int         optlen)
{
    auto ret = ::setsockopt(res_, level, optname,
        static_cast<const char*>(optval), optlen);
    if (!ret) { return; }

    if (ret == SOCKET_ERROR)
    {
        auto err = ::WSAGetLastError();
        MSTC_THROW_EXCEPTION(SetSockOptError(err));
    }
    else
    {
        MSTC_THROW_EXCEPTION(SetSockOptFailure{});
    }
}

void
mstc::network::Socket::closesocket()
{
    auto ret = close();
    if (!ret) { return; }

    if (ret == SOCKET_ERROR)
    {
        auto err = ::WSAGetLastError();
        MSTC_THROW_EXCEPTION(CloseSocketError(err));
    }
    else
    {
        MSTC_THROW_EXCEPTION(CloseSocketFailure{});
    }
}

void
mstc::network::Socket::shutdown(
    int how)
{
    auto ret = ::shutdown(res_, how);
    if (!ret) { return; }

    if (ret == SOCKET_ERROR)
    {
        auto err = ::WSAGetLastError();
        MSTC_THROW_EXCEPTION(ShutdownError(err));
    }
    else
    {
        MSTC_THROW_EXCEPTION(ShutdownFailure{});
    }
}