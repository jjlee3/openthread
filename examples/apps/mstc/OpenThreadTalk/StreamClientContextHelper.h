#pragma once

#include "Types.h"
#include "ClientContextHelper.h"
#include "IAsyncThreadPage.h"

namespace OpenThreadTalk
{
    class StreamClientContextHelper : public ClientContextHelper
    {
    public:
        using base_t = ClientContextHelper;
        using base_t::base_t;

        void ReceiveLoop(StreamSocket^, DataReader^);
    };
}
