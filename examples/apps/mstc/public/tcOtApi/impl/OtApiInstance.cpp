#include "pch/pch.h"
#include <utility>
#include <algorithm>
#include <tcOtApi/OtApiInstance.h>

#ifdef OTDLL
mstc::ot::KApiInstance::KApiInstance() :
    res_{::otApiInit()}
{
    if (res_ == invalid())
    {
        MSTC_THROW_EXCEPTION(OtApiInitFailure{});
    }
}

mstc::ot::KApiInstance::KApiInstance(
    KApiInstance&& rhs) noexcept :
    KApiInstance{ std::move(rhs.res_) }
{
}

mstc::ot::KApiInstance::KApiInstance(
    R&& res) noexcept :
    res_{ std::move(res) }
{
    res = invalid();
}

mstc::ot::KApiInstance::~KApiInstance()
{
    close();
}

mstc::ot::KApiInstance&
mstc::ot::KApiInstance::operator =(
    R&& res) noexcept
{
    attach(std::move(res));
    return *this;
}

mstc::ot::KApiInstance&
mstc::ot::KApiInstance::operator =(
    KApiInstance&& rhs) noexcept
{
    if (this == &rhs) { return *this; }

    *this = std::move(rhs.res_);
    rhs.res_ = invalid();
    return *this;
}
#endif // OTDLL

mstc::ot::KApiInstance::R
mstc::ot::KApiInstance::get() const noexcept
{
    return res_;
}

void
mstc::ot::KApiInstance::attach(
    R&& res) noexcept
{
    close();
    res_ = res;
    res = invalid();
    return;
}

mstc::ot::KApiInstance::R
mstc::ot::KApiInstance::detach() noexcept
{
    auto res = res_;
    res_ = invalid();
    return res;
}

void
mstc::ot::KApiInstance::close() noexcept
{
    if (res_ == invalid()) { return; }

#ifdef OTDLL
    std::for_each(multiMap_.begin(), multiMap_.end(),
        [this](const auto& it)
    {
        ::otSetDeviceAvailabilityChangedCallback(res_, nullptr, nullptr);
    });
    multiMap_.clear();

    ::otApiFinalize(res_);
#endif
    res_ = invalid();
}

void
mstc::ot::KApiInstance::swap(
    KApiInstance& rhs) noexcept
{
    using std::swap;

    swap(res_, rhs.res_);
}

void
mstc::ot::KApiInstance::SetDeviceAvailabilityChangedCallback(
    otDeviceAvailabilityChangedCallback callback,
    void*                               context)
{
#ifdef OTDLL
    ::otSetDeviceAvailabilityChangedCallback(res_, callback, context);
    multiMap_.insert(std::make_pair(callback, context));
#endif
}

void
mstc::ot::KApiInstance::RemoveDeviceAvailabilityChangedCallback(
    otDeviceAvailabilityChangedCallback callback,
    void*                               context)
{
#ifdef OTDLL
    auto rng = multiMap_.equal_range(callback);
    for (auto it = rng.first; it != rng.second; ++it)
    {
        if (it->second != context) { continue; }

        multiMap_.erase(it);
        ::otSetDeviceAvailabilityChangedCallback(res_, nullptr, nullptr);
        return;
    }
#endif
}