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

#include "pch.h"
#include "MainPage.xaml.h"
#include "OtDevice.h"

using namespace Thread;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Networking;
using namespace Windows::Networking::Connectivity;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;

MainPage^ MainPage::Current = nullptr;
StreamSocketListener^ listener = nullptr;

#define GUID_FORMAT L"{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}"
#define GUID_ARG(guid) guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]

#define MAC8_FORMAT L"%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X"
#define MAC8_ARG(mac) mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], mac[6], mac[7]

PCWSTR ToString(otDeviceRole role)
{
    switch (role)
    {
    case kDeviceRoleOffline:    return L"Offline";
    case kDeviceRoleDisabled:   return L"Disabled";
    case kDeviceRoleDetached:   return L"Disconnected";
    case kDeviceRoleChild:      return L"Connected - Child";
    case kDeviceRoleRouter:     return L"Connected - Router";
    case kDeviceRoleLeader:     return L"Connected - Leader";
    }

    return L"Unknown Role State";
}

void OTCALL
ThreadDeviceAvailabilityCallback(
    bool        /* aAdded */, 
    const GUID* /* aDeviceGuid */, 
    _In_ void*  /* aContext */
    )
{
    // Trigger the interface list to update
    MainPage::Current->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        ref new Windows::UI::Core::DispatchedHandler(
            [=]() {
                MainPage::Current->BuildInterfaceList();
            }
        )
    );
}

void OTCALL
ThreadStateChangeCallback(
    uint32_t aFlags, 
    _In_ void* /* aContext */
    )
{
    if ((aFlags & OT_NET_ROLE) != 0)
    {
        // Trigger the interface list to update
        MainPage::Current->Dispatcher->RunAsync(
            Windows::UI::Core::CoreDispatcherPriority::Normal,
            ref new Windows::UI::Core::DispatchedHandler(
                [=]() {
                    MainPage::Current->BuildInterfaceList();
                }
            )
        );
    }
}

MainPage::MainPage()
{
    InitializeComponent();

    MainPage::Current = this;
    _rootPage = this;
    _isFullScreen = false;
    
    _apiInstance = nullptr;
    InterfaceConfigCancelButton->Click +=
        ref new RoutedEventHandler(
            [=](Platform::Object^, RoutedEventArgs^) {
                InterfaceConfiguration->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
            }
    );
    InterfaceConfigOkButton->Click +=
        ref new RoutedEventHandler(
            [=](Platform::Object^, RoutedEventArgs^) {
                InterfaceConfiguration->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
                ConnectNetwork(_currentInterface);
            }
    );
    InterfaceDetailsCloseButton->Click +=
        ref new RoutedEventHandler(
            [=](Platform::Object^, RoutedEventArgs^) {
                InterfaceDetails->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
            }
    );
    ThreadConnect->Click +=
        ref new RoutedEventHandler(
            [=](Platform::Object^, RoutedEventArgs^) {
                ThreadConnectHandler();
            }
    );
    Talk->Click +=
        ref new RoutedEventHandler(
            [=](Platform::Object^, RoutedEventArgs^) {
                TalkHandler();
            }
    );
}

