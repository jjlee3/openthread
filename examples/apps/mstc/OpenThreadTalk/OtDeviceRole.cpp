#include "pch.h"
#include "Types.h"
#include "OtDeviceRole.h"

using namespace ot;

const wchar_t*
DeviceRole::ToStringW(
    otDeviceRole devRole)
{
    switch (devRole)
    {
    case kDeviceRoleOffline:  return L"Offline";
    case kDeviceRoleDisabled: return L"Disabled";
    case kDeviceRoleDetached: return L"Disconnected";
    case kDeviceRoleChild:    return L"Connected - Child";
    case kDeviceRoleRouter:   return L"Connected - Router";
    case kDeviceRoleLeader:   return L"Connected - Leader";
    }

    throw ref new OpenThreadTalk::FailureException(
        L"DeviceRole::ToStringW unknown role");
}

const char*
DeviceRole::ToStringA(
    otDeviceRole devRole)
{
    switch (devRole)
    {
    case kDeviceRoleOffline:  return "Offline";
    case kDeviceRoleDisabled: return "Disabled";
    case kDeviceRoleDetached: return "Disconnected";
    case kDeviceRoleChild:    return "Connected - Child";
    case kDeviceRoleRouter:   return "Connected - Router";
    case kDeviceRoleLeader:   return "Connected - Leader";
    }

    throw ref new OpenThreadTalk::FailureException(
        L"DeviceRole::ToStringW unknown role");
}
