#include "pch/pch.h"
#include <utility>
#include <algorithm>
#include <tcOtApi/OtDevice.h>
#include <tcOtApi/OtExtAddr.h>
#include <tcOtApi/OtMeshLocalEid.h>
#include <tcOtApi/OtFormat.h>

#ifdef OTDLL
mstc::ot::KDevice::KDevice(
    otApiInstance* pApiInstance,
    const GUID*    devGuid) :
    KMemory{::otInstanceInit(pApiInstance, devGuid)},
    interface_{res_}, threadNet_{res_}
{
    if (res_ == invalid())
    {
        MSTC_THROW_EXCEPTION(OtInstanceInitFailure{});
    }

    guid_ = *devGuid;
}

mstc::ot::KDevice::KDevice() :
    KMemory{nullptr}
{
}
#else
#ifdef OPENTHREAD_MULTIPLE_INSTANCE
mstc::ot::KDevice::KDevice(
    void*   instanceBuffer,
    size_t* bufferSize) :
    KMemory{::otInstanceInit(instanceBuffer, bufferSize)}
{
    if (res_ == invalid())
    {
        MSTC_THROW_EXCEPTION(OtInstanceInitFailure{});
    }
}

mstc::ot::KDevice::KDevice() :
    KMemory{nullptr}
{
}
#else
mstc::ot::KDevice::KDevice() :
    KMemory{::otInstanceInit()}
{
    if (res_ == invalid())
    {
        MSTC_THROW_EXCEPTION(OtInstanceInitFailure{});
    }
}
#endif // OPENTHREAD_MULTIPLE_INSTANCE
#endif // OTDLL

mstc::ot::KDevice::KDevice(
    KDevice&& rhs) noexcept :
    base_t{std::move(rhs)}, multiMap_{std::move(rhs.multiMap_)},
    interface_{std::move(rhs.interface_)},
    threadNet_{std::move(rhs.threadNet_)}
{
    guid_ = std::move(rhs.guid_);
    rhs.guid_ = GUID_NULL;
}

mstc::ot::KDevice::KDevice(
    R&& res) noexcept :
    base_t{std::move(res)}
{
}

mstc::ot::KDevice::~KDevice()
{
    close();
}

mstc::ot::KDevice&
mstc::ot::KDevice::operator =(
    R&& res) noexcept
{
    attach(std::move(res));
    return *this;
}

mstc::ot::KDevice&
mstc::ot::KDevice::operator =(
    KDevice&& rhs) noexcept
{
    if (this == &rhs) { return *this; }

    *static_cast<base_t*>(this) = std::move(*static_cast<base_t*>(&rhs));
    guid_ = std::move(rhs.guid_);
    rhs.guid_ = GUID_NULL;
    multiMap_ = std::move(rhs.multiMap_);
    interface_ = std::move(rhs.interface_);
    threadNet_ = std::move(rhs.threadNet_);
    return *this;
}

void
mstc::ot::KDevice::attach(R&& res) noexcept
{
    close();
    base_t::attach(std::move(res));
}

mstc::ot::KDevice::R
mstc::ot::KDevice::detach() noexcept
{
    auto res = base_t::detach();
    multiMap_.clear();
    return res;
}

void
mstc::ot::KDevice::close() noexcept
{
    if (!res_) { return; }

    std::for_each(multiMap_.begin(), multiMap_.end(),
        [this](const auto& it)
    {
        ::otRemoveStateChangeCallback(res_, it.first, it.second);
    });
    guid_ = GUID_NULL;
    multiMap_.clear();
    interface_.close();
    threadNet_.close();

#ifndef OTDLL
    ::otInstanceFinalize(res_);
#endif
    base_t::close();
}

const GUID&
mstc::ot::KDevice::GetGuid() const
{
    return guid_;
}

GUID
mstc::ot::KDevice::GetDeviceGuid() const
{
#ifdef OTDLL
    return ::otGetDeviceGuid(res_);
#else
    return guid_;
#endif
}

ThreadError
mstc::ot::KDevice::SetStateChangeCallback(
    otStateChangedCallback callback,
    void*                  context)
{
    auto ret = ::otSetStateChangedCallback(res_, callback, context);
    multiMap_.insert(std::make_pair(callback, context));
    return ret;
}

void
mstc::ot::KDevice::RemoveStateChangeCallback(
    otStateChangedCallback callback,
    void*                  context)
{
    auto rng = multiMap_.equal_range(callback);
    for (auto it = rng.first; it != rng.second; ++it)
    {
        if (it->second != context) { continue; }

        multiMap_.erase(it);
        ::otRemoveStateChangeCallback(res_, callback, context);
        return;
    }
}

const wchar_t*
mstc::ot::KDevice::RoleToStringW() const
{
    auto role = ::otGetDeviceRole(res_);
    
    switch (role)
    {
    case kDeviceRoleOffline:    return L"Offline";
    case kDeviceRoleDisabled:   return L"Disabled";
    case kDeviceRoleDetached:   return L"Disconnected";
    case kDeviceRoleChild:      return L"Connected - Child";
    case kDeviceRoleRouter:     return L"Connected - Router";
    case kDeviceRoleLeader:     return L"Connected - Leader";
    }

    return L"Unknown Role State";
}

const char*
mstc::ot::KDevice::RoleToStringA() const
{
    auto role = ::otGetDeviceRole(res_);
    
    switch (role)
    {
    case kDeviceRoleOffline:    return "Offline";
    case kDeviceRoleDisabled:   return "Disabled";
    case kDeviceRoleDetached:   return "Disconnected";
    case kDeviceRoleChild:      return "Connected - Child";
    case kDeviceRoleRouter:     return "Connected - Router";
    case kDeviceRoleLeader:     return "Connected - Leader";
    }

    return "Unknown Role State";
}

int
mstc::ot::KDevice::MacFormatW(
    wchar_t* buffer,
    size_t   cntBuffer) const
{
    KExtendedAddress macAddr{get()};
    return KFormat::MacW(buffer, cntBuffer, macAddr.get());
}

int
mstc::ot::KDevice::MacFormatA(
    char*  buffer,
    size_t cntBuffer) const
{
    KExtendedAddress macAddr{get()};
    return KFormat::MacA(buffer, cntBuffer, macAddr.get());
}

int
mstc::ot::KDevice::Ipv6FormatW(
    wchar_t* buffer,
    size_t   cntBuffer) const
{
    ot::KMeshLocalEid ipv6{get()};
    return KFormat::Ipv6W(buffer, cntBuffer,
        *reinterpret_cast<const in6_addr*>(ipv6.get()));
}

int
mstc::ot::KDevice::Ipv6FormatA(
    char*  buffer,
    size_t cntBuffer) const
{
    ot::KMeshLocalEid ipv6{get()};
    return KFormat::Ipv6A(buffer, cntBuffer,
        *reinterpret_cast<const in6_addr*>(ipv6.get()));
}

ThreadError
mstc::ot::KDevice::InterfaceUp()
{
    return interface_.Up();
}

ThreadError
mstc::ot::KDevice::InterfaceDown()
{
    return interface_.Down();
}

ThreadError
mstc::ot::KDevice::ThreadStart()
{
    return threadNet_.Start();
}

ThreadError
mstc::ot::KDevice::ThreadStop()
{
    return threadNet_.Stop();
}