void MainPage::ThreadConnectHandler()
{
    if (CoreApplication::Properties->HasKey("listener"))
    {
        CoreApplication::Properties->Remove("listener");
    }
    auto listener = ref new DatagramSocket();
    auto listenerContext = ref new ListenerContext(_rootPage, listener);
    listener->MessageReceived +=
        ref new TypedEventHandler<DatagramSocket^, DatagramSocketMessageReceivedEventArgs^>(
            listenerContext, &ListenerContext::OnMessage);

    // Events cannot be hooked up directly to the ScenarioInput1 object, as the object can fall out-of-scope and be
    // deleted. This would render any event hooked up to the object ineffective. The ListenerContext guarantees that
    // both the listener and object that serves its events have the same lifetime.
    CoreApplication::Properties->Insert("listener", listenerContext);

    auto serverAddr = ref new HostName(GetIpv6Address(_listenerInterface));
    create_task(listener->BindEndpointAsync(
        serverAddr, "51000")).then(
            [this, serverAddr](task<void> previousTask)
    {
            try
            {
                // Try getting an exception.
                previousTask.get();
                _rootPage->NotifyUser(
                    "Listening on address " + serverAddr->CanonicalName, 
                    NotifyType::StatusMessage);
            }
            catch (Exception^ exception)
            {
                CoreApplication::Properties->Remove("listener");
                _rootPage->NotifyUser(
                    "Start listening failed with error: " + exception->Message, 
                    NotifyType::ErrorMessage);
                return;
            }
    });

    if (CoreApplication::Properties->HasKey("client"))
    {
        CoreApplication::Properties->Remove("client");
    }
    auto client = ref new DatagramSocket();
    auto clientContext = ref new ClientContext(_rootPage, client);
    client->MessageReceived +=
        ref new TypedEventHandler<DatagramSocket^, DatagramSocketMessageReceivedEventArgs^>(
            clientContext, &ClientContext::OnMessage);

    auto clientAddr = ref new HostName(GetIpv6Address(_clientInterface));
    create_task(client->BindEndpointAsync(
        clientAddr, "51100")).then(
            [this, clientAddr](task<void> previousTask)
    {
            try
            {
                // Try getting an exception.
                previousTask.get();
                _rootPage->NotifyUser(
                    "Bind to address " + clientAddr->CanonicalName, 
                    NotifyType::StatusMessage);
            }
            catch (Exception^ exception)
            {
                CoreApplication::Properties->Remove("client");
                _rootPage->NotifyUser(
                    "Client binding failed with error: " + exception->Message, 
                    NotifyType::ErrorMessage);
                return;
            }
    });

    // Events cannot be hooked up directly to the ScenarioInput2 object, as the object can fall out-of-scope and be
    // deleted. This would render any event hooked up to the object ineffective. The SocketContext guarantees that
    // both the socket and object that serves its events have the same lifetime.
    CoreApplication::Properties->Insert("client", clientContext);

    _rootPage->NotifyUser("Connecting from: " + clientAddr->CanonicalName +" to: " + serverAddr->CanonicalName,
        NotifyType::StatusMessage);

    // Connect to the server (by default, the listener we created in the previous step).
    create_task(client->ConnectAsync(serverAddr, "51000")).then(
        [this, clientContext, clientAddr, serverAddr] (task<void> previousTask)
    {
        try
        {
            // Try getting an exception.
            previousTask.get();
            _rootPage->NotifyUser("Client connected from: " + clientAddr->CanonicalName +" to: " + serverAddr->CanonicalName, NotifyType::StatusMessage);
            clientContext->SetConnected();
        }
        catch (Exception^ exception)
        {
            _rootPage->NotifyUser("Connect failed with error: " + exception->Message, NotifyType::ErrorMessage);
            return;
        }
    });
}

String^ MainPage::GetIpv6Address(Platform::Guid guid)
{
    GUID deviceGuid = guid;
    ot::KDevice device{ApiInstance, &deviceGuid};
    wchar_t address[46] = { 0 };
    device.Ipv6Format(address, _countof(address));
    return ref new String(address);
}

void MainPage::TalkHandler()
{
    if (!CoreApplication::Properties->HasKey("client"))
    {
        _rootPage->NotifyUser("Please run previous steps before doing this one.", NotifyType::ErrorMessage);
        return;
    }

    ClientContext^ clientContext = dynamic_cast<ClientContext^>(CoreApplication::Properties->Lookup("client"));
    if (!clientContext->IsConnected())
    {
        _rootPage->NotifyUser("The socket is not yet connected. Please wait.", NotifyType::ErrorMessage);
        return;
    }

    String^ stringToSend("Hello from mstc");
    try
    {
        clientContext->GetWriter()->WriteString(stringToSend);
        _rootPage->NotifyUser("Sending - " + stringToSend, NotifyType::StatusMessage);
    }
    catch (Exception^ ex)
    {
        _rootPage->NotifyUser("Send failed with error: " + ex->Message, NotifyType::ErrorMessage);
    }

    // Write the locally buffered data to the network. Please note that write operation will succeed
    // even if the server is not listening.
    create_task(clientContext->GetWriter()->StoreAsync()).then(
        [this] (task<unsigned int> writeTask)
    {
        try
        {
            // Try getting an exception.
            writeTask.get();
        }
        catch (Exception^ exception)
        {
            _rootPage->NotifyUser("Send failed with error: " + exception->Message, NotifyType::ErrorMessage);
        }
    });
}

void MainPage::OnNavigatedTo(NavigationEventArgs^ e)
{
    Window::Current->CoreWindow->VisibilityChanged += ref new TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::VisibilityChangedEventArgs^>(this, &MainPage::OnVisibilityChanged);

    SizeChanged += ref new SizeChangedEventHandler(this, &MainPage::OnWindowSizeChanged);
    Loaded += ref new RoutedEventHandler(this, &MainPage::OnLoaded);
    Unloaded += ref new RoutedEventHandler(this, &MainPage::OnUnloaded);

    _isVisible = Window::Current->CoreWindow->Visible;
}

