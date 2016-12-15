#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <tcOtApi/OtApiInstance.h>
#include <tcOtApi/OtDevice.h>
#include "detail/MenuItem.h"
#include "detail/InterfacesSelectMenu.h"

struct Options;

// for OpenThread
class OThread
{
    using otDeviceAvailabilityChangedCallback =
        mstc::ot::KApiInstance::otDeviceAvailabilityChangedCallback;
public:
    using intf_menu_t     = InterfacesSelectMenu;
    using intf_menu_sptr  = std::shared_ptr<intf_menu_t>;
    using intf_menu_csptr = intf_menu_sptr;
    using menu_csptr      = Menu::menu_csptr;

    explicit OThread(mstc::ot::KApiInstance&,
        otStateChangedCallback, void*);

    void BuildInterfaceList();

    void RemoveStateChangeCallback();

    menu_csptr Menu() const { return interfaceMenu_; }

protected:
    using device_t             = mstc::ot::KDevice;
    using devices_t            = std::vector<device_t>;
    using devs_descs_t         = std::vector<std::string>;
    using menu_item_t          = MenuItem;
    using menu_items_t         = std::vector<menu_item_t>;

    device_t CreateNewInterface(const GUID&);
    
    void BuildMenu();

    mstc::ot::KApiInstance& kApiInst_;
    otStateChangedCallback  thrd_state_change_;
    void*                   context_;

    devices_t               devices_;
    menu_csptr              interfaceMenu_;
};