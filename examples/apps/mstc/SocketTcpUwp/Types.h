#pragma once

namespace SocketTcpUwp
{
    namespace WU = Windows::UI;
    namespace WUC = WU::Core;
    namespace WUX = WU::Xaml;
    namespace WUXM = WUX::Media;
    namespace WUXC = WUX::Controls;
    namespace WN = Windows::Networking;
    namespace WNS = WN::Sockets;
    namespace WF = Windows::Foundation;
    namespace WFM = WF::Metadata;
    namespace WSS = Windows::Storage::Streams;
    namespace WAC = Windows::ApplicationModel::Core;

    using Object = Platform::Object;
    using String = Platform::String;
    using Exception = Platform::Exception;
    using InvalidArgumentException = Platform::InvalidArgumentException;
    using FailureException = Platform::FailureException;
    using Colors = WU::Colors;
    using CoreApplication = WAC::CoreApplication;
    using DispatchedHandler = WUC::DispatchedHandler;
    using CoreDispatcherPriority = WUC::CoreDispatcherPriority;
    using RoutedEventArgs = WUX::RoutedEventArgs;
    using RadioButton = WUXC::RadioButton;
    using Grid = WUXC::Grid;
    using SolidColorBrush = WUXM::SolidColorBrush;
    using IInputStream = WSS::IInputStream;
    using IOutputStream = WSS::IOutputStream;
    using DataReader = WSS::DataReader;
    using DataWriter = WSS::DataWriter;
    using HostName = WN::HostName;
    using EndpointPair = WN::EndpointPair;
    using SocketError = WNS::SocketError;
    using SocketErrorStatus = WNS::SocketErrorStatus;
    using Service = String;

    using StreamSocketListener = WNS::StreamSocketListener;
    using ConnectionReceivedEventArgs = WNS::StreamSocketListenerConnectionReceivedEventArgs;
    using ConnectionHandler = WF::TypedEventHandler<StreamSocketListener^, ConnectionReceivedEventArgs^>;
    using StreamSocket = WNS::StreamSocket;
}