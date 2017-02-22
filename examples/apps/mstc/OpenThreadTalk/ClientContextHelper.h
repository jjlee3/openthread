#pragma once

#include "Types.h"
#include "ContextHelper.h"
#include "IAsyncThreadPage.h"

namespace OpenThreadTalk
{
    class ClientContextHelper : public ContextHelper
    {
    public:
        ClientContextHelper(IAsyncThreadPage^ page);

        void SendMessage(DataWriter^ dataWriter, bool withMsgLen, String^ msg);

        void SetConnected(bool connected) { connected_ = connected; }
        bool IsConnected() { return connected_; }

        void Receive(DataReader^, unsigned int strLen);

    protected:
        bool connected_ = false;
    };
}