void MainPage::OnLoaded(Object^ sender, RoutedEventArgs^ e)
{
    // Initialize api handle
    _apiInstance = otApiInit();
    if (_apiInstance)
    {
        // Register for device availability callbacks
        otSetDeviceAvailabilityChangedCallback(ApiInstance, ThreadDeviceAvailabilityCallback, nullptr);

        // Build the initial list
        BuildInterfaceList();
    }
}

void MainPage::OnUnloaded(Object^ sender, RoutedEventArgs^ e)
{
    // Unregister for callbacks
    otSetDeviceAvailabilityChangedCallback(ApiInstance, nullptr, nullptr);

    // Clean up api handle
    otApiFinalize(ApiInstance);
    _apiInstance = nullptr;
}

void MainPage::OnResuming()
{
}

void MainPage::OnWindowSizeChanged(Object^ sender, SizeChangedEventArgs^ args)
{
    _windowSize = args->NewSize;
}

void MainPage::OnVisibilityChanged(Windows::UI::Core::CoreWindow^ coreWindow, Windows::UI::Core::VisibilityChangedEventArgs^ args)
{
    // The Visible property is toggled when the app enters and exits minimized state.
    // But obscuring the app with another window does not change Visible state, oddly enough.
    _isVisible = args->Visible;
}

void MainPage::ShowInterfaceDetails(Platform::Guid InterfaceGuid)
{
    if (ApiInstance == nullptr) return;

    GUID deviceGuid = InterfaceGuid;
    auto device = otInstanceInit(ApiInstance, &deviceGuid);

    auto extendedAddress = otGetExtendedAddress(device);
    if (extendedAddress)
    {
        WCHAR szMac[256] = { 0 };
        swprintf_s(szMac, 256, MAC8_FORMAT, MAC8_ARG(extendedAddress));
        InterfaceMacAddress->Text = ref new String(szMac);

        otFreeMemory(extendedAddress);
    }
    else
    {
        InterfaceMacAddress->Text = L"ERROR";
    }

    auto ml_eid = otGetMeshLocalEid(device);
    if (ml_eid)
    {
        WCHAR szAddress[46] = { 0 };
        RtlIpv6AddressToStringW((const PIN6_ADDR)ml_eid, szAddress);
        InterfaceML_EID->Text = ref new String(szAddress);

        otFreeMemory(ml_eid);
    }
    else
    {
        InterfaceML_EID->Text = L"ERROR";
    }
    
    auto rloc16 = otGetRloc16(device);
    WCHAR szRloc[16] = { 0 };
    swprintf_s(szRloc, 16, L"%4x", rloc16);
    InterfaceRLOC->Text = ref new String(szRloc);

    if (otGetDeviceRole(device) > kDeviceRoleChild)
    {
        uint8_t index = 0;
        otChildInfo childInfo;
        while (kThreadError_None == otGetChildInfoByIndex(device, index, &childInfo))
        {
            index++;
        }

        WCHAR szText[64] = { 0 };
        swprintf_s(szText, 64, L"%d", index);
        InterfaceChildren->Text = ref new String(szText);

        InterfaceNeighbors->Text = L"unknown";

        InterfaceNeighbors->Visibility = Windows::UI::Xaml::Visibility::Visible;
        InterfaceNeighborsText->Visibility = Windows::UI::Xaml::Visibility::Visible;
        InterfaceChildren->Visibility = Windows::UI::Xaml::Visibility::Visible;
        InterfaceChildrenText->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }

    // Show the details
    InterfaceDetails->Visibility = Windows::UI::Xaml::Visibility::Visible;

    otFreeMemory(device);
}

