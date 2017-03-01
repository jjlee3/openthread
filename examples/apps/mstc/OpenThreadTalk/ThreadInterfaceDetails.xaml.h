//
// ThreadInterfaceDetails.xaml.h
// Declaration of the ThreadInterfaceDetails class
//

#pragma once

#include "ThreadInterfaceDetails.g.h"
#include "Types.h"
#include "IAsyncThreadPage.h"
#include "OtDevice.h"

namespace OpenThreadTalk
{
	[WFM::WebHostHidden]
	public ref class ThreadInterfaceDetails sealed
	{
	public:
		ThreadInterfaceDetails();

        void Init(IAsyncThreadPage^ page);

        void Show(Guid interfaceGuid);

        // system might rebuild a new device list due to state change
        void BuildDeviceList();

    private:
        IAsyncThreadPage^ page_;
        // current device
        GUID              deviceGuid_;
    };
}
