#pragma once

#include "openthread.h"
#include <tcOtApi/OtMemory.h>

namespace mstc
{
namespace ot
{
class KApiInstance;

class KEnumDevices : public KMemory<otDeviceList>
{
public:
    using base_t = KMemory<otDeviceList>;
    // resourec type
    using R = base_t::R;
    using base_t::base_t;

#ifdef OTDLL
    KEnumDevices(otApiInstance*);
#endif // OTDLL
}; // KEnumDevices
} // ot
} // mstc