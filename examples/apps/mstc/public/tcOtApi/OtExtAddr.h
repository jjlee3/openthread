#pragma once

#include "openthread.h"
#include <tcOtApi/OtMemory.h>
#include <base/Exception.h>

namespace mstc
{
namespace ot
{
class KDevice;

class KExtendedAddress : public KMemory<const uint8_t>
{
public:
    EXCEPT_BASE   (Failure);
    EXCEPT_DERIVED(OtGetExtendedAddressFailure, Failure);

public:
    using base_t = KMemory<const uint8_t>;
    // resourec type
    using R = base_t::R;
    using base_t::base_t;

    explicit KExtendedAddress(otInstance*);
}; // KExtendedAddress
} // ot
} // mstc