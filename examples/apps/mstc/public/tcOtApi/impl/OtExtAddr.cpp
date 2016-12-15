#include "pch/pch.h"
#include <tcOtApi/OtDevice.h>
#include <tcOtApi/OtExtAddr.h>

mstc::ot::KExtendedAddress::KExtendedAddress(
    otInstance* pDevice) :
    KMemory{::otGetExtendedAddress(pDevice)}
{
    if (!res_)
    {
        MSTC_THROW_EXCEPTION(OtGetExtendedAddressFailure{});
    }
}
