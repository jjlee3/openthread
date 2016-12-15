#include "pch.h"
#include "OtDevice.h"
#include "OtMeshLocalEid.h"

ot::KMeshLocalEid::KMeshLocalEid(
    const KDevice& kDevice) :
    KMeshLocalEid{kDevice.get()}
{
}

ot::KMeshLocalEid::KMeshLocalEid(
    otInstance* pDevice) :
    KMemory{::otGetMeshLocalEid(pDevice)}
{
    if (!res_) { throw Exception("KMeshLocalEid::KMeshLocalEid otGetMeshLocalEid Failure"); }
}