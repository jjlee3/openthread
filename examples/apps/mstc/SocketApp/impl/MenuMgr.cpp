#include "detail/pch.h"
#include "detail/MenuMgr.h"
#include "detail/ModeMenu.h"

IMenuMgr* g_menuMgr = nullptr;

MenuMgr::MenuMgr()
{
}

void
MenuMgr::switchToMainMenu()
{
    lock_t lck{mtxMenu_};
    menu_ = std::shared_ptr<Menu>{std::make_shared<ModeMenu>()};
    menu_->show();
}

void
MenuMgr::execute(
    const std::string& line)
{
    lock_t lck{mtxMenu_};
    menu_ = menu_->input(line);
    menu_->show();
}

void
MenuMgr::refresh()
{
    lock_t lck{mtxMenu_};
    menu_->show();
}