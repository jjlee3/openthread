#include "pch.h"
#include "OtDevice.h"
#include "OtExtAddr.h"

ot::KExtendedAddress::KExtendedAddress(
    const KDevice& kDevice) :
    KExtendedAddress{kDevice.get()}
{
}

ot::KExtendedAddress::KExtendedAddress(
    otInstance* pDevice) :
    KMemory{::otGetExtendedAddress(pDevice)}
{
    if (res_ == invalid())
    {
        throw Exception("KExtendedAddress::KExtendedAddress otGetExtendedAddress Failure");
    }
}
