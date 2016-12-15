#include "pch/pch.h"
#include <network/Wsa.h>

mstc::network::Wsa::Wsa() : err_(WSASYSNOTREADY)
{
    WSADATA wsaData;
    err_ = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err_) { MSTC_THROW_EXCEPTION(StartupError(err_)); }
}

mstc::network::Wsa::~Wsa()
{
    if (!err_) { ::WSACleanup(); }
}