#pragma once

#include <vector>
#include <unordered_map>
#include <functional>
#include "Types.h"
#include "IAsyncThreadPage.h"
#include "IThreadDeviceList.h"
#include "InternalThreadDeviceList.h"
#include "GuidHelper.h"
#include "OtApiInstance.h"
#include "OtDevice.h"
#include "OtEnumDevices.h"

namespace OpenThreadTalk
{
    struct ThreadGlobals
    {
    public:
        using Device = ot::Device;
        using device_list_notify = detail::thread_device_list_notify;

        ThreadGlobals(IAsyncThreadPage^, CoreDispatcher^, device_list_notify&&);

        Device* Find(const GUID& guid) const;

        ot::ApiInstance     otApiInstance_;
        std::vector<Device> devices_;

    private:
        using dev_map_t = std::unordered_map<GUID, Device*>;

        static void OTCALL ThreadDeviceAvailabilityCallback(
            bool added, const GUID* devGuid, _In_ void* context);

        static void OTCALL ThreadStateChangeCallback(
            uint32_t flags, _In_ void* context);

        void DeviceChangeCallback();

        void BuildDeviceList();
        void BuildDevice(otApiInstance*, const GUID&);

        IAsyncThreadPage^  page_;
        CoreDispatcher^    dispatcher_;
        device_list_notify threadDevListNotify_;
        dev_map_t          devMap_;
    };

    extern ThreadGlobals* g_pThreadGlobals;
}
