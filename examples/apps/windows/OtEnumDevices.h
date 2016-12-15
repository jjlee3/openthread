#pragma once

#ifndef _OT_ENUM_DEVICES_H_
#define _OT_ENUM_DEVICES_H_

#include "openthread.h"
#include "OtMemory.h"
#include "OtApiInstance.h"

namespace ot
{
    class KEnumDevices : public KMemory<otDeviceList>
    {
    public:
        using base_t = KMemory<otDeviceList>;
        // resourec type
        using R = base_t::R;
        using base_t::base_t;

#ifdef OTDLL
        KEnumDevices(const KApiInstance&);
#endif // OTDLL
    };
}

#endif // _OT_ENUM_DEVICES_H_