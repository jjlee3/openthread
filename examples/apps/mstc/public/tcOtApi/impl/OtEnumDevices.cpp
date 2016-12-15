#include "pch/pch.h"
#include <tcOtApi/OtApiInstance.h>
#include <tcOtApi/OtEnumDevices.h>

#ifdef OTDLL
mstc::ot::KEnumDevices::KEnumDevices(
    otApiInstance* pApiInstance) :
    KMemory{::otEnumerateDevices(pApiInstance)}
{
}
#endif