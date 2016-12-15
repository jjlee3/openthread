#include "pch.h"
#include <utility>
#include "OtApiInstance.h"

#ifdef OTDLL
ot::KApiInstance::KApiInstance() :
    res_{::otApiInit()}
{
    if (res_ == invalid())
    {
        throw Exception("KApiInstance::KApiInstance otApiInit Failure");
    }
}
#endif

ot::KApiInstance::KApiInstance(KApiInstance&& rhs) noexcept :
    KApiInstance{std::move(rhs.res_)}
{
}

ot::KApiInstance::KApiInstance(R&& res) noexcept :
    res_{std::move(res)}
{
    res = invalid();
}

ot::KApiInstance::~KApiInstance()
{
    close();
}

ot::KApiInstance&
ot::KApiInstance::operator =(
    R&& res) noexcept
{
    attach(std::move(res));
    return *this;
}

ot::KApiInstance&
ot::KApiInstance::operator =(
    KApiInstance&& rhs) noexcept
{
    if (this == &rhs) { return *this; }

    *this = std::move(rhs.res_);
    rhs.res_ = invalid();
    return *this;
}

ot::KApiInstance::R
ot::KApiInstance::get() const noexcept
{
    return res_;
}

void
ot::KApiInstance::attach(
    R&& res) noexcept
{
    close();
    res_ = res;
    res = invalid();
    return;
}

ot::KApiInstance::R
ot::KApiInstance::detach() noexcept
{
    auto res = res_;
    res_ = invalid();
    return res;
}

void
ot::KApiInstance::close() noexcept
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
ot::KApiInstance::swap(
    KApiInstance& rhs) noexcept
{
    using std::swap;

    swap(res_, rhs.res_);
}

void
ot::KApiInstance::SetDeviceAvailabilityChangedCallback(
    otDeviceAvailabilityChangedCallback callback,
    void*                               context)
{
#ifdef OTDLL
    ::otSetDeviceAvailabilityChangedCallback(res_, callback, context);
    multiMap_.insert(std::make_pair(callback, context));
#endif
}

void
ot::KApiInstance::RemoveDeviceAvailabilityChangedCallback(
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