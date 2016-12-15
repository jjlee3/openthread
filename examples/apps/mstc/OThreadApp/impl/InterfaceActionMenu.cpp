#include "detail/pch.h"
#include <stdlib.h>
#include <memory>
#include <tcOtApi/OtMeshLocalEid.h>
#include "detail/Options.h"
#include "detail/InterfaceActionMenu.h"
#include "detail/StdOutput.h"
#include "detail/Listener.h"
#include "detail/IOThreadMgr.h"

InterfaceActionMenu::InterfaceActionMenu(
    devices_t& devices,
    int        selected) :
    devices_{devices}, selected_{selected}
{
}

void
InterfaceActionMenu::show()
{
    auto& device = devices_[selected_];
    auto deviceRole = ::otGetDeviceRole(device.get());

    g_out([deviceRole](std::ostream& os)
    {
        os << "\nSelect one action and <Enter>:\n";
        if (deviceRole == kDeviceRoleDisabled)
        {
            os << "C> Connect\n";
        }
        else
        {
            os << "D> Disconnect\n" "S> Show Details\n"
                "L> Run Listener\n" "N> Client Mode\n";
        }
        os << "M> Main Menu\n" "Q> Quit\n";
    });
}

Menu::menu_csptr
InterfaceActionMenu::input(
    const std::string& line)
{
    if (line == "M" || line == "m")
    {
        MSTC_THROW_EXCEPTION(MainMenu{});
    }
    else if (line == "Q" || line == "q")
    {
        MSTC_THROW_EXCEPTION(Quit{});
    }
    else if (line == "C" || line == "c")
    {
        ConnectNetwork();
        MSTC_THROW_EXCEPTION(Refresh{});
    }
    else if (line == "D" || line == "d")
    {
        DisconnectNetwork();
        MSTC_THROW_EXCEPTION(Refresh{});
    }
    else if (line == "S" || line == "s")
    {
        ShowDetails();
        MSTC_THROW_EXCEPTION(Stay{});
    }
    else if (line == "L" || line == "l")
    {
        RunListener();
        MSTC_THROW_EXCEPTION(Stay{});
    }
    else if (line == "N" || line == "n")
    {
        SwitchToClientMode();
        MSTC_THROW_EXCEPTION(ClientMode{});
    }
    else
    {
        MSTC_THROW_EXCEPTION(Failure{"Invalid Input"});
    }
}

void
InterfaceActionMenu::ConnectNetwork()
{
    auto& device = devices_[selected_];
    //
    // !!!Configure!!! - We hard code it as a temp solution!!!
    //
    otNetworkName networkName = {};
    memcpy(networkName.m8, "Test Network", sizeof(networkName.m8));
    ::otSetNetworkName(device.get(), networkName.m8);

    otMasterKey masterKey = {};
    memcpy(masterKey.m8, "Pas$W0rd", sizeof(masterKey.m8));
    ::otSetMasterKey(device.get(), masterKey.m8, sizeof(masterKey.m8));

    ::otSetChannel(device.get(), 11);
    ::otSetMaxAllowedChildren(device.get(), 15);

    ::otSetPanId(device.get(), 0x4567);

    //
    // Bring up the interface and start the Thread logic
    //
    device.InterfaceUp();
    device.ThreadStart();

    g_out([](std::ostream& os)
    {
        os << " Network Connected" << std::endl;
    });
}

void
InterfaceActionMenu::DisconnectNetwork()
{
    auto& device = devices_[selected_];

    device.ThreadStop();
    device.InterfaceDown();

    g_out([](std::ostream& os)
    {
        os << " Network Disconnected" << std::endl;
    });
}

void
InterfaceActionMenu::ShowDetails()
{
    auto& device = devices_[selected_];

    char mac[256];
    device.MacFormatA(mac, _countof(mac));
    
    g_out([&mac](std::ostream& os)
    {
        os << " MAC:" << mac;
    });

    char ipv6[256];

    device.Ipv6FormatA(ipv6, _countof(ipv6));

    g_out([&ipv6](std::ostream& os)
    {
        os << " ML-EID:" << ipv6;
    });

    char rloc[16];
    auto rloc16 = ::otGetRloc16(device.get());
    sprintf_s(rloc, _countof(rloc), "%4X", rloc16);

    g_out([&rloc](std::ostream& os)
    {
        os << " RLOC:" << rloc;
    });

    auto deviceRole = ::otGetDeviceRole(device.get());
    if (deviceRole > kDeviceRoleChild)
    {
        uint8_t index = 0;
        otChildInfo childInfo;
        while (kThreadError_None == ::otGetChildInfoByIndex(device.get(), index, &childInfo))
        {
            ++index;
        }
        g_out([index](std::ostream& os)
        {
            os << " Children:" << static_cast<int>(index);
        });
    }
    g_out([](std::ostream& os)
    {
        os << std::endl;
    });
}

void
InterfaceActionMenu::RunListener()
{
    auto& device = devices_[selected_];

    mstc::ot::KMeshLocalEid ipv6{device.get()};
//  g_options.listenerAddr_.sin6_addr =
//      *reinterpret_cast<const in6_addr*>(ipv6.get());
    memcpy(&g_options.listenerAddr_.sin6_addr,
        ipv6.get(), sizeof(IN6_ADDR));

    auto listener = std::make_shared<Listener>();
    listener->start(g_options);
    g_ioThreadMgr->SetListener(std::move(listener));
}

void
InterfaceActionMenu::SwitchToClientMode()
{
    auto& device = devices_[selected_];

    mstc::ot::KMeshLocalEid ipv6{device.get()};
//  g_options.clientAddr_.sin6_addr =
//      *reinterpret_cast<const in6_addr*>(ipv6.get());
    memcpy(&g_options.clientAddr_.sin6_addr,
        ipv6.get(), sizeof(g_options.clientAddr_.sin6_addr));
    
    if (devices_.size() < 2)
    {
    }
    else if (devices_.size() == 2)
    {
        auto& devListener = devices_[1 - selected_];

        mstc::ot::KMeshLocalEid ipv6Listener{devListener.get()};
//      g_options.listenerAddr_.sin6_addr =
//          *reinterpret_cast<const in6_addr*>(ipv6Listener.get());
        memcpy(&g_options.listenerAddr_.sin6_addr,
            ipv6Listener.get(), sizeof(g_options.listenerAddr_.sin6_addr));
    }
    else
    {
    }

    g_out([](std::ostream& os)
    {
        os << " Switch to Client Mode" << std::endl;
    });
}