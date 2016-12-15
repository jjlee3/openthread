#pragma once

#include <string>
#include <memory>
#include <base/Exception.h>

class Menu;

class IMenu
{
public:
    using menu_t     = Menu;
    using menu_sptr  = std::shared_ptr<menu_t>;
    using menu_csptr = menu_sptr;

    virtual void show() = 0;
    virtual menu_csptr input(const std::string& inputline) = 0;
};

class Menu : public IMenu
{
public:
    // stay the same menu
    EXCEPT_BASE(Stay);
    // Refresh the menu
    EXCEPT_BASE(Refresh);
    EXCEPT_BASE(MainMenu);
    EXCEPT_BASE(Quit);
    EXCEPT_BASE(Failure);
    EXCEPT_BASE(ClientMode);

public:
    virtual ~Menu() = default;
};