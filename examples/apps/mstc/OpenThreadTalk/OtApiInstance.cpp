#include "pch.h"
#include "Types.h"
#include "OtApiInstance.h"

using namespace ot;

#ifdef OTDLL
ApiInstance::ApiInstance() :
    res_{ ::otApiInit() }
{
    if (res_ == invalid())
    {
        throw ref new OpenThreadTalk::FailureException(
            L"ApiInstance::ApiInstance otApiInit Failure");
    }
}
#endif // OTDLL

ApiInstance::ApiInstance(
    ApiInstance&& rhs) noexcept :
    ApiInstance{ std::move(rhs.res_) }
{
}

ApiInstance::ApiInstance(
    R&& res) noexcept :
    res_{ std::move(res) }
{
    res = invalid();
}

ApiInstance::~ApiInstance()
{
    close();
}

ApiInstance&
ApiInstance::operator =(
    R&& res) noexcept
{
    attach(std::move(res));
    return *this;
}

ApiInstance&
ApiInstance::operator =(
    ApiInstance&& rhs) noexcept
{
    if (this == &rhs) { return *this; }

    *this = std::move(rhs.res_);
    rhs.res_ = invalid();
    return *this;
}

ApiInstance::R
ApiInstance::get() const noexcept
{
    return res_;
}

void
ApiInstance::attach(
    R&& res) noexcept
{
    close();
    res_ = res;
    res = invalid();
    return;
}

ApiInstance::R
ApiInstance::detach() noexcept
{
    auto res = res_;
    res_ = invalid();
    return res;
}

void
ApiInstance::close() noexcept
{
    if (res_ == invalid()) { return; }

#ifdef OTDLL
    std::for_each(multiMap_.begin(), multiMap_.end(),
        [this](const auto& e)
    {
        ::otSetDeviceAvailabilityChangedCallback(res_, e.first, nullptr);
    });
    multiMap_.clear();

    ::otApiFinalize(res_);
#endif
    res_ = invalid();
}

void
ApiInstance::swap(
    ApiInstance& rhs) noexcept
{
    using std::swap;

    swap(res_, rhs.res_);
}

void
ApiInstance::SetDeviceAvailabilityChangedCallback(
    otDeviceAvailabilityChangedCallback callback,
    void*                               context)
{
#ifdef OTDLL
    ::otSetDeviceAvailabilityChangedCallback(res_, callback, context);
    multiMap_.insert(std::make_pair(callback, context));
#endif
}

void
ApiInstance::RemoveDeviceAvailabilityChangedCallback(
    otDeviceAvailabilityChangedCallback callback,
    void*                               context)
{
#ifdef OTDLL
    auto rng = multiMap_.equal_range(callback);
    for (auto it = rng.first; it != rng.second; ++it)
    {
        if (it->second != context) { continue; }

        ::otSetDeviceAvailabilityChangedCallback(res_, it->first, nullptr);
        multiMap_.erase(it);
        return;
    }
#endif
}
