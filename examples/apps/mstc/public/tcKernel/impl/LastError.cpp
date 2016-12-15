#include "pch/pch.h"
#include <assert.h>
#include <base/Scope.h>
#include <base/Utf.h>
#include <tcKernel/Lang.h>
#include <tcKernel/LastError.h>

mstc::tckernel::LastError::LastError(
    DWORD lastErr) : lastErr_{ lastErr }
{
    void* msgBuf = nullptr;

    auto freeBuf = mstc::base::scope([&msgBuf]()
    {
        if (msgBuf) { ::LocalFree(static_cast<HLOCAL>(msgBuf)); }
    });
    
    toMessage(lastErr, mstc::tckernel::Lang::getId(), &msgBuf);

    char buf[256];

    if (msgBuf)
    {
        sprintf_s(buf, "Last Error 0x%08X(%d) %s", lastErr, lastErr,
            mstc::base::Utf::utf16to8(reinterpret_cast<const wchar_t*>(msgBuf)).c_str());
    }
    else
    {
        sprintf_s(buf, "Last Error 0x%08X(%d) <unknown>", lastErr, lastErr);
    }

    error_ = buf;
}

DWORD
mstc::tckernel::LastError::toMessage(
    DWORD lastErr, DWORD langId, void** msgBuf)
{
    DWORD fmtMsgErr = ERROR_SUCCESS;

    auto size = formatMessage(lastErr, langId, msgBuf);
    if (!size) { fmtMsgErr = ::GetLastError(); }

    if (size)
    {
    }
    else if (fmtMsgErr != ERROR_RESOURCE_LANG_NOT_FOUND)
    {
    }
    else if (::GetSystemDefaultLangID() == langId)
    {
        assert(*msgBuf == nullptr);
        langId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
        size = formatMessage(lastErr, langId, msgBuf);
        fmtMsgErr = size ? ERROR_SUCCESS : ::GetLastError();
    }
    else
    {
    }

    if (size)
    {
    }
    else if (fmtMsgErr != ERROR_RESOURCE_LANG_NOT_FOUND)
    {
    }
    else
    {
        assert(*msgBuf == nullptr);
        langId = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
        size = formatMessage(lastErr, langId, msgBuf);
        fmtMsgErr = size ? ERROR_SUCCESS : ::GetLastError();
    }

    return size;
}

DWORD
mstc::tckernel::LastError::formatMessage(
    DWORD lastErr, DWORD langId, void** msgBuf)
{
    if (lastErr < 12000)
    {
        return formatMessage(nullptr, lastErr, langId, msgBuf);
    }
    else if (lastErr <= 12999)
    {
        return formatMessage(::GetModuleHandleW(L"wininet.dll"), lastErr, langId, msgBuf);
    }
    else
    {
        return formatMessage(nullptr, lastErr, langId, msgBuf);
    }
}