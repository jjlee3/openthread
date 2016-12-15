#include "pch.h"
#include "OtFormat.h"

int ot::KFormat::Mac(
    wchar_t*     buffer,
    size_t       sizeBuffer,
    const uint8* macAddr)
{
    return swprintf_s(buffer, sizeBuffer, MAC8_FORMAT, MAC8_ARG(macAddr));
}

int ot::KFormat::Guid(
    wchar_t*    buffer,
    size_t      sizeBuffer,
    const GUID& guid)
{
    return swprintf_s(buffer, sizeBuffer, GUID_FORMAT, GUID_ARG(guid));
}

int ot::KFormat::Ipv6(
    wchar_t*        buffer,
    size_t          sizeBuffer,
    const in6_addr& in6addr)
{
    ::RtlIpv6AddressToStringW(&in6addr, buffer);
    return static_cast<int>(wcslen(buffer));
}