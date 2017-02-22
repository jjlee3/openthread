#pragma once

#include "Types.h"
#include "ListenerContextHelper.h"
#include "IAsyncThreadPage.h"

namespace OpenThreadTalk
{
    class StreamListenerContextHelper : public ListenerContextHelper
    {
    public:
        using base_t = ListenerContextHelper;
        using base_t::base_t;

        void ReceiveLoop(StreamSocket^, DataReader^, DataWriter^);
    };
}
