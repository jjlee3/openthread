#pragma once

#include "Types.h"

namespace SocketUwp
{
    public enum struct NotifyType
    {
        Status,
        Error,
    };

    public interface struct IAsyncThreadPage
    {
        void NotifyFromAsyncThread(String^ message, NotifyType type);
    };
}
