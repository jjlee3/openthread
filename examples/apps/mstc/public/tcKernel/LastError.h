#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

namespace mstc
{
namespace tckernel
{
class LastError
{
public:
    explicit LastError(DWORD lastErr);

    operator DWORD() const noexcept { return lastErr_; }

    const std::string& error() const noexcept { return error_; }

    static DWORD toMessage(DWORD lastErr, DWORD langId, void** msgBuf);

    static DWORD formatMessage(DWORD lastErr, DWORD langId, void** msgBuf);

    static DWORD formatMessage(void* source, DWORD lastErr,
        DWORD langId, void** msgBuf)
    {
        return ::FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            source, lastErr, langId,
            reinterpret_cast<wchar_t*>(msgBuf), 0, nullptr);
    }

    DWORD       lastErr_;
    std::string error_;
}; // LasstError

#define EXCEPT_LASTERR_BASE(LeExceptCLASS, ExceptBASE)                      \
    struct LeExceptCLASS : public ExceptBASE                                \
    {                                                                       \
        using base_t = ExceptBASE;                                          \
    public:                                                                 \
        explicit LeExceptCLASS(DWORD lastErr) :                             \
            base_t(std::string(), typeid(LeExceptCLASS).name()),            \
            lastErr_{lastErr}                                               \
        {                                                                   \
            *this << mstc::base::throw_class_args(std::to_string(lastErr)); \
            *this << mstc::base::throw_error(lastErr_.error());             \
        }                                                                   \
                                                                            \
        SET_OPERATOR(LeExceptCLASS)                                         \
                                                                            \
        operator DWORD() const noexcept { return lastErr_; }                \
                                                                            \
    protected:                                                              \
        explicit LeExceptCLASS(DWORD lastErr, const char* throwCls) :       \
            base_t(std::string(), throwCls),                                \
            lastErr_{lastErr}                                               \
        {                                                                   \
            *this << mstc::base::throw_class_args(std::to_string(lastErr)); \
            *this << mstc::base::throw_error(lastErr_.error());             \
        }                                                                   \
                                                                            \
        mstc::tckernel::LastError lastErr_;                                 \
    };

#define EXCEPT_LASTERR_DERIVED(LeExceptCLASS, ExceptBASE)                   \
    struct LeExceptCLASS : public ExceptBASE                                \
    {                                                                       \
        using base_t = ExceptBASE;                                          \
    public:                                                                 \
        explicit LeExceptCLASS(DWORD lastErr) :                             \
            base_t(lastErr, typeid(LeExceptCLASS).name()) {}                \
                                                                            \
        SET_OPERATOR(LeExceptCLASS)                                         \
                                                                            \
    };

} // tckernel
} // mstc