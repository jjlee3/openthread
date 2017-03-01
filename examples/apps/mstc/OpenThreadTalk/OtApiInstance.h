#pragma once

#ifndef _OT_API_INSTANCE_H_
#define _OT_API_INSTANCE_H_

#include <map>
#include <openthread.h>

namespace ot
{
    class ApiInstance
    {
    public:
        // resourec type
        using R = otApiInstance*;

        inline static constexpr R invalid() { return nullptr; }

#ifdef OTDLL
        ApiInstance();
#endif
        ApiInstance(ApiInstance&& rhs) noexcept;
        explicit ApiInstance(R&& res) noexcept;
        ~ApiInstance();

        ApiInstance& operator =(R&& res) noexcept;
        ApiInstance& operator =(ApiInstance&& rhs) noexcept;

        R get() const noexcept;

        void attach(R&& res) noexcept;
        R detach() noexcept;
        void close() noexcept;

        void swap(ApiInstance& rhs) noexcept;

        void SetDeviceAvailabilityChangedCallback(
            otDeviceAvailabilityChangedCallback, void*);

        void RemoveDeviceAvailabilityChangedCallback(
            otDeviceAvailabilityChangedCallback, void*);

    protected:
        using multi_map_t = std::multimap<otDeviceAvailabilityChangedCallback, void*>;

        ApiInstance(const ApiInstance&) = delete;
        ApiInstance& operator =(const ApiInstance&) = delete;

        R           res_ = invalid();
        multi_map_t multiMap_;
    };
}

#endif // _OT_API_INSTANCE_H_
