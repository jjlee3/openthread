#pragma once

#ifndef _OT_MESH_LOCAL_EID_H_
#define _OT_MESH_LOCAL_EID_H_

#include "OtMemory.h"

namespace ot
{
    class Device;

    class MeshLocalEid : public Memory<const otIp6Address>
    {
    public:
        using base_t = Memory<const otIp6Address>;
        // resourec type
        using R = base_t::R;
        using base_t::base_t;

        explicit MeshLocalEid(const Device&);
        explicit MeshLocalEid(otInstance*);

        // format IPv6 address to string
        static int Ipv6StringA(char* buffer, size_t sizeBuffer, R ipv6);

        // format IPv6 address to string
        int Ipv6StringA(char* buffer, size_t sizeBuffer)
        {
            return Ipv6StringA(buffer, sizeBuffer, get());
        }

        // format IPv6 address to string
        static int Ipv6StringW(wchar_t* buffer, size_t sizeBuffer, R ipv6);

        // format IPv6 address to string
        int Ipv6StringW(wchar_t* buffer, size_t sizeBuffer)
        {
            return Ipv6StringW(buffer, sizeBuffer, get());
        }
    };
}

#endif // _OT_MESH_LOCAL_EID_H_
