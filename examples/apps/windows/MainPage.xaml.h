/*
 *  Copyright (c) 2016, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "MainPage.g.h"

namespace Thread
{
    public enum class NotifyType
    {
        StatusMessage,
        ErrorMessage
    };

    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public ref class MainPage sealed
    {
    public:
        MainPage();

        void OnResuming();

        void BuildInterfaceList();

        void ConnectNetwork(Platform::Guid InterfaceGuid);
        void ShowInterfaceDetails(Platform::Guid InterfaceGuid);
        void DisconnectNetwork(Platform::Guid InterfaceGuid);

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnUnloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void OnWindowSizeChanged(Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ args);
        void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ coreWindow, Windows::UI::Core::VisibilityChangedEventArgs^ args);

        UIElement^ CreateNewInterface(Platform::Guid InterfaceGuid);

        void ThreadConnectHandler();
        void TalkHandler();

        bool _isVisible;
        bool _isFullScreen;

        Windows::Foundation::Size _windowSize;
        
        void *_apiInstance;
        #define ApiInstance ((otApiInstance*)_apiInstance)

        std::vector<void*> _devices;

        Platform::Guid     _currentInterface;
        int                _checkedItems;
        Platform::Guid     _listenerInterface;
        Platform::Guid     _clientInterface;

    internal:
        void SelectTalkers(Windows::UI::Xaml::Controls::CheckBox^ selectedTalker,
            Platform::Guid selectedGuid);
        void UnselectTalkers(Windows::UI::Xaml::Controls::CheckBox^ selectedTalker,
            Platform::Guid selectedGuid);
        Platform::String^ GetIpv6Address(Platform::Guid guid);
        void NotifyUser(Platform::String^ strMessage, NotifyType type);

        static MainPage^ Current;
        MainPage^        _rootPage;
    };

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class ListenerContext sealed
    {
    public:
        ListenerContext(MainPage^ rootPage, Windows::Networking::Sockets::DatagramSocket^ listener);
        void OnMessage(Windows::Networking::Sockets::DatagramSocket^ socket,
            Windows::Networking::Sockets::DatagramSocketMessageReceivedEventArgs^ eventArguments);
        bool IsMatching(Windows::Networking::HostName^ hostName, Platform::String^ port);

    private:
        ~ListenerContext();
        void NotifyUserFromAsyncThread(Platform::String^ message, NotifyType type);
        void EchoMessage(Windows::Networking::Sockets::DatagramSocketMessageReceivedEventArgs^ eventArguments);

        MainPage^                                     rootPage;
        Windows::Networking::Sockets::DatagramSocket^ listener;

        CRITICAL_SECTION                              lock;
        Windows::Storage::Streams::IOutputStream^     outputStream;
        Windows::Networking::HostName^                hostName;
        Platform::String^                             port;
    };

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class ClientContext sealed
    {
    public:
        ClientContext(MainPage^ rootPage, Windows::Networking::Sockets::DatagramSocket^ listener);
        void OnMessage(Windows::Networking::Sockets::DatagramSocket^ socket,
            Windows::Networking::Sockets::DatagramSocketMessageReceivedEventArgs^ eventArguments);
        Windows::Storage::Streams::DataWriter^ GetWriter();
        boolean IsConnected();
        void SetConnected();

    private:
        ~ClientContext();
        void NotifyUserFromAsyncThread(Platform::String^ message, NotifyType type);

        MainPage^                                     rootPage;
        Windows::Networking::Sockets::DatagramSocket^ client;
        boolean                                       connected;
        Windows::Storage::Streams::DataWriter^        writer;
    };
}
