#pragma once

#include <in6addr.h>
#include <winsock2.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <inttypes.h>
#include <base/StringMacros.h>

namespace mstc
{
namespace ot
{
#define GUID_FORMAT "{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}"
#define GUID_FORMATW    MACRO_WIDEN_STR(GUID_FORMAT)
#define GUID_ARG(guid) guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]

#define MAC8_FORMAT "%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X"
#define MAC8_FORMATW    MACRO_WIDEN_STR(MAC8_FORMAT)
#define MAC8_ARG(mac) mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], mac[6], mac[7]

class KFormat
{
public:
    static int MacW(wchar_t* buffer, size_t cntBuffer, const uint8_t* macAddr);
    static int MacA(char* buffer, size_t cntBuffer, const uint8_t* macAddr);
    static int GuidW(wchar_t* buffer, size_t cntBuffer, const GUID&);
    static int GuidA(char* buffer, size_t cntBuffer, const GUID&);
    static int Ipv6W(wchar_t* buffer, size_t cntBuffer, const in6_addr&);
    static int Ipv6A(char* buffer, size_t cntBuffer, const in6_addr&);
}; // KFormat
} // ot
} // mstc