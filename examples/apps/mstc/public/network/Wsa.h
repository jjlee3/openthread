#pragma once

#include <winsock2.h>
#include <base/Exception.h>
#include <tcKernel/LastError.h>

namespace mstc
{
namespace network
{
class Wsa
{
public:
    EXCEPT_LASTERR_BASE   (Error       , mstc::base::Exception);
    EXCEPT_LASTERR_DERIVED(StartupError, Error);

    Wsa();
    ~Wsa();

private:
    int err_;
}; // Wsa
} // network
} // mstc