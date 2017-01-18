#pragma once

#include "detail/Menu.h"
#include "detail/MenuItem.h"

class ModeMenu : public Menu
{
public:
    virtual void show() override;
    virtual menu_csptr input(const std::string& line) override;

protected:
    void RunListener();
};