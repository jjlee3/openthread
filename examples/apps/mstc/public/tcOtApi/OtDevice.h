#pragma once

#include <cguid.h>
#include <map>
#include "openthread.h"
#include <tcOtApi/OtMemory.h>
#include <tcOtApi/OtInterface.h>
#include <tcOtApi/OtThreadNet.h>
#include <base/Exception.h>

namespace mstc
{
namespace ot
{
class KDevice : public KMemory<otInstance>
{
public:
    EXCEPT_BASE   (Failure);
    EXCEPT_DERIVED(OtInstanceInitFailure, Failure);

public:
    using base_t = KMemory<otInstance>;
    using base_t::base_t;

#ifdef OTDLL
    KDevice(otApiInstance*, const GUID* devGuid);
#else
#ifdef OPENTHREAD_MULTIPLE_INSTANCE
    KDevice(void* instanceBuffer, size_t* bufferSize);
#endif // OPENTHREAD_MULTIPLE_INSTANCE
#endif // OTDLL
    KDevice();
    KDevice(KDevice&& rhs) noexcept;
    explicit KDevice(R&& res) noexcept;

    ~KDevice();

    KDevice& operator =(R&& res) noexcept;
    KDevice& operator =(KDevice&& rhs) noexcept;

    void attach(R&& res) noexcept;
    R detach() noexcept;
    void close() noexcept;

    const GUID& GetGuid() const;
    GUID GetDeviceGuid() const;

    ThreadError SetStateChangeCallback(otStateChangedCallback,
        void* context);
    void RemoveStateChangeCallback(otStateChangedCallback,
        void* context);

    const wchar_t* RoleToStringW() const;
    const char* RoleToStringA() const;
    int MacFormatW(wchar_t* buffer, size_t cntBuffer) const;
    int MacFormatA(char* buffer, size_t cntBuffer) const;
    int Ipv6FormatW(wchar_t* buffer, size_t cntBuffer) const;
    int Ipv6FormatA(char* buffer, size_t cntBuffer) const;

    ThreadError InterfaceUp();
    ThreadError InterfaceDown();
    ThreadError ThreadStart();
    ThreadError ThreadStop();

protected:
    using map_t = std::multimap<otStateChangedCallback, void*>;

    GUID       guid_ = GUID_NULL;
    map_t      multiMap_;
    KInterface interface_;
    KThreadNet threadNet_;
}; // KDevice
} // ot
} // mstc