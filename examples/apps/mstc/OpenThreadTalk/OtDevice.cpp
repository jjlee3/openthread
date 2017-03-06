#include "pch.h"
#include "Types.h"
#include "OtApiInstance.h"
#include "OtDevice.h"

using namespace ot;

#ifdef OTDLL
Device::Device(
    const ApiInstance& apiInstance,
    const GUID&        devGuid) :
    Device{ apiInstance.get(), devGuid }
{
}

Device::Device(
    otApiInstance* pApiInstance,
    const GUID&    devGuid) :
    base_t{ ::otInstanceInit(pApiInstance, &devGuid) }
{
    if (res_ == invalid())
    {
        throw ref new OpenThreadTalk::InvalidArgumentException(
            L"Device::Device otInstanceInit Failure");
    }
}
#endif // OTDLL

Device::~Device()
{
    close();
}

void
Device::close() noexcept
{
    if (res_ == invalid()) { return; }

    std::for_each(multiMap_.begin(), multiMap_.end(),
        [this](const auto& e)
    {
        ::otRemoveStateChangeCallback(res_, e.first, e.second);
    });
    multiMap_.clear();

#ifndef OTDLL
    ::otInstanceFinalize(res_);
#endif
    base_t::close();
}

GUID
Device::GetGuid() const
{
    return ::otGetDeviceGuid(res_);
}

otDeviceRole
Device::GetRole() const
{
    return ::otGetDeviceRole(res_);
}

uint16_t
Device::GetRloc16() const
{
    return ::otGetRloc16(res_);
}

ThreadError
Device::GetChildInfoByIndex(
    uint8_t      index,
    otChildInfo* childInfo) const
{
    return ::otGetChildInfoByIndex(res_, index, childInfo);
}

ThreadError
Device::SetStateChangeCallback(
    otStateChangedCallback callback,
    void*                  context)
{
    auto ret = ::otSetStateChangedCallback(res_, callback, context);
    multiMap_.insert(std::make_pair(callback, context));
    return ret;
}

void
Device::RemoveStateChangeCallback(
    otStateChangedCallback callback,
    void*                  context)
{
    auto rng = multiMap_.equal_range(callback);
    for (auto it = rng.first; it != rng.second; ++it)
    {
        if (it->second != context) { continue; }

        ::otRemoveStateChangeCallback(res_, callback, context);
        multiMap_.erase(it);
        return;
    }
}

ThreadError
Device::SetNetworkName(
    const char* networkName)
{
    return ::otSetNetworkName(res_, networkName);
}

ThreadError
Device::SetMasterKey(
    const uint8_t* masterKey,
    uint8_t        keyLength)
{
    return ::otSetMasterKey(res_, masterKey, keyLength);
}

ThreadError
Device::SetChannel(
    uint8_t channel)
{
    return ::otSetChannel(res_, channel);
}

ThreadError
Device::SetMaxAllowChildren(
    uint8_t maxChildren)
{
    return ::otSetMaxAllowedChildren(res_, maxChildren);
}

ThreadError
Device::SetPanId(
    otPanId panId)
{
    return ::otSetPanId(res_, panId);
}

void
Device::ConnectNetwork()
{
    ::otInterfaceUp(res_);
    ::otThreadStart(res_);
}

void
Device::DisconnectNetwork()
{
    ::otThreadStop(res_);
    ::otInterfaceDown(res_);
}
