#pragma once

#include "Types.h"

namespace OpenThreadTalk
{
    public enum class NotifyType
    {
        Status,
        Error,
    };

    public interface struct IAsyncThreadPage
    {
        void NotifyFromAsyncThread(String^ message, NotifyType type);
    };
}