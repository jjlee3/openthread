#pragma once

#ifndef _OT_API_INSTANCE_H_
#define _OT_API_INSTANCE_H_

#include "openthread.h"
#include "OtFailure.h"

namespace ot
{
    class KApiInstance
    {
    public:
        class Exception : public KFailureException
        {
        public:
            using base_t = KFailureException;
            using base_t::base_t;
        };

    public:
        // resourec type
        using R = otApiInstance*;

        inline static constexpr R invalid() { return nullptr; }

#ifdef OTDLL
        KApiInstance();
#else
        KApiInstance() noexcept {}
#endif
        KApiInstance(KApiInstance&& rhs) noexcept;
        explicit KApiInstance(R&& res) noexcept;
        ~KApiInstance();

        KApiInstance& operator =(R&& res) noexcept;
        KApiInstance& operator =(KApiInstance&& rhs) noexcept;

        R get() const noexcept;

        void attach(R&& res) noexcept;
        R detach() noexcept;
        void close() noexcept;

        void swap(KApiInstance& rhs) noexcept;

        void SetDeviceAvailabilityChangedCallback(
            otDeviceAvailabilityChangedCallback, void*);

        void RemoveDeviceAvailabilityChangedCallback(
            otDeviceAvailabilityChangedCallback, void*);

    protected:
        using multi_map_type = std::multimap<otDeviceAvailabilityChangedCallback, void*>;

        KApiInstance(const KApiInstance&) = delete;
        KApiInstance& operator =(const KApiInstance&) = delete;

        R              res_ = invalid();
        multi_map_type multiMap_;
    };
}

#endif // _OT_API_INSTANCE_H_