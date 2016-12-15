#pragma once

#include <vector>
#include <tcOtApi/OtDevice.h>
#include "detail/Menu.h"

class InterfaceActionMenu : public Menu
{
public:
    using device_t  = mstc::ot::KDevice;
    using devices_t = std::vector<device_t>;

    InterfaceActionMenu(devices_t&, int selected);

    virtual void show() override;
    virtual menu_csptr input(const std::string& inputLine) override;

protected:
    void ConnectNetwork();    
    void DisconnectNetwork();
    void ShowDetails();
    void RunListener();
    void SwitchToClientMode();

    devices_t& devices_;
    int        selected_;
};
