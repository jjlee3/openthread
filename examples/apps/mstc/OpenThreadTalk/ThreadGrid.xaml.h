//
// ThreadGrid.xaml.h
// Declaration of the ThreadGrid class
//

#pragma once

#include "ThreadGrid.g.h"
#include <memory>
#include "Types.h"
#include "IInterfaceUIElements.h"
#include "IThreadDeviceList.h"
#include "IAsyncThreadPage.h"
#include "IMainPageUIElements.h"
#include "ThreadGlobals.h"

namespace OpenThreadTalk
{
	[WFM::WebHostHidden]
    public ref class ThreadGrid sealed :
        public IInterfaceUIElements, public IThreadDeviceList
	{
	public:
		ThreadGrid();

        void Init(IAsyncThreadPage^ page, IMainPageUIElements^ mainPageUIElements);

        // IInterfaceUIElements
        virtual ThreadInterfaceConfiguration^ ThreadInterfaceConfiguration()
        {
            return ThrdInterfaceConfiguration;
        }

        virtual ThreadInterfaceDetails^ ThreadInterfaceDetails()
        {
            return ThrdInterfaceDetails;
        }

        // IThreadDeviceList
        virtual event ThreadDeviceListEventHandler^ ThreadDeviceListEvent;

        void OnLoaded(Object^ sender, RoutedEventArgs^ e);
        void OnUnloaded(Object^ sender, RoutedEventArgs^ e);

    private:
        void BuildInterfaceList();

        IAsyncThreadPage^     page_;
        IMainPageUIElements^  mainPageUIElements_;

        std::unique_ptr<ThreadGlobals> threadGlobals_;
    };
}
