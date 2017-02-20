#pragma once

#include "Types.h"

namespace SocketUwp
{
    public interface struct IListenerContext
    {
        void Listen_Click(Object^ sender, RoutedEventArgs^ e);
    };
}
