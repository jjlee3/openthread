#pragma once

#include <map>
#include "openthread.h"
#include <base/Exception.h>

namespace mstc
{
namespace ot
{
class KApiInstance
{
public:
    EXCEPT_BASE   (Failure);
    EXCEPT_DERIVED(OtApiInitFailure, Failure);

public:
    // resourec type
#ifdef OTDLL
    using R = otApiInstance*;

    using otDeviceAvailabilityChangedCallback = ::otDeviceAvailabilityChangedCallback;
#else
    using R = void*;

    // copy declaration in openthread.h under ifdef OTDLL
    typedef void (OTCALL *otDeviceAvailabilityChangedCallback)(
        bool aAdded, const GUID *aDeviceGuid, void *aContext);
#endif

    inline static constexpr R invalid() { return nullptr; }

#ifdef OTDLL
    KApiInstance();
    KApiInstance(KApiInstance&& rhs) noexcept;
    explicit KApiInstance(R&& res) noexcept;
    ~KApiInstance();

    KApiInstance& operator =(R&& res) noexcept;
    KApiInstance& operator =(KApiInstance&& rhs) noexcept;
#endif

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
    using map_t = std::multimap<otDeviceAvailabilityChangedCallback, void*>;

    R     res_;
#ifdef OTDLL
    map_t multiMap_;
#endif
}; // KApiInstance
} // ot
} // mstc