#pragma once

#include "openthread.h"

namespace mstc
{
namespace ot
{
class KThreadNet
{
public:
//  explicit KThreadNet(otInstance* pOtDevice = KDevice::invalid()) noexcept;
    explicit KThreadNet(otInstance* pOtDevice = nullptr) noexcept;
    KThreadNet(KThreadNet&& rhs) noexcept;
    ~KThreadNet();

    KThreadNet& operator =(KThreadNet&& rhs) noexcept;

    void close() noexcept;

    void swap(KThreadNet& rhs) noexcept;

    ThreadError Start();
    ThreadError Stop();

protected:
    KThreadNet(const KThreadNet&) = delete;
    KThreadNet& operator =(const KThreadNet&) = delete;

    otInstance* pOtDevice_;
    bool        start_ = false;
}; // KThreadNet
} // ot
} // mstc