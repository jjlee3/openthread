#pragma once

namespace SocketTcpUwp
{
    public enum struct NotifyType
    {
        Status,
        Error,
    };

    public interface struct IAsyncThreadPage
    {
        void NotifyFromAsyncThread(Platform::String^ message, NotifyType type);
    };
}