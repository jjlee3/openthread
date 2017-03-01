#include "pch.h"
#include "Types.h"
#include "OtDevice.h"
#include "OtMeshLocalEid.h"
#include "Ipv6.h"

using namespace ot;

MeshLocalEid::MeshLocalEid(
    const Device& dev) :
    MeshLocalEid{ dev.get() }
{
}

MeshLocalEid::MeshLocalEid(
    otInstance* dev) :
    Memory{ ::otGetMeshLocalEid(dev) }
{
    if (res_ == invalid())
    {
        throw ref new OpenThreadTalk::InvalidArgumentException(
            L"MeshLocalEid::MeshLocalEid otGetMeshLocalEid Failure");
    }
}

int
MeshLocalEid::Ipv6StringA(
    char*  buffer,
    size_t sizeBuffer,
    R      ipv6)
{
    return OpenThreadTalk::Ipv6::ToStringA(buffer, sizeBuffer,
        *reinterpret_cast<const in6_addr*>(ipv6));
}

int ot::MeshLocalEid::Ipv6StringW(
    wchar_t* buffer,
    size_t   sizeBuffer,
    R        ipv6)
{
    return OpenThreadTalk::Ipv6::ToStringW(buffer, sizeBuffer,
        *reinterpret_cast<const in6_addr*>(ipv6));
}
