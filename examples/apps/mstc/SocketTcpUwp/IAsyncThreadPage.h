#pragma once

namespace SocketTcpUwp
{
    public enum class NotifyType
    {
        Status,
        Error,
    };

    public interface struct IAsyncThreadPage
    {
        void NotifyFromAsyncThread(Platform::String^ message, NotifyType type);
    };
}