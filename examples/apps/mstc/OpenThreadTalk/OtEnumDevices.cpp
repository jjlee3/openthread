#include "pch.h"
#include "Types.h"
#include "OtApiInstance.h"
#include "OtEnumDevices.h"

using namespace ot;

EnumDevices::EnumDevices(
    const ApiInstance& apiInstance) :
    EnumDevices{ apiInstance.get() }
{
}

EnumDevices::EnumDevices(
    otApiInstance* pApiInstance) :
    Memory{ ::otEnumerateDevices(pApiInstance) }
{
    if (res_ == invalid())
    {
        throw ref new OpenThreadTalk::InvalidArgumentException(
            L"EnumDevices::EnumDevices otEnumerateDevices Failure");
    }
}
