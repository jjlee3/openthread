#include "detail/pch.h"
#include <cctype>
#include <utility>
#include <algorithm>
#include "detail/InterfacesSelectMenu.h"
#include "detail/StdOutput.h"
#include "detail/InterfaceActionMenu.h"
 
InterfacesSelectMenu::InterfacesSelectMenu(
    devices_t&     devices,
    menu_items_t&& interfaces) :
    devices_{devices},
    interfaces_{std::move(interfaces)}
{
}

void
InterfacesSelectMenu::show()
{
    g_out([](std::ostream& os)
    {
        os << "\nSelect one openthread interface and <Enter>:\n";
    });

    for (int i = 0; i < interfaces_.size(); ++i)
    {
        g_out([i, this](std::ostream& os)
        {
            os << (i + 1) << "> " << interfaces_[i].descrition() << std::endl;
        });
    }

    g_out([](std::ostream& os)
    {
        os << "M> Main Menu\n" "Q> Quit\n";
    });
}

// input a number for selecting an interface or
// input "Q" to quit
Menu::menu_csptr
InterfacesSelectMenu::input(
    const std::string& line)
{
    if (line == "M" || line == "m")
    {
        MSTC_THROW_EXCEPTION(MainMenu{});
    }
    else if (line == "Q" || line == "q")
    {
        MSTC_THROW_EXCEPTION(Quit{});
    }

    if (std::find_if(line.begin(), line.end(),
        [](char c) { return !std::isdigit(c); }) != line.end())
    {
        MSTC_THROW_EXCEPTION(Failure{"Invalid Number"});
    }

    auto num = std::atoi(line.c_str());

    if (num < 1)
    {
        MSTC_THROW_EXCEPTION(Failure{"Invalid Range"});
    }
    else if (num > interfaces_.size())
    {
        MSTC_THROW_EXCEPTION(Failure{"Invalid Range"});
    }
    else
    {
        interfaces_[num - 1]();
        return std::make_shared<InterfaceActionMenu>(devices_, num - 1);
    }
}