#include "pch/pch.h"
#include <tcOtApi/OtDevice.h>
#include <tcOtApi/OtInterface.h>

mstc::ot::KInterface::KInterface(
    otInstance* pOtDevice) noexcept :
    pOtDevice_{pOtDevice}
{
}

mstc::ot::KInterface::KInterface(
    KInterface&& rhs) noexcept :
    pOtDevice_{std::move(rhs.pOtDevice_)}, up_{std::move(rhs.up_)}
{
    rhs.pOtDevice_ = KDevice::invalid();
    rhs.up_ = false;
}

mstc::ot::KInterface::~KInterface()
{
    close();
}

mstc::ot::KInterface&
mstc::ot::KInterface::operator =(
    KInterface&& rhs) noexcept
{
    pOtDevice_     = std::move(rhs.pOtDevice_);
    rhs.pOtDevice_ = KDevice::invalid();
    up_            = std::move(rhs.up_);
    rhs.up_        = false;
    return *this;
}

void
mstc::ot::KInterface::close() noexcept
{
    if (!::otIsInterfaceUp(pOtDevice_)) { return; }

    Down();
}

void
mstc::ot::KInterface::swap(
    KInterface& rhs) noexcept
{
    using std::swap;

    swap(pOtDevice_, rhs.pOtDevice_);
    swap(up_, rhs.up_);
}

ThreadError
mstc::ot::KInterface::Up()
{
    auto up = ::otIsInterfaceUp(pOtDevice_);
    if (up && up_) { return kThreadError_None; }

    auto err = ::otInterfaceUp(pOtDevice_);
    up_ = (err == kThreadError_None);
    return err;
}

ThreadError
mstc::ot::KInterface::Down()
{
    auto up = ::otIsInterfaceUp(pOtDevice_);
    if (!up && !up_) { return kThreadError_None; }

    auto err = ::otInterfaceDown(pOtDevice_);
    up_ = !(err == kThreadError_None);
    return err;
}