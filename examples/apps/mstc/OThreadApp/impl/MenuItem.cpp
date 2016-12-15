#include "detail/pch.h"
#include <utility>
#include "detail/MenuItem.h"

MenuItem::MenuItem(
    std::string strItem,
    action_fun  action) :
    strItem_{std::move(strItem)}, action_{std::move(action)}
{
}