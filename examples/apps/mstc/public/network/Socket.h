#pragma once

#include <winsock2.h>
#include <base/Exception.h>
#include <tcKernel/LastError.h>

namespace mstc
{
namespace network
{
class Socket
{
public:
    EXCEPT_BASE           (Failure);
    EXCEPT_DERIVED        (BindFailure       , Failure);
    EXCEPT_DERIVED        (ListenFailure     , Failure);
    EXCEPT_DERIVED        (IoctlFailure      , Failure);
    EXCEPT_DERIVED        (SelectFailure     , Failure);
    EXCEPT_DERIVED        (ConnectFailure    , Failure);
    EXCEPT_DERIVED        (SendFailure       , Failure);
    EXCEPT_DERIVED        (SendToFailure     , Failure);
    EXCEPT_DERIVED        (RecvFailure       , Failure);
    EXCEPT_DERIVED        (RecvFromFailure   , Failure);
    EXCEPT_DERIVED        (GetSockNameFailure, Failure);
    EXCEPT_DERIVED        (SetSockOptFailure , Failure);
    EXCEPT_DERIVED        (CloseSocketFailure, Failure);
    EXCEPT_DERIVED        (ShutdownFailure   , Failure);

    EXCEPT_LASTERR_BASE   (Error           , Failure);
    EXCEPT_LASTERR_DERIVED(SocketError     , Error);
    EXCEPT_LASTERR_DERIVED(BindError       , Error);
    EXCEPT_LASTERR_DERIVED(ListenError     , Error);
    EXCEPT_LASTERR_DERIVED(IoctlError      , Error);
    EXCEPT_LASTERR_DERIVED(SelectError     , Error);
    EXCEPT_LASTERR_DERIVED(AcceptError     , Error);
    EXCEPT_LASTERR_DERIVED(ConnectError    , Error);
    EXCEPT_LASTERR_DERIVED(SendError       , Error);
    EXCEPT_LASTERR_DERIVED(SendToError     , Error);
    EXCEPT_LASTERR_DERIVED(RecvError       , Error);
    EXCEPT_LASTERR_DERIVED(RecvFromError   , Error);
    EXCEPT_LASTERR_DERIVED(GetSockNameError, Error);
    EXCEPT_LASTERR_DERIVED(SetSockOptError , Error);
    EXCEPT_LASTERR_DERIVED(CloseSocketError, Error);
    EXCEPT_LASTERR_DERIVED(ShutdownError   , Error);

public:
    // resourec type
    using R = SOCKET;

    inline static constexpr R invalid() { return INVALID_SOCKET; }

    Socket(Socket&& rhs) noexcept;
    explicit Socket(R&& res = invalid()) noexcept;
    ~Socket();

    Socket& operator =(R&& res) noexcept;
    Socket& operator =(Socket&& rhs) noexcept;

    R get() const noexcept;
    operator const R&() const noexcept { return res_; }

    void attach(R&& res) noexcept;
    R detach() noexcept;
    int close() noexcept;

    void swap(Socket& rhs) noexcept;

    void socket(int addrFamily, int type, int protocol);
    void bind(const sockaddr*, int len) const;
    void listen(int backlog) const;
    void ioctl(long cmd, unsigned long* arg) const;
    void select(fd_set* readfds, fd_set* writefds, fd_set* exceptfds,
        const timeval* timeout) const;
    Socket accept(sockaddr*, int* len) const;
    void connect(const sockaddr*, int len) const;
    int send(const void* buf, int len, int flags) const;
    int sendExact(const void* buf, int len, int flags) const;
    int sendto(const void* buf, int len, int flags,
        const sockaddr* to, int tolen) const;
    int recv(void* buf, int len, int flags) const;
    int recvExact(void* buf, int len, int flags) const;
    int recvfrom(void* buf, int len, int flags,
        sockaddr* from, int* fromlen) const;
    void getsockname(sockaddr*, int* len) const;
    void setsockopt(int level, int optname, const void* optval, int optlen);
    void closesocket();
    void shutdown(int how);

protected:
    Socket(const Socket&) = delete;
    Socket& operator =(const Socket&) = delete;

    R res_ = invalid();
}; // Socket
} // network
} // mstc