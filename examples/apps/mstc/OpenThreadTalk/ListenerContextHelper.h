#pragma once

#include "Types.h"
#include "ContextHelper.h"
#include "IAsyncThreadPage.h"

namespace OpenThreadTalk
{
    class ListenerContextHelper : public ContextHelper
    {
    public:
        ListenerContextHelper(IAsyncThreadPage^ page, String^ serverName);

        void Receive(DataReader^, unsigned int strLen, DataWriter^, bool withMsgLen);

        void EchoMessage(DataWriter^, bool withMsgLen, String^ echo);

    protected:
        String^ CreateEchoMessage(String^ msg);

        String^ serverName_;
    };
}
