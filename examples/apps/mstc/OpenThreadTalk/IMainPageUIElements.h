#pragma once

#include "Types.h"

namespace OpenThreadTalk
{
    public interface struct IMainPageUIElements
    {
        UIElement^ ThreadGrid();
        UIElement^ TalkGrid();
    };
}
