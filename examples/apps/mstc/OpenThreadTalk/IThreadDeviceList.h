#pragma once

namespace OpenThreadTalk
{
    // callback provided by subscribers 
    public delegate void ThreadDeviceListEventHandler();

    // provide a formal notification interface to subscribers
    public interface struct IThreadDeviceList
    {
        event ThreadDeviceListEventHandler^ ThreadDeviceListEvent;
    };
}
