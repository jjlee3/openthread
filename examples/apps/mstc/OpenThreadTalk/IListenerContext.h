#pragma once

#include "Types.h"

namespace OpenThreadTalk
{
    public interface struct IListenerContext
    {
        void Listen_Click(Object^ sender, RoutedEventArgs^ e);
    };
}
