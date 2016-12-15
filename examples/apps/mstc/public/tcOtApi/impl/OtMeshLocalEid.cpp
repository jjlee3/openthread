#include "pch/pch.h"
#include <tcOtApi/OtDevice.h>
#include <tcOtApi/OtMeshLocalEid.h>

mstc::ot::KMeshLocalEid::KMeshLocalEid(
    otInstance* pDevice) :
    KMemory{::otGetMeshLocalEid(pDevice)}
{
    if (!res_)
    {
        MSTC_THROW_EXCEPTION(OtGetMeshLocalEidFailure{});
    }
}