#include "detail/pch.h"
#include <memory>
#include "detail/ModeMenu.h"
#include "detail/StdOutput.h"
#include "detail/Options.h"
#include "detail/Listener.h"
#include "detail/IMenuMgr.h"

void
ModeMenu::show()
{
    g_out([](std::ostream& os)
    {
        os << "\nSelect mode and <Enter>:\n";
        os << "S> Server Mode\n" "C> Client Mode\n";
        os << "Q> Quit\n";
    });
}

Menu::menu_csptr
ModeMenu::input(
    const std::string& line)
{
    if (line == "S" || line == "s")
    {
        RunListener();
        MSTC_THROW_EXCEPTION(Stay{});
    }
    if (line == "C" || line == "c")
    {
        MSTC_THROW_EXCEPTION(ClientMode{});
    }
    if (line == "Q" || line == "q")
    {
        MSTC_THROW_EXCEPTION(Quit{});
    }

    MSTC_THROW_EXCEPTION(Failure{"Invalid Input"});
}

void
ModeMenu::RunListener()
{
    auto listener = std::make_shared<Listener>();
    listener->start(g_options);
    g_menuMgr->SetListener(std::move(listener));
}
