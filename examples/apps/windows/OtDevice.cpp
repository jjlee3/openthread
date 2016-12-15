#include "pch.h"
#include <algorithm>
#include <utility>
#include "OtApiInstance.h"
#include "OtDevice.h"
#include "OtExtAddr.h"
#include "OtMeshLocalEid.h"
#include "OtFormat.h"

#ifdef OTDLL
ot::KDevice::KDevice(
    const KApiInstance& kApiInstance,
    const GUID*         devGuid) :
    KDevice{kApiInstance.get(), devGuid}
{
}

ot::KDevice::KDevice(
    otApiInstance* pApiInstance,
    const GUID*    devGuid) :
    KMemory{::otInstanceInit(pApiInstance, devGuid)}
{
    if (!res_) { throw Exception("KDevice::KDevice otInstanceInit (device) Failure"); }
}
#else
#ifdef OPENTHREAD_MULTIPLE_INSTANCE
ot::KDevice::KDevice(
    void*   instanceBuffer,
    size_t* bufferSize) :
    KMemory{::otInstanceInit(instanceBuffer, bufferSize)}
{
}
#else
ot::KDevice::KDevice() :
    KMemory{::otInstanceInit()}
{
}
#endif // OPENTHREAD_MULTIPLE_INSTANCE
#endif // OTDLL

ot::KDevice::~KDevice()
{
    close();
}

void
ot::KDevice::close() noexcept
{
    if (!res_) { return; }

    std::for_each(multiMap_.begin(), multiMap_.end(),
        [this](const auto& it)
    {
        ::otRemoveStateChangeCallback(res_, it.first, it.second);
    });
    multiMap_.clear();

#ifndef OTDLL
    ::otInstanceFinalize(res_);
#endif
    base_t::close();
}

GUID
ot::KDevice::GetDeviceGuid() const
{
    return ::otGetDeviceGuid(res_);
}

ThreadError
ot::KDevice::SetStateChangeCallback(
    otStateChangedCallback callback,
    void*                  context)
{
    auto ret = ::otSetStateChangedCallback(res_, callback, context);
    multiMap_.insert(std::make_pair(callback, context));
    return ret;
}

void
ot::KDevice::RemoveStateChangeCallback(
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

const wchar_t* ot::KDevice::RoleToString() const
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

int ot::KDevice::MacFormat(
    wchar_t* buffer,
    size_t   sizeBuffer) const
{
    KExtendedAddress macAddr{*this};
    return KFormat::Mac(buffer, sizeBuffer, macAddr.get());
}

int ot::KDevice::Ipv6Format(
    wchar_t* buffer,
    size_t   sizeBuffer) const
{
    ot::KMeshLocalEid ipv6{*this};
    return KFormat::Ipv6(buffer, sizeBuffer,
        *reinterpret_cast<const in6_addr*>(ipv6.get()));
}