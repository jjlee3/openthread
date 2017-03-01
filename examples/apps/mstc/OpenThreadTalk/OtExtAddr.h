#pragma once

#ifndef _OT_EXT_ADDR_H_
#define _OT_EXT_ADDR_H_

#include <OtMemory.h>

#define MAC8_FORMATA "%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X"
#define MAC8_FORMATW L"%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X"
#define MAC8_ARG(mac) mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], mac[6], mac[7]

namespace ot
{
    class Device;
    using extended_addr = const uint8_t;

    class ExtendedAddress : public Memory<extended_addr>
    {
    public:
        using base_t = Memory<extended_addr>;
        // resourec type
        using R = base_t::R;
        using base_t::base_t;

#ifdef OTDLL
        explicit ExtendedAddress(const Device&);
        explicit ExtendedAddress(otInstance*);
#endif // OTDLL

        // format MAC address to string
        static int MacAddressStringA(char* buffer, size_t sizeBuffer, R macAddr);

        // format MAC address to string
        int MacAddressStringA(char* buffer, size_t sizeBuffer)
        {
            return MacAddressStringA(buffer, sizeBuffer, get());
        }

        // format MAC address to string
        static int MacAddressStringW(wchar_t* buffer, size_t sizeBuffer, R macAddr);

        // format MAC address to string
        int MacAddressStringW(wchar_t* buffer, size_t sizeBuffer)
        {
            return MacAddressStringW(buffer, sizeBuffer, get());
        }
    };
}

#endif // _OT_ENUM_DEVICES_H_
