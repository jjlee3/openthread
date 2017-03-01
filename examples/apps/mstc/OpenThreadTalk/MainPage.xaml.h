//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "Types.h"
#include "IAsyncThreadPage.h"
#include "IMainPageUIElements.h"

namespace OpenThreadTalk
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public ref class MainPage sealed :
        public IAsyncThreadPage, public IMainPageUIElements
    {
    public:
        MainPage();

        // IAsyncThreadPage
        virtual void NotifyFromAsyncThread(String^ message, NotifyType type);

        void Notify(String^ message, NotifyType type);

        virtual UIElement^ ThreadGrid()
        {
            return (UIElement^)(ThrdGrid);
        }

        virtual UIElement^ TalkGrid()
        {
            return (UIElement^)(TlkGrid);
        }

    protected:
        virtual void OnNavigatedTo(NavigationEventArgs^ e) override;

    private:
        void OnLoaded(Object^ sender, RoutedEventArgs^ e);
        void OnUnloaded(Object^ sender, RoutedEventArgs^ e);
    };
}
