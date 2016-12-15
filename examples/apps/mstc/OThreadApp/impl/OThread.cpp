#include "detail/pch.h"
#include <stdlib.h>
#include <algorithm>
#include <unordered_map>
#include <tcOtApi/OtApiInstance.h>
#include <tcOtApi/OtEnumDevices.h>
#include <tcOtApi/OtFormat.h>
#include "detail/OThread.h"
#include "detail/StdOutput.h"

OThread::OThread(
    mstc::ot::KApiInstance& kApiInst,
    otStateChangedCallback  f,
    void*                   context) :
    kApiInst_{kApiInst}, thrd_state_change_{f},
    context_{context}
{
}

namespace std
{
template <> struct hash<GUID>
{
    size_t operator ()(const GUID& guid) const noexcept
    {
        const uint64_t* p = reinterpret_cast<const uint64_t*>(&guid);
        std::hash<uint64_t> hash;
        return hash(p[0]) ^ hash(p[1]);
    }
};
}

void
OThread::BuildInterfaceList()
{
    using map_t = std::unordered_map<GUID, device_t>;
    map_t map;
    
    std::for_each(devices_.begin(), devices_.end(),
        [&map](device_t& device)
    {
        map.insert(std::make_pair(device.GetGuid(), std::move(device)));
    });
    devices_.clear();

    mstc::ot::KEnumDevices enumDevices{kApiInst_.get()};

    auto deviceList = enumDevices.get();
    if (!deviceList) { return; }

    auto mie = map.end();

    for (decltype(deviceList->aDevicesLength) i = 0; i < deviceList->aDevicesLength; ++i)
    {
        auto mit = map.find(deviceList->aDevices[i]);
        if (mit != mie)
        {
            devices_.push_back(std::move(mit->second));
        }
        else
        {
            devices_.push_back(CreateNewInterface(deviceList->aDevices[i]));
        }
    }

    BuildMenu();
}

OThread::device_t OThread::CreateNewInterface(
    const GUID& guid)
{
    mstc::ot::KDevice device{kApiInst_.get(), &guid};
    device.SetStateChangeCallback(thrd_state_change_, context_);
    return device;
}

void OThread::BuildMenu()
{
    menu_items_t menuItems;

   for (auto& device : devices_)
    {
        char text[256] = {};
        const auto& guid = device.GetGuid();
        auto len = mstc::ot::KFormat::GuidA(text, _countof(text), guid);
        sprintf_s(&text[len], _countof(text) - len, " %s", device.RoleToStringA());

        menuItems.emplace_back(text, [&device]()
        {
            g_out([&device](std::ostream& os)
            {
                char text[256] = {};
                const auto& guid = device.GetGuid();
                mstc::ot::KFormat::GuidA(text, _countof(text), guid);
                os << " " << text << " selected" << std::endl;
            });
        });
    }

    interfaceMenu_ = std::make_shared<intf_menu_t>(
        devices_, std::move(menuItems));
}

void
OThread::RemoveStateChangeCallback()
{
    for (auto& device : devices_)
    {
        device.RemoveStateChangeCallback(thrd_state_change_, context_);
   }
}