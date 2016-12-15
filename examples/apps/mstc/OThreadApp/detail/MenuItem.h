#pragma once

#include <string>
#include <functional>

class MenuItem
{
public:
    using action_fun = std::function<void()>;

    MenuItem(std::string strItem, action_fun action);

    const std::string& descrition() const { return strItem_; }
    void operator ()() { action_(); }

protected:
    std::string           strItem_;
    std::function<void()> action_;
};