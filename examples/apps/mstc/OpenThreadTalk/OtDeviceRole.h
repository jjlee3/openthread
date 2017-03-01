#pragma once

#ifndef _OT_DEVICE_ROLE_H_
#define _OT_DEVICE_ROLE_H_

#include <map>

namespace ot
{
    class DeviceRole
    {
    public:
        static const wchar_t* ToStringW(otDeviceRole devRole);
        static const char* ToStringA(otDeviceRole devRole);
    };
}

#endif // _OT_DEVICE_ROLE_H_