UIElement^ MainPage::CreateNewInterface(Platform::Guid InterfaceGuid)
{
    GUID deviceGuid = InterfaceGuid;

    auto device = otInstanceInit(ApiInstance, &deviceGuid);
    auto deviceRole = otGetDeviceRole(device);

    WCHAR szText[256] = { 0 };
    swprintf_s(szText, 256, L"%s\r\n\t" GUID_FORMAT L"\r\n\t%s",
        L"openthread interface", // TODO ...
        GUID_ARG(deviceGuid), 
        ::ToString(deviceRole));

    auto InterfaceStackPanel = ref new StackPanel();
    InterfaceStackPanel->Orientation = Orientation::Horizontal;

    auto checkBox = ref new CheckBox();
    checkBox->Checked +=
        ref new RoutedEventHandler(
            [=](Platform::Object^, RoutedEventArgs^) {
            SelectTalkers(checkBox, InterfaceGuid);
        }
    );
    checkBox->Unchecked +=
        ref new RoutedEventHandler(
            [=](Platform::Object^, RoutedEventArgs^) {
            UnselectTalkers(checkBox, InterfaceGuid);
        }
    );
    InterfaceStackPanel->Children->Append(checkBox);

    auto InterfaceTextBlock = ref new TextBlock();
    InterfaceTextBlock->Text = ref new String(szText);
    InterfaceTextBlock->FontSize = 16;
    InterfaceTextBlock->Margin = Thickness(10);
    InterfaceTextBlock->TextWrapping = TextWrapping::Wrap;
    InterfaceStackPanel->Children->Append(InterfaceTextBlock);

    if (deviceRole == kDeviceRoleDisabled)
    {
        auto ConnectButton = ref new Button();
        ConnectButton->Content = ref new String(L"Connect");
        ConnectButton->Click +=
            ref new RoutedEventHandler(
                [=](Platform::Object^, RoutedEventArgs^) {
                    _currentInterface = InterfaceGuid;
                    InterfaceConfiguration->Visibility = Windows::UI::Xaml::Visibility::Visible;
                }
        );
        InterfaceStackPanel->Children->Append(ConnectButton);
    }
    else
    {
        auto DetailsButton = ref new Button();
        DetailsButton->Content = ref new String(L"Details");
        DetailsButton->Click +=
            ref new RoutedEventHandler(
                [=](Platform::Object^, RoutedEventArgs^) {
                    ShowInterfaceDetails(InterfaceGuid);
                }
        );
        InterfaceStackPanel->Children->Append(DetailsButton);

        auto DisconnectButton = ref new Button();
        DisconnectButton->Content = ref new String(L"Disconnect");
        DisconnectButton->Click +=
            ref new RoutedEventHandler(
                [=](Platform::Object^, RoutedEventArgs^) {
                    DisconnectNetwork(InterfaceGuid);
                }
        );
        InterfaceStackPanel->Children->Append(DisconnectButton);
    }

    // Register for callbacks on the device
    otSetStateChangedCallback(device, ThreadStateChangeCallback, nullptr);

    // Cache the device
    _devices.push_back(device);

    return InterfaceStackPanel;
}

void MainPage::SelectTalkers(
    CheckBox^      selectedTalker,
    Platform::Guid selectedInterfaceGuid)
{
    if (_checkedItems >= 2)
    {
        selectedTalker->IsChecked = false;
        return;
    }

    ++_checkedItems;
    switch (_checkedItems)
    {
    case 1:
        _listenerInterface = selectedInterfaceGuid;
        break;
    case 2:
        _clientInterface = selectedInterfaceGuid;
        ThreadConnect->IsEnabled = true;
        Talk->IsEnabled = true;
        break;
    }
}

void MainPage::UnselectTalkers(
    CheckBox^      selectedTalker,
    Platform::Guid selectedInterfaceGuid)
{
    --_checkedItems;
    if (_checkedItems != 2)
    {
        ThreadConnect->IsEnabled = false;
        Talk->IsEnabled = false;
    }
}

void MainPage::BuildInterfaceList()
{
    if (ApiInstance == nullptr) return;

    _checkedItems = 0;
    ThreadConnect->IsEnabled = false;
    Talk->IsEnabled = false;
    StatusBlock->Text = "";

    // Clear all existing children
    InterfaceList->Items->Clear();

    // Clean up devices
    for each (auto device in _devices)
    {
        // Unregister for callbacks for the device
        otSetStateChangedCallback((otInstance*)device, nullptr, nullptr);

        // Free the device
        otFreeMemory(device);
    }
    _devices.clear();

    // Enumerate the new device list
    auto deviceList = otEnumerateDevices(ApiInstance);
    if (deviceList)
    {
        // Dump the results to the console
        for (DWORD dwIndex = 0; dwIndex < deviceList->aDevicesLength; dwIndex++)
        {
            InterfaceList->Items->Append(CreateNewInterface(deviceList->aDevices[dwIndex]));
        }

        otFreeMemory(deviceList);
    }
}

