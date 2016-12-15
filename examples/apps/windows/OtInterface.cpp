#include "pch.h"
#include "OtDevice.h"
#include "OtInterface.h"

ot::KInterface::KInterface(
    const KDevice& ckDevice) noexcept :
    ckDevice_(ckDevice)
{
}

ot::KInterface::KInterface(KInterface&& rhs) noexcept :
    ckDevice_{std::move(rhs.ckDevice_)}, up_{std::move(rhs.up_)},
    threadErr_{std::move(rhs.threadErr_)}
{
    rhs.up_ = false;
    rhs.threadErr_ = kThreadError_None;
}

ot::KInterface::~KInterface()
{
    close();
    threadErr_ = kThreadError_None;
}

void
ot::KInterface::close() noexcept
{
    if (!up_) { return; }

    threadErr_ = ::otInterfaceDown(ckDevice_.get());
    up_ = false;
}