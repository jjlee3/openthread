#pragma once

#include "Types.h"
#include "IAsyncThreadPage.h"

namespace OpenThreadTalk
{
    class ContextHelper
    {
    public:
        ContextHelper(IAsyncThreadPage^ page);

        void NotifyFromAsyncThread(String^ message, NotifyType type)
        {
            page_->NotifyFromAsyncThread(message, type);
        }

        String^ ReadString(DataReader^ dataReader);

    protected:
        IAsyncThreadPage^ page_;
    };
}
