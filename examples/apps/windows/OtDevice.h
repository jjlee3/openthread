#pragma once

#ifndef _OT_DEVICE_H_
#define _OT_DEVICE_H_

#include <map>
#include "openthread.h"
#include "OtMemory.h"
#include "OtFailure.h"

namespace ot
{
    class KApiInstance;

    class KDevice : public KMemory<otInstance>
    {
    public:
        class Exception : public KFailureException
        {
        public:
            using base_t = KFailureException;
            using base_t::base_t;
        };

    public:
        using base_t = KMemory<otInstance>;
        // resourec type
        using R = base_t::R;
        using base_t::base_t;

#ifdef OTDLL
        KDevice(const KApiInstance&, const GUID* devGuid);
        KDevice(otApiInstance*, const GUID* devGuid);
#else
#ifdef OPENTHREAD_MULTIPLE_INSTANCE
        KDevice(void* instanceBuffer, size_t* bufferSize);
#else
        KDevice();
#endif // OPENTHREAD_MULTIPLE_INSTANCE
#endif // OTDLL
        ~KDevice();

        void close() noexcept;

        GUID GetDeviceGuid() const;

        ThreadError SetStateChangeCallback(otStateChangedCallback,
            void* context);
        void RemoveStateChangeCallback(otStateChangedCallback,
            void* context);

        const wchar_t* RoleToString() const;
        int MacFormat(wchar_t* buffer, size_t sizeBuffer) const;
        int Ipv6Format(wchar_t* buffer, size_t sizeBuffer) const;

    protected:
        using multi_map_type = std::multimap<otStateChangedCallback, void*>;

        multi_map_type multiMap_;
    };
}

#endif // _OT_DEVICE_H_