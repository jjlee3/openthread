#pragma once

#ifndef _OT_FORMAT_H_
#define _OT_FORMAT_H_

#define GUID_FORMAT L"{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}"
#define GUID_ARG(guid) guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]

#define MAC8_FORMAT L"%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X"
#define MAC8_ARG(mac) mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], mac[6], mac[7]

namespace ot
{
    class KFormat
    {
    public:
        static int Mac(wchar_t* buffer, size_t sizeBuffer, const uint8* macAddr);
        static int Guid(wchar_t* buffer, size_t sizeBuffer, const GUID&);
        static int Ipv6(wchar_t* buffer, size_t sizeBuffer, const in6_addr&);
    };
}

#endif // _OT_FORMAT_H_