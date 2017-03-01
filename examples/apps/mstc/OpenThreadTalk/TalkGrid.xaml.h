//
// TalkGrid.xaml.h
// Declaration of the TalkGrid class
//

#pragma once

#include "TalkGrid.g.h"
#include "Types.h"
#include "IAsyncThreadPage.h"
#include "IMainPageUIElements.h"

namespace OpenThreadTalk
{
	[WFM::WebHostHidden]
	public ref class TalkGrid sealed
	{
	public:
		TalkGrid();

        void Init(IAsyncThreadPage^ page, IMainPageUIElements^ mainPageUIElements);

    private:
        // change protocol
        // TCP <-> UDP
        void Protocol_Changed(Object^ sender, RoutedEventArgs^ e);

        // change role from server client (or vice versa)
        void Role_Changed(Object^ sender, RoutedEventArgs^ e);
    };
}
