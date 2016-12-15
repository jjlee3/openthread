#include "pch.h"
#include "OtEnumDevices.h"

#ifdef OTDLL
ot::KEnumDevices::KEnumDevices(
    const KApiInstance& kApiInstance) :
    KMemory{::otEnumerateDevices(kApiInstance.get())}
{
}
#endif