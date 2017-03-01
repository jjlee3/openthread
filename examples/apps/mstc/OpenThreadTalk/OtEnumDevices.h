#pragma once

#ifndef _OT_ENUM_DEVICES_H_
#define _OT_ENUM_DEVICES_H_

#include <OtMemory.h>

namespace ot
{
    class ApiInstance;

    class EnumDevices : public Memory<otDeviceList>
    {
    public:
        using base_t = Memory<otDeviceList>;
        // resourec type
        using R = base_t::R;
        using base_t::base_t;

#ifdef OTDLL
        explicit EnumDevices(const ApiInstance&);
        explicit EnumDevices(otApiInstance*);
#endif // OTDLL
    };
}

#endif // _OT_ENUM_DEVICES_H_
