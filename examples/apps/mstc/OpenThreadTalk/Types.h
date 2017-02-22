#pragma once

namespace OpenThreadTalk
{
    namespace WAC = Windows::ApplicationModel::Core;
    namespace WF = Windows::Foundation;
    namespace WFM = WF::Metadata;
    namespace WN = Windows::Networking;
    namespace WNS = WN::Sockets;
    namespace WSS = Windows::Storage::Streams;
    namespace WU = Windows::UI;
    namespace WUC = WU::Core;
    namespace WUX = WU::Xaml;
    namespace WUXC = WUX::Controls;
    namespace WUXM = WUX::Media;

    using Object = Platform::Object;
    using String = Platform::String;
    using Exception = Platform::Exception;
    using InvalidArgumentException = Platform::InvalidArgumentException;
    using FailureException = Platform::FailureException;
    using Service = String;

    using CoreApplication = WAC::CoreApplication;
    using EndpointPair = WN::EndpointPair;
    using HostName = WN::HostName;
    using SocketError = WNS::SocketError;
    using SocketErrorStatus = WNS::SocketErrorStatus;
    using DataReader = WSS::DataReader;
    using DataWriter = WSS::DataWriter;
    using IInputStream = WSS::IInputStream;
    using IOutputStream = WSS::IOutputStream;
    using Colors = WU::Colors;
    using CoreDispatcherPriority = WUC::CoreDispatcherPriority;
    using DispatchedHandler = WUC::DispatchedHandler;
    using RoutedEventArgs = WUX::RoutedEventArgs;
    using Grid = WUXC::Grid;
    using RadioButton = WUXC::RadioButton;
    using SolidColorBrush = WUXM::SolidColorBrush;

    using StreamSocketListener = WNS::StreamSocketListener;
    using ConnectionReceivedEventArgs = WNS::StreamSocketListenerConnectionReceivedEventArgs;
    using ConnectionHandler = WF::TypedEventHandler<StreamSocketListener^, ConnectionReceivedEventArgs^>;
    using StreamSocket = WNS::StreamSocket;

    using DatagramSocket = WNS::DatagramSocket;
    using MessageReceivedEventArgs = WNS::DatagramSocketMessageReceivedEventArgs;
    using MessageHandler = WF::TypedEventHandler<DatagramSocket^, MessageReceivedEventArgs^>;
}