void MainPage::ConnectNetwork(Platform::Guid InterfaceGuid)
{
    if (ApiInstance == nullptr) return;

    GUID deviceGuid = InterfaceGuid;
    auto device = otInstanceInit(ApiInstance, &deviceGuid);

    //
    // Configure
    //

    otNetworkName networkName = {};
    wcstombs(networkName.m8, InterfaceConfigName->Text->Data(), sizeof(networkName.m8));
    otSetNetworkName(device, networkName.m8);

    otMasterKey masterKey = {};
    wcstombs((char*)masterKey.m8, InterfaceConfigKey->Text->Data(), sizeof(masterKey.m8));
    otSetMasterKey(device, masterKey.m8, sizeof(masterKey.m8));

    otSetChannel(device, (uint8_t)InterfaceConfigChannel->Value);
    otSetMaxAllowedChildren(device, (uint8_t)InterfaceConfigMaxChildren->Value);

    otSetPanId(device, 0x4567);

    //
    // Bring up the interface and start the Thread logic
    //

    otInterfaceUp(device);

    otThreadStart(device);

    // Cleanup
    otFreeMemory(device);
}

void MainPage::DisconnectNetwork(Platform::Guid InterfaceGuid)
{
    if (ApiInstance == nullptr) return;

    GUID deviceGuid = InterfaceGuid;
    auto device = otInstanceInit(ApiInstance, &deviceGuid);

    //
    // Start the Thread logic and the interface
    //

    otThreadStop(device);

    otInterfaceDown(device);

    // Cleanup
    otFreeMemory(device);
}

void MainPage::NotifyUser(String^ strMessage, NotifyType type)
{
    switch (type)
    {
    case NotifyType::StatusMessage:
        StatusBorder->Background = ref new SolidColorBrush(Windows::UI::Colors::Green);
        break;
    case NotifyType::ErrorMessage:
        StatusBorder->Background = ref new SolidColorBrush(Windows::UI::Colors::Red);
        break;
    default:
        break;
    }
    StatusBlock->Text = strMessage;

    // Collapse the StatusBlock if it has no text to conserve real estate.
    if (StatusBlock->Text != "")
    {
        StatusBorder->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }
    else
    {
        StatusBorder->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    }
}

ListenerContext::ListenerContext(MainPage^ rootPage, DatagramSocket^ listener)
{
    this->rootPage = rootPage;
    this->listener = listener;
    InitializeCriticalSectionEx(&lock, 0, 0);
}

ListenerContext::~ListenerContext()
{
    // A DatagramSocket can be closed in two ways:
    //  - explicitly: using the 'delete' keyword (listener is closed even if there are outstanding references to it).
    //  - implicitly: removing the last reference to it (i.e., falling out-of-scope).
    //
    // When a DatagramSocket is closed implicitly, it can take several seconds for the local UDP port being used
    // by it to be freed/reclaimed by the lower networking layers. During that time, other UDP sockets on the machine
    // will not be able to use the port. Thus, it is strongly recommended that DatagramSocket instances be explicitly
    // closed before they go out of scope(e.g., before application exit). The call below explicitly closes the socket.
    delete listener;
    listener = nullptr;

    DeleteCriticalSection(&lock);
}

void ListenerContext::OnMessage(
    Windows::Networking::Sockets::DatagramSocket^ socket,
    Windows::Networking::Sockets::DatagramSocketMessageReceivedEventArgs^ eventArguments)
{
    if (outputStream != nullptr)
    {
        EchoMessage(eventArguments);
        return;
    }

    // We do not have an output stream yet so create one.
    create_task(socket->GetOutputStreamAsync(eventArguments->RemoteAddress, eventArguments->RemotePort)).then(
        [this, socket, eventArguments] (IOutputStream^ stream)
    {
        // It might happen that the OnMessage was invoked more than once before the GetOutputStreamAsync call
        // completed. In this case we will end up with multiple streams - just keep one of them.
        EnterCriticalSection(&lock);

        if (outputStream == nullptr)
        {
            outputStream = stream;
            hostName = eventArguments->RemoteAddress;
            port = eventArguments->RemotePort;
        }

        LeaveCriticalSection(&lock);

        EchoMessage(eventArguments);
    }).then([this, socket, eventArguments] (task<void> previousTask)
    {
        try
        {
            // Try getting all exceptions from the continuation chain above this point.
            previousTask.get();
        }
        catch (Exception^ exception)
        {
            NotifyUserFromAsyncThread(
                "Getting an output stream failed with error: " + exception->Message, 
                NotifyType::ErrorMessage);
        }
    });
}

