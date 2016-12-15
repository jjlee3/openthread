#pragma once

#include <vector>
#include <tcOtApi/OtDevice.h>
#include "detail/Menu.h"
#include "detail/MenuItem.h"

class OThread;

class InterfacesSelectMenu : public Menu
{
public:
    using device_t     = mstc::ot::KDevice;
    using devices_t    = std::vector<device_t>;
    using menu_item_t  = MenuItem;
    using menu_items_t = std::vector<menu_item_t>;

    InterfacesSelectMenu(devices_t&, menu_items_t&& interfaces);

    virtual void show() override;
    virtual menu_csptr input(const std::string& inputLine) override;

protected:
    devices_t&   devices_;
    menu_items_t interfaces_;
};