//
// ThreadInterfaceConfig.xaml.h
// Declaration of the ThreadInterfaceConfiguration class
//

#pragma once

#include "ThreadInterfaceConfiguration.g.h"
#include "Types.h"
#include "IAsyncThreadPage.h"
#include "OtDevice.h"

namespace OpenThreadTalk
{
	[WFM::WebHostHidden]
	public ref class ThreadInterfaceConfiguration sealed
	{
	public:
		ThreadInterfaceConfiguration();

        void Init(IAsyncThreadPage^ page);

        void Show(Guid interfaceGuid);

        // system might rebuild a new device list due to state change
        void BuildDeviceList();

    private:
        using Device = ot::Device;

        void Configure();

        IAsyncThreadPage^ page_;
        // current device
        GUID              deviceGuid_;
    };
}
