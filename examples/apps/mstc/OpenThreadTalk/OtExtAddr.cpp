#include "pch.h"
#include "Types.h"
#include "OtDevice.h"
#include "OtExtAddr.h"

using namespace ot;

ExtendedAddress::ExtendedAddress(
    const Device& dev) :
    ExtendedAddress{ dev.get() }
{
}

ExtendedAddress::ExtendedAddress(
    otInstance* dev) :
    Memory{ ::otGetExtendedAddress(dev) }
{
    if (res_ == invalid())
    {
        throw ref new OpenThreadTalk::InvalidArgumentException(
            L"EnumDevices::EnumDevices otGetExtendedAddress Failure");
    }
}

int
ExtendedAddress::MacAddressStringA(
    char*  buffer,
    size_t sizeBuffer,
    R      macAddr)
{
    return sprintf_s(buffer, sizeBuffer, MAC8_FORMATA, MAC8_ARG(macAddr));
}

int
ExtendedAddress::MacAddressStringW(
    wchar_t* buffer,
    size_t   sizeBuffer,
    R        macAddr)
{
    return swprintf_s(buffer, sizeBuffer, MAC8_FORMATW, MAC8_ARG(macAddr));
}
