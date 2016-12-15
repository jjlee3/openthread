#include "pch/pch.h"
#include <ws2ipdef.h>
#include <mstcpip.h>
#include <stdio.h>
#include <tcOtApi/OtFormat.h>

int
mstc::ot::KFormat::MacW(
    wchar_t*       buffer,
    size_t         cntBuffer,
    const uint8_t* macAddr)
{
    return swprintf_s(buffer, cntBuffer, MAC8_FORMATW, MAC8_ARG(macAddr));
}

int
mstc::ot::KFormat::MacA(
    char*          buffer,
    size_t         cntBuffer,
    const uint8_t* macAddr)
{
    return sprintf_s(buffer, cntBuffer, MAC8_FORMAT, MAC8_ARG(macAddr));
}

int
mstc::ot::KFormat::GuidW(
    wchar_t*    buffer,
    size_t      cntBuffer,
    const GUID& guid)
{
    return swprintf_s(buffer, cntBuffer, GUID_FORMATW, GUID_ARG(guid));
}

int
mstc::ot::KFormat::GuidA(
    char*       buffer,
    size_t      cntBuffer,
    const GUID& guid)
{
    return sprintf_s(buffer, cntBuffer, GUID_FORMAT, GUID_ARG(guid));
}

int
mstc::ot::KFormat::Ipv6W(
    wchar_t*        buffer,
    size_t          sizeBuffer,
    const in6_addr& in6addr)
{
    ::RtlIpv6AddressToStringW(&in6addr, buffer);
    return static_cast<int>(wcslen(buffer));
}

int
mstc::ot::KFormat::Ipv6A(
    char*           buffer,
    size_t          sizeBuffer,
    const in6_addr& in6addr)
{
    ::RtlIpv6AddressToStringA(&in6addr, buffer);
    return static_cast<int>(strlen(buffer));
}