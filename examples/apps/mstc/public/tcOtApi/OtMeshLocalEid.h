#pragma once

#include "openthread.h"
#include <tcOtApi/OtMemory.h>
#include <base/Exception.h>

namespace mstc
{
namespace ot
{
class KDevice;

class KMeshLocalEid : public KMemory<const otIp6Address>
{
public:
    EXCEPT_BASE   (Failure);
    EXCEPT_DERIVED(OtGetMeshLocalEidFailure, Failure);

public:
    using base_t = KMemory<const otIp6Address>;
    // resourec type
    using R = base_t::R;
    using base_t::base_t;

    explicit KMeshLocalEid(otInstance*);
}; // KMeshLocalEid
} // ot
} // mstc