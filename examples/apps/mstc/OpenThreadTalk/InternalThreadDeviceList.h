#pragma once

#include <functional>

namespace OpenThreadTalk
{
    namespace detail // private usage
    {
        // a callback provided by an internal subscriber
        // to notify a new device list is ready
        // this establish a communication channel for non WinRT classes
        using thread_device_list_notify = std::function<void()>;
    }
}
