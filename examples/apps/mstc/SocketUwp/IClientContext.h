#pragma once

#include "Types.h"

namespace SocketUwp
{
    public interface struct IClientContext
    {
        void Connect_Click(Object^ sender, RoutedEventArgs^ e);

        void Send_Click(Object^ sender, RoutedEventArgs^ e, String^ input);
    };
}
