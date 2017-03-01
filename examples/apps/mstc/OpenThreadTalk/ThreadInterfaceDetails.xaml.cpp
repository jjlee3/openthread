//
// ThreadInterfaceDetails.xaml.cpp
// Implementation of the ThreadInterfaceDetails class
//

#include "pch.h"
#include <stdlib.h>
#include "ThreadInterfaceDetails.xaml.h"
#include "ThreadGlobals.h"
#include "OtExtAddr.h"
#include "OtMeshLocalEid.h"

using namespace OpenThreadTalk;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

ThreadInterfaceDetails::ThreadInterfaceDetails()
{
	InitializeComponent();
}

void
ThreadInterfaceDetails::Init(
    IAsyncThreadPage^ page)
{
    page_ = page;

    InterfaceDetailsCloseButton->Click +=
        ref new RoutedEventHandler([this](Object^, RoutedEventArgs^)
    {
        Visibility = WUX::Visibility::Collapsed;
    });
}

void
ThreadInterfaceDetails::Show(
    Guid interfaceGuid)
{
    deviceGuid_ = interfaceGuid;
    auto pDevice = g_pThreadGlobals->Find(deviceGuid_);
    if (!pDevice)
    {
        throw ref new FailureException(L"GUID Not Found");
    }

    auto& device = *pDevice;

    {
        ot::ExtendedAddress extendedAddress{ device };
        wchar_t szMac[256] = {};
        extendedAddress.MacAddressStringW(szMac, _countof(szMac));
        InterfaceMacAddress->Text = ref new String(szMac);
    }

    {
        ot::MeshLocalEid mlEid{ device };
        wchar_t szIpv6[46] = {};
        mlEid.Ipv6StringW(szIpv6, _countof(szIpv6));
        InterfaceML_EID->Text = ref new String(szIpv6);
    }

    auto rloc16 = device.GetRloc16();
    wchar_t szRloc[16];
    swprintf_s(szRloc, L"%4x", rloc16);
    InterfaceRLOC->Text = ref new String(szRloc);

    auto deviceRole = device.GetRole();
    if (deviceRole > kDeviceRoleChild)
    {
        uint8_t     index = 0;
        otChildInfo childInfo;
        while (kThreadError_None == device.GetChildInfoByIndex(index, &childInfo))
        {
            index++;
        }

        wchar_t szText[64] = {};
        swprintf_s(szText, L"%d", index);
        InterfaceChildren->Text = ref new String(szText);
        InterfaceNeighbors->Text = L"unknown";

        InterfaceChildrenText->Visibility = WUX::Visibility::Visible;
        InterfaceChildren->Visibility = WUX::Visibility::Visible;
        InterfaceNeighborsText->Visibility = WUX::Visibility::Visible;
        InterfaceNeighbors->Visibility = WUX::Visibility::Visible;
    }
    else
    {
        InterfaceChildren->Text = L"";
        InterfaceNeighbors->Text = L"";

        InterfaceChildrenText->Visibility = WUX::Visibility::Collapsed;
        InterfaceChildren->Visibility = WUX::Visibility::Collapsed;
        InterfaceNeighborsText->Visibility = WUX::Visibility::Collapsed;
        InterfaceNeighbors->Visibility = WUX::Visibility::Collapsed;
    }

    Visibility = WUX::Visibility::Visible;
}

void
ThreadInterfaceDetails::BuildDeviceList()
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