bool ListenerContext::IsMatching(HostName^ hostName, String^ port)
{
    return (this->hostName == hostName && this->port == port);
}

void ListenerContext::EchoMessage(DatagramSocketMessageReceivedEventArgs^ eventArguments)
{
    if (!IsMatching(eventArguments->RemoteAddress, eventArguments->RemotePort))
    {
        // In the sample we are communicating with just one peer. To communicate with multiple peers, an application
        // should cache output streams (e.g., by using a hash map), because creating an output stream for each
        // received datagram is costly. Keep in mind though, that every cache requires logic to remove old
        // or unused elements; otherwise, the cache will turn into a memory leaking structure.
        NotifyUserFromAsyncThread("Got datagram from " + eventArguments->RemoteAddress->DisplayName + ":" +
            eventArguments->RemotePort + ", but already 'connected' to " + hostName->DisplayName + ":" +
            port, NotifyType::ErrorMessage);
    }

    create_task(outputStream->WriteAsync(eventArguments->GetDataReader()->DetachBuffer())).then(
        [this] (task<unsigned int> writeTask)
    {
        try
        {
            // Try getting an exception.
            writeTask.get();
        }
        catch (Exception^ exception)
        {
            NotifyUserFromAsyncThread(
                "Echoing a message failed with error: " + exception->Message, 
                NotifyType::ErrorMessage);
        }
    });
}

void ListenerContext::NotifyUserFromAsyncThread(String^ message, NotifyType type)
{
    rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, message, type] ()
    {
        rootPage->NotifyUser(message, type);
    }));
}

ClientContext::ClientContext(MainPage^ rootPage, DatagramSocket^ client)
{
    this->rootPage  = rootPage;
    this->client    = client;
    this->connected = false;
}

ClientContext::~ClientContext()
{
    // A DatagramSocket can be closed in two ways:
    //  - explicitly: using the 'delete' keyword (listener is closed even if there are outstanding references to it).
    //  - implicitly: removing the last reference to it (i.e., falling out-of-scope).
    //
    // When a DatagramSocket is closed implicitly, it can take several seconds for the local UDP port being used
    // by it to be freed/reclaimed by the lower networking layers. During that time, other UDP sockets on the machine
    // will not be able to use the port. Thus, it is strongly recommended that DatagramSocket instances be explicitly
    // closed before they go out of scope(e.g., before application exit). The call below explicitly closes the socket.
    connected = false;
    delete client;
    client = nullptr;

    if (writer != nullptr)
    {
        delete writer;
        writer = nullptr;
    }
}

void ClientContext::OnMessage(DatagramSocket^ socket, DatagramSocketMessageReceivedEventArgs^ eventArguments)
{
    try
    {
        // Interpret the incoming datagram's entire contents as a string.
        unsigned int stringLength = eventArguments->GetDataReader()->UnconsumedBufferLength;
        String^ receivedMessage = eventArguments->GetDataReader()->ReadString(stringLength);

        NotifyUserFromAsyncThread(
            "Received data from server peer: \"" + receivedMessage + "\"", 
            NotifyType::StatusMessage);
    }
    catch (Exception^ exception)
    {
        SocketErrorStatus socketError = SocketError::GetStatus(exception->HResult);
        if (socketError == SocketErrorStatus::ConnectionResetByPeer)
        {
            // This error would indicate that a previous send operation resulted in an ICMP "Port Unreachable" message.
            NotifyUserFromAsyncThread(
                "Peer does not listen on the specific port. Please make sure that you run step 1 first " +
                "or you have a server properly working on a remote server.", 
                NotifyType::ErrorMessage);
        }
        else if (socketError != SocketErrorStatus::Unknown)
        {
            NotifyUserFromAsyncThread(
                "Error happened when receiving a datagram: " + socketError.ToString(), 
                NotifyType::ErrorMessage);
        }
        else
        {
            throw;
        }
    }
}

DataWriter^ ClientContext::GetWriter()
{
    if (writer == nullptr)
    {
        writer = ref new DataWriter(client->OutputStream);
    }

    return writer;
}

boolean ClientContext::IsConnected()
{
    return connected;
}

void ClientContext::SetConnected()
{
    connected = true;
}

void ClientContext::NotifyUserFromAsyncThread(String^ message, NotifyType type)
{
    rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, message, type] ()
    {
        rootPage->NotifyUser(message, type);
    }));
}