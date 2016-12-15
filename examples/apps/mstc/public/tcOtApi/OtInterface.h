#pragma once

#include "openthread.h"
#include <tcOtApi/OtMemory.h>
#include <tcOtApi/OtDevice.h>

namespace mstc
{
namespace ot
{
class KInterface
{
public:
//  explicit KInterface(otInstance* pOtDevice = KDevice::invalid()) noexcept;
    explicit KInterface(otInstance* pOtDevice = nullptr) noexcept;
    KInterface(KInterface&& rhs) noexcept;
    ~KInterface();

    KInterface& operator =(KInterface&& rhs) noexcept;

    void close() noexcept;

    void swap(KInterface& rhs) noexcept;

    ThreadError Up();
    ThreadError Down();

protected:
    KInterface(const KInterface&) = delete;
    KInterface& operator =(const KInterface&) = delete;

    otInstance* pOtDevice_;
    bool        up_ = false;
}; // KInterface
} // ot
} // mstc