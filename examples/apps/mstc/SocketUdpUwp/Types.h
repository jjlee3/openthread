#pragma once

namespace SocketUdpUwp
{
    namespace WU = Windows::UI;
    namespace WUX = Windows::UI::Xaml;
    namespace WUXM = WUX::Media;
    namespace WUXC = Windows::UI::Xaml::Controls;
    namespace WN = Windows::Networking;
    namespace WNS = Windows::Networking::Sockets;
    namespace WF = Windows::Foundation;
    namespace WFM = Windows::Foundation::Metadata;
    namespace WSS = Windows::Storage::Streams;
    namespace WAC = Windows::ApplicationModel::Core;

    using Object = Platform::Object;
    using String = Platform::String;
    using Exception = Platform::Exception;
    using InvalidArgumentException = Platform::InvalidArgumentException;
    using FailureException = Platform::FailureException;
    using RoutedEventArgs = WUX::RoutedEventArgs;
    using SolidColorBrush = WUXM::SolidColorBrush;
    using Colors = WU::Colors;
    using RadioButton = WUXC::RadioButton;
    using Grid = WUXC::Grid;
    using CoreApplication = WAC::CoreApplication;
    using IInputStream = WSS::IInputStream;
    using IOutputStream = WSS::IOutputStream;
    using DataReader = WSS::DataReader;
    using DataWriter = WSS::DataWriter;
    using HostName = WN::HostName;
    using EndpointPair = WN::EndpointPair;
    using SocketError = WNS::SocketError;
    using SocketErrorStatus = WNS::SocketErrorStatus;
    using Service = String;

    using DatagramSocket = WNS::DatagramSocket;
    using MessageReceivedEventArgs = WNS::DatagramSocketMessageReceivedEventArgs;
    using MessageHandler = WF::TypedEventHandler<DatagramSocket^, MessageReceivedEventArgs^>;
}