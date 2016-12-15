#include "pch/pch.h"
#include <tcOtApi/OtDevice.h>
#include <tcOtApi/OtThreadNet.h>

mstc::ot::KThreadNet::KThreadNet(
    otInstance* pOtDevice) noexcept :
    pOtDevice_{pOtDevice}
{
}

mstc::ot::KThreadNet::KThreadNet(
    KThreadNet&& rhs) noexcept :
    pOtDevice_{std::move(rhs.pOtDevice_)}, start_{std::move(rhs.start_)}
{
    rhs.pOtDevice_ = KDevice::invalid();
    rhs.start_ = false;
}

mstc::ot::KThreadNet::~KThreadNet()
{
    close();
}

mstc::ot::KThreadNet&
mstc::ot::KThreadNet::operator =(
    KThreadNet&& rhs) noexcept
{
    pOtDevice_     = std::move(rhs.pOtDevice_);
    rhs.pOtDevice_ = KDevice::invalid();
    start_         = std::move(rhs.start_);
    rhs.start_ = false;
    return *this;
}

void
mstc::ot::KThreadNet::close() noexcept
{
    Stop();
}

void
mstc::ot::KThreadNet::swap(
    KThreadNet& rhs) noexcept
{
    using std::swap;

    swap(pOtDevice_, rhs.pOtDevice_);
    swap(start_, rhs.start_);
}

ThreadError
mstc::ot::KThreadNet::Start()
{
    auto deviceRole = ::otGetDeviceRole(pOtDevice_);
    auto start = deviceRole != kDeviceRoleDisabled;
    if (start && start_) { return kThreadError_None; }

    auto err = ::otThreadStart(pOtDevice_);
    start_ = (err == kThreadError_None);
    return err;
}

ThreadError
mstc::ot::KThreadNet::Stop()
{
    auto deviceRole = ::otGetDeviceRole(pOtDevice_);
    auto start = deviceRole != kDeviceRoleDisabled;
    if (!start && !start_) { return kThreadError_None; }

    auto err = ::otThreadStop(pOtDevice_);
    start_ = !(err == kThreadError_None);
    return err;
}