#pragma once

#include <mutex>
#include "detail/IMenuMgr.h"
#include "detail/Menu.h"

class MenuMgr : public IMenuMgr
{
public:
    MenuMgr();

    void switchToMainMenu();

    void execute(const std::string& line);

    void refresh();

    void SetListener(listener_csptr&& listener) override
    {
        listener_ = std::move(listener);
    }

protected:
    using mutex_t    = std::mutex;
    using lock_t     = std::lock_guard<mutex_t>;
    using menu_csptr = Menu::menu_csptr;

    mutable mutex_t mtxMenu_;
    menu_csptr      menu_;
    listener_sptr   listener_;
};