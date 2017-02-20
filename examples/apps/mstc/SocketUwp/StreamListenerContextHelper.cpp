#include "pch.h"
#include "StreamListenerContextHelper.h"

using namespace Concurrency;

void
SocketUwp::StreamListenerContextHelper::ReceiveLoop(
    StreamSocket^ streamSocket,
    DataReader^   dataReader,
    DataWriter^   dataWriter)
{
    // Read first 4 bytes (length of the subsequent string).
    create_task(dataReader->LoadAsync(sizeof(UINT32))).then(
        [this, streamSocket, dataReader, dataWriter](unsigned int size)
    {
        if (size < sizeof(UINT32))
        {
            // The underlying socket was closed before we were able to read the whole data.
            cancel_current_task();
        }

        unsigned int strLen = dataReader->ReadUInt32();
        return create_task(dataReader->LoadAsync(strLen)).then(
            [this, dataReader, dataWriter, strLen](unsigned int actualStrLen)
        {
            if (actualStrLen != strLen)
            {
                // The underlying socket was closed before we were able to read the whole data.
                cancel_current_task();
            }

            Receive(dataReader, strLen, dataWriter, true);
        });
    }).then([this, streamSocket, dataReader, dataWriter](task<void> previousTask)
    {
        try
        {
            // Try getting all exceptions from the continuation chain above this point.
            previousTask.get();

            // Everything went ok, so try to receive another string. The receive will continue until the stream is
            // broken (i.e. peer closed the socket).
            ReceiveLoop(streamSocket, dataReader, dataWriter);
        }
        catch (Exception^ ex)
        {
            NotifyFromAsyncThread("Read stream failed with error: " + ex->Message,
                NotifyType::Error);

            // Explicitly close the socket.
            delete streamSocket;
        }
        catch (task_canceled&)
        {
            // Do not print anything here - this will usually happen because user closed the client socket.

            // Explicitly close the socket.
            delete streamSocket;
        }
    });
}
