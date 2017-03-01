#pragma once

#include "ThreadInterfaceConfiguration.xaml.h"
#include "ThreadInterfaceDetails.xaml.h"

namespace OpenThreadTalk
{
    public interface struct IInterfaceUIElements
    {
        ThreadInterfaceConfiguration^ ThreadInterfaceConfiguration();
        ThreadInterfaceDetails^ ThreadInterfaceDetails();
    };
}
