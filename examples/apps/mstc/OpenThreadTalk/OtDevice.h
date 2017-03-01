#pragma once

#ifndef _OT_DEVICE_H_
#define _OT_DEVICE_H_

#include <map>
#include <OtMemory.h>

typedef struct _GUID GUID;

namespace ot
{
    class ApiInstance;

    class Device : public Memory<otInstance>
    {
    public:
        using base_t = Memory<otInstance>;
        // resourec type
        using R = base_t::R;
        using base_t::base_t;

        Device(Device&& rhs) noexcept :
            Device{ std::move(rhs.res_) }
        {
            multiMap_ = std::move(rhs.multiMap_);
        }

        explicit Device(R&& res = invalid()) noexcept :
            base_t{ std::move(res) }
        {
        }

#ifdef OTDLL
        explicit Device(const ApiInstance&, const GUID&);
        explicit Device(otApiInstance*, const GUID&);
#else
#ifdef OPENTHREAD_MULTIPLE_INSTANCE
        explicit KDevice(void* instanceBuffer, size_t* bufferSize);
#else
        explicit KDevice();
#endif // OPENTHREAD_MULTIPLE_INSTANCE
#endif // OTDLL
        ~Device();

        void close() noexcept;

        GUID GetGuid() const;
        otDeviceRole GetRole() const;
        uint16_t GetRloc16() const;
        ThreadError GetChildInfoByIndex(uint8_t index, otChildInfo*) const;

        ThreadError SetStateChangeCallback(otStateChangedCallback,
            void* context);
        void RemoveStateChangeCallback(otStateChangedCallback,
            void* context);

        ThreadError SetNetworkName(const char* networkName);
        ThreadError SetMasterKey(const uint8_t* masterKey, uint8_t keyLength);
        ThreadError SetChannel(uint8_t channel);
        ThreadError SetMaxAllowChildren(uint8_t maxChildren);
        ThreadError SetPanId(otPanId panId);

        void ConnectNetwork();
        void DisconnectNetwork();

    protected:
        using multi_map_t = std::multimap<otStateChangedCallback, void*>;

        multi_map_t multiMap_;
    };
}

#endif // _OT_DEVICE_H_
