#include "pch.h"
#include "GuidHelper.h"

using namespace OpenThreadTalk;

int
GuidHelper::ToStringA(
    char*  buffer,
    size_t sizeBuffer,
    const  GUID& guid)
{
    return sprintf_s(buffer, sizeBuffer, GUID_FORMATA, GUID_ARG(guid));
}

int
GuidHelper::ToStringW(
    wchar_t* buffer,
    size_t   sizeBuffer,
    const    GUID& guid)
{
    return swprintf_s(buffer, sizeBuffer, GUID_FORMATW, GUID_ARG(guid));
}
