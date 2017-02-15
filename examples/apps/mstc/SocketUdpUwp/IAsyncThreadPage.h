#pragma once

#include "Types.h"

namespace SocketUdpUwp
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