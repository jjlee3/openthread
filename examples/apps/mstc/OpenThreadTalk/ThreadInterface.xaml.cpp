//
// Interface.xaml.cpp
// Implementation of the Interface class
//

#include "pch.h"
#include "ThreadInterface.xaml.h"
#include "ThreadGlobals.h"
#include "OtDeviceRole.h"
#include "GuidHelper.h"

using namespace OpenThreadTalk;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

ThreadInterface::ThreadInterface()
{
    InitializeComponent();
}

UIElement^
ThreadInterface::Create(
    Guid                  interfaceGuid,
    IInterfaceUIElements^ interfaceUIElements)
{
    auto threadInterface = ref new ThreadInterface();
    threadInterface->Init(interfaceGuid, interfaceUIElements);

    return threadInterface;
}
 
void
ThreadInterface::Init(
    Guid                  interfaceGuid,
    IInterfaceUIElements^ interfaceUIElements)
{
    GUID deviceGuid = interfaceGuid_ = interfaceGuid;
    pDevice_ = g_pThreadGlobals->Find(deviceGuid);
    if (!pDevice_)
    {
        throw ref new FailureException(L"GUID Not Found");
    }

    auto& device = *pDevice_;
    auto deviceRole = device.GetRole();

    wchar_t szText[256] = {};
    swprintf_s(szText, L"%s\n\t" GUID_FORMATW L"\n\t%s",
        L"openthread interface", // TODO ...
        GUID_ARG(deviceGuid), ot::DeviceRole::ToStringW(deviceRole));
    InterfaceText->Text = ref new String(szText);

    if (deviceRole == kDeviceRoleDisabled)
    {
        ConnectButton->Click +=
            ref new RoutedEventHandler(
                [this, interfaceUIElements](Object^, RoutedEventArgs^)
        {
            auto thrdInterfaceConfiguration =
                interfaceUIElements->ThreadInterfaceConfiguration();
            thrdInterfaceConfiguration->Show(interfaceGuid_);
        });
        ConnectButton->Visibility = WUX::Visibility::Visible;
    }
    else
    {
        DetailsButton->Click +=
            ref new RoutedEventHandler(
                [this, interfaceUIElements](Object^, RoutedEventArgs^)
        {
            auto thrdInterfaceDetails = interfaceUIElements->ThreadInterfaceDetails();
            thrdInterfaceDetails->Show(interfaceGuid_);
        });
        DetailsButton->Visibility = WUX::Visibility::Visible;

        DisconnectButton->Click +=
            ref new RoutedEventHandler(
                [&device](Object^, RoutedEventArgs^)
        {
            device.DisconnectNetwork();
        });
        DisconnectButton->Visibility = WUX::Visibility::Visible;
    }
}
