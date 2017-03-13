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
#include <utility>
#include <algorithm>
#include "ClientControl.xaml.h"
#include "Factory.h"

using namespace Thread;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

ClientControl::ClientControl()
{
    InitializeComponent();

    ServerPort->Text = DEF_SERVER_PORT.ToString();
    ClientPort->Text = DEF_PORT.ToString();
}

void
ClientControl::Init(
    IAsyncThreadNotify^  notify,
    IMainPageUIElements^ mainPageUIElements)
{
    _notify = std::move(notify);
    _mainPageUIElements = std::move(mainPageUIElements);
}

void
ClientControl::ProtocolChanged(
    Protocol protocol)
{
    _protocol = protocol;
}

void
ClientControl::Connect_Click(
    Object^          sender,
    RoutedEventArgs^ e)
{
    try
    {
        auto clientArgs = ref new ClientArgs();

        auto serverIP = ServerIP->Text;
        if (serverIP->IsEmpty()) Exception::CreateException(E_INVALIDARG, "No Server IP");

        // check valid chars of Ipv6 Address
        if (std::all_of(serverIP->Data(), serverIP->Data() + serverIP->Length(),
            [](wchar_t c)
        {
            if (c < '0') return false;
            else if (c <= '9') return true; // '0' ~ '9'
            else if (c == ':') return true; // ':'
            else if (c < 'a') return false;
            else if (c <= 'z') return true; // 'a' ~ 'z'
            else return false;
        }))
            Exception::CreateException(E_INVALIDARG, "Not a valid Server IPv6 address");

        clientArgs->ServerHostName = ref new HostName(serverIP);

        clientArgs->ServerPort = ServerPort->Text;
        if (clientArgs->ServerPort->IsEmpty()) clientArgs->ServerPort = DEF_SERVER_PORT.ToString();

        auto clientIP = ClientIP->Text;
        if (clientIP->IsEmpty()) Exception::CreateException(E_INVALIDARG, "No Client IP");

        // check valid chars of Ipv6 Address
        if (std::all_of(clientIP->Data(), clientIP->Data() + clientIP->Length(),
            [](wchar_t c)
        {
            if (c < '0') return false;
            else if (c <= '9') return true; // '0' ~ '9'
            else if (c == ':') return true; // ':'
            else if (c < 'a') return false;
            else if (c <= 'z') return true; // 'a' ~ 'z'
            else return false;
        }))
            Exception::CreateException(E_INVALIDARG, "Not a valid client IPv6 address");


        clientArgs->ClientHostName = ref new HostName(clientIP);

        clientArgs->ClientPort = ClientPort->Text;
        if (clientArgs->ClientPort->IsEmpty()) clientArgs->ClientPort = DEF_PORT.ToString();

        auto cleintContext = Factory::CreateClientContext(_notify, clientArgs, _protocol);
        cleintContext->Connect_Click(sender, e);
    }
    catch (Exception^ ex)
    {
        CoreApplication::Properties->Remove("clientContext");
        _notify->NotifyFromAsyncThread(
            "Connecting failed with input error: " + ex->Message,
            NotifyType::Error);
    }
    catch (task_canceled&)
    {
        CoreApplication::Properties->Remove("clientContext");
    }
}

void
ClientControl::Send_Click(
    Object^          sender,
    RoutedEventArgs^ e)
{
    try
    {
        auto input = Input->Text;
        if (input->IsEmpty()) Exception::CreateException(E_INVALIDARG, "No Input");

        if (!CoreApplication::Properties->HasKey("clientContext"))
            Exception::CreateException(E_UNEXPECTED, "Not Connected");

        auto clientContext = dynamic_cast<IClientContext^>(
            CoreApplication::Properties->Lookup("clientContext"));
        if (clientContext == nullptr)
            Exception::CreateException(E_UNEXPECTED, "No clientContext");

        clientContext->Send_Click(sender, e, input);
    }
    catch (Exception^ ex)
    {
        _notify->NotifyFromAsyncThread(
            "Sending message failed with error: " + ex->Message,
            NotifyType::Error);
    }
}

void
ClientControl::Exit_Click(
    Object^          sender,
    RoutedEventArgs^ e)
{
    _mainPageUIElements->TalkGrid->Visibility = Xaml::Visibility::Collapsed;
    _mainPageUIElements->ThreadGrid->Visibility = Xaml::Visibility::Visible;
}
