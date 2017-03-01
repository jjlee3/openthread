//
// ThreadInterfaceConfig.xaml.cpp
// Implementation of the ThreadInterfaceConfig class
//

#include "pch.h"
#include <stdlib.h>
#include "ThreadInterfaceConfiguration.xaml.h"
#include "ThreadGlobals.h"

using namespace OpenThreadTalk;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

ThreadInterfaceConfiguration::ThreadInterfaceConfiguration()
{
	InitializeComponent();
}

void
ThreadInterfaceConfiguration::Init(
    IAsyncThreadPage^ page)
{
    page_ = page;

    InterfaceConfigOkButton->Click +=
        ref new RoutedEventHandler([this](Object^, RoutedEventArgs^)
    {
        try
        {
            Visibility = WUX::Visibility::Collapsed;
            Configure();
        }
        catch (Exception^ ex)
        {
            page_->NotifyFromAsyncThread(
                "Interface configuration error: " + ex->Message,
                NotifyType::Error);
        }
    });

    InterfaceConfigCancelButton->Click +=
        ref new RoutedEventHandler([this](Object^, RoutedEventArgs^)
    {
        Visibility = WUX::Visibility::Collapsed;
    });
}

void
ThreadInterfaceConfiguration::Show(
    Guid interfaceGuid)
{
    deviceGuid_ = interfaceGuid;
    Visibility = WUX::Visibility::Visible;
}

void
ThreadInterfaceConfiguration::Configure()
{
    auto pDevice = g_pThreadGlobals->Find(deviceGuid_);
    if (!pDevice)
    {
        throw ref new FailureException(L"GUID Not Found");
    }

    auto& device = *pDevice;

    size_t size = 0;
    otNetworkName networkName = {};
    wcstombs_s(&size, networkName.m8, InterfaceConfigName->Text->Data(),
        sizeof(networkName.m8));
    auto ret = device.SetNetworkName(networkName.m8);
    if (ret != kThreadError_None)
    {
        throw ref new FailureException(L"Set Network Name Failure");
    }

    size = 0;
    otMasterKey masterKey = {};
    wcstombs_s(&size, reinterpret_cast<char*>(masterKey.m8),
        sizeof(masterKey.m8), InterfaceConfigKey->Text->Data(),
        sizeof(masterKey.m8));
    ret = device.SetMasterKey(masterKey.m8, sizeof(masterKey.m8));
    if (ret != kThreadError_None)
    {
        throw ref new FailureException(L"Set Master Key Failure");
    }

    ret = device.SetChannel(
        static_cast<uint8_t>(InterfaceConfigChannel->Value));
    if (ret != kThreadError_None)
    {
        throw ref new FailureException(L"Set Channel Failure");
    }

    ret = device.SetMaxAllowChildren(
        static_cast<uint8_t>(InterfaceConfigMaxChildren->Value));
    if (ret != kThreadError_None)
    {
        throw ref new FailureException(L"Set Max Allow Children Failure");
    }

    ret = device.SetPanId(0x4567);
    if (ret != kThreadError_None)
    {
        throw ref new FailureException(L"Set Pan ID Failure");
    }

    device.ConnectNetwork();
}

// duplicate the functionality from ThreadInterfaceDetails::BuildDeviceList
void
ThreadInterfaceConfiguration::BuildDeviceList()
{
    if (Visibility != WUX::Visibility::Visible) { return; }

    bool devStillInList = false;

    auto& devices = g_pThreadGlobals->devices_;
    for (auto& device : devices)
    {
        auto deviceGuid = device.GetGuid();
        if (deviceGuid == deviceGuid_)
        {
            devStillInList = true;
            break;
        }
    }

    if (devStillInList)
    {
        Show(deviceGuid_);
    }
    else
    {
        Visibility = WUX::Visibility::Collapsed;
    }
}
