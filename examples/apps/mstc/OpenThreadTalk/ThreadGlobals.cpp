#include "pch.h"
#include <utility>
#include "ThreadGlobals.h"

using namespace OpenThreadTalk;

ThreadGlobals* OpenThreadTalk::g_pThreadGlobals = nullptr;

ThreadGlobals::ThreadGlobals(
    IAsyncThreadPage^    page,
    CoreDispatcher^      dispatcher,
    device_list_notify&& threadDevListNotify) :
    page_{ std::move(page) },
    dispatcher_{ std::move(dispatcher) },
    threadDevListNotify_{ std::move(threadDevListNotify) }
{
    otApiInstance_.SetDeviceAvailabilityChangedCallback(
        ThreadDeviceAvailabilityCallback, this);

    // Build the intial list
    DeviceChangeCallback();
}

ot::Device*
ThreadGlobals::Find(
    const GUID & guid) const
{
    auto it = devMap_.find(guid);
    if (it == devMap_.end()) { return nullptr; }

    return it->second;
}

void OTCALL
ThreadGlobals::ThreadDeviceAvailabilityCallback(
    bool        /*added*/,
    const GUID* /*devGuid*/,
    void*       context)
{
    reinterpret_cast<ThreadGlobals*>(context)->DeviceChangeCallback();
}

void OTCALL
ThreadGlobals::ThreadStateChangeCallback(
    uint32_t flags,
    void*    context)
{
    if ((flags & OT_NET_ROLE) == 0) { return; }

    reinterpret_cast<ThreadGlobals*>(context)->DeviceChangeCallback();
}

void
ThreadGlobals::DeviceChangeCallback()
{
    dispatcher_->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this]()
    {
        BuildDeviceList();
    }));
}

void
ThreadGlobals::BuildDeviceList()
{
    auto otApiInst = otApiInstance_.get();
    if (otApiInst == ot::ApiInstance::invalid()) { return; }

    devMap_.clear();
    devices_.clear();
    ot::EnumDevices enumDevices{ otApiInst };
    auto deviceList = enumDevices.get();

    for (decltype(deviceList->aDevicesLength) index = 0;
        index < deviceList->aDevicesLength; ++index)
    {
        try
        {
            BuildDevice(otApiInst, deviceList->aDevices[index]);
        }
        catch (Exception^ ex)
        {
            page_->NotifyFromAsyncThread(
                "BuildDevice error: " + ex->Message, NotifyType::Error);
        }
    }

    // build device map
    for (auto& device : devices_)
    {
        devMap_.emplace(device.GetGuid(), &device);
    }

    if (threadDevListNotify_)
    {
        threadDevListNotify_();
    }
}

void
ThreadGlobals::BuildDevice(
    otApiInstance* otApiInst,
    const GUID&    guid)
{
    Device device{ otApiInst, guid };
    device.SetStateChangeCallback(ThreadStateChangeCallback,
        const_cast<void*>(reinterpret_cast<const void*>(this)));
    devices_.push_back(std::move(device));
}
