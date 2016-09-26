/*
 *  Copyright (c) 2016, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include "precomp.h"
#include "otNodeApi.tmh"

#define DEBUG_PING 1

#define GUID_FORMAT "{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}"
#define GUID_ARG(guid) guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]

typedef DWORD (*fp_otvmpOpenHandle)(HANDLE* phandle);
typedef VOID  (*fp_otvmpCloseHandle)(HANDLE handle);
typedef DWORD (*fp_otvmpAddVirtualBus)(HANDLE handle, ULONG* pBusNumber, ULONG* pIfIndex);
typedef DWORD (*fp_otvmpRemoveVirtualBus)(HANDLE handle, ULONG BusNumber);
typedef DWORD (*fp_otvmpSetAdapterTopologyGuid)(HANDLE handle, DWORD BusNumber, const GUID* pTopologyGuid);

fp_otvmpOpenHandle              otvmpOpenHandle = nullptr;
fp_otvmpCloseHandle             otvmpCloseHandle = nullptr;
fp_otvmpAddVirtualBus           otvmpAddVirtualBus = nullptr;
fp_otvmpRemoveVirtualBus        otvmpRemoveVirtualBus = nullptr;
fp_otvmpSetAdapterTopologyGuid  otvmpSetAdapterTopologyGuid = nullptr;

HMODULE gVmpModule = nullptr;
HANDLE  gVmpHandle = nullptr;

ULONG gNextBusNumber = 1;
GUID gTopologyGuid = {0};

volatile LONG gNumberOfInterfaces = 0;

otApiInstance *gApiInstance = nullptr;

otApiInstance* GetApiInstance()
{
    if (gApiInstance == nullptr)
    { 
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0)
        {
            printf("WSAStartup failed!\r\n");
            return nullptr;
        }

        gApiInstance = otApiInit();
        if (gApiInstance == nullptr)
        {
            printf("otApiInit failed!\r\n");
            return nullptr;
        }

        gVmpModule = LoadLibrary(TEXT("otvmpapi.dll"));
        if (gVmpModule == nullptr)
        {
            printf("LoadLibrary(\"otvmpapi\") failed!\r\n");
            return nullptr;
        }

        otvmpOpenHandle             = (fp_otvmpOpenHandle)GetProcAddress(gVmpModule, "otvmpOpenHandle");
        otvmpCloseHandle            = (fp_otvmpCloseHandle)GetProcAddress(gVmpModule, "otvmpCloseHandle");
        otvmpAddVirtualBus          = (fp_otvmpAddVirtualBus)GetProcAddress(gVmpModule, "otvmpAddVirtualBus");
        otvmpRemoveVirtualBus       = (fp_otvmpRemoveVirtualBus)GetProcAddress(gVmpModule, "otvmpRemoveVirtualBus");
        otvmpSetAdapterTopologyGuid = (fp_otvmpSetAdapterTopologyGuid)GetProcAddress(gVmpModule, "otvmpSetAdapterTopologyGuid");

        if (otvmpOpenHandle == nullptr) printf("otvmpOpenHandle is null!\r\n");
        if (otvmpCloseHandle == nullptr) printf("otvmpCloseHandle is null!\r\n");
        if (otvmpAddVirtualBus == nullptr) printf("otvmpAddVirtualBus is null!\r\n");
        if (otvmpRemoveVirtualBus == nullptr) printf("otvmpRemoveVirtualBus is null!\r\n");
        if (otvmpSetAdapterTopologyGuid == nullptr) printf("otvmpSetAdapterTopologyGuid is null!\r\n");

        (VOID)otvmpOpenHandle(&gVmpHandle);
        if (gVmpHandle == nullptr)
        {
            printf("otvmpOpenHandle failed!\r\n");
            return nullptr;
        }

        auto status = UuidCreate(&gTopologyGuid);
        if (status != NO_ERROR)
        {
            printf("UuidCreate failed, 0x%x!\r\n", status);
            return nullptr;
        }

        auto offset = getenv("INSTANCE");
        if (offset)
        {
            gNextBusNumber = (atoi(offset) * 32) % 1000 + 1;
        }
        else
        {
            srand(gTopologyGuid.Data1);
            gNextBusNumber = rand() % 1000 + 1;
        }

        printf("New topology created\r\n" GUID_FORMAT " [%d]\r\n\r\n", GUID_ARG(gTopologyGuid), gNextBusNumber);
    }

    return gApiInstance;
}

void Unload()
{
    if (gNumberOfInterfaces != 0)
    {
        printf("Unloaded with %d outstanding nodes!\r\n", gNumberOfInterfaces);
    }

    if (gApiInstance)
    {
        if (gVmpHandle != nullptr)
        {
            otvmpCloseHandle(gVmpHandle);
            gVmpHandle = nullptr;
        }

        if (gVmpModule != nullptr)
        {
            CloseHandle(gVmpModule);
            gVmpModule = nullptr;
        }

        otApiFinalize(gApiInstance);
        gApiInstance = nullptr;

        WSACleanup();

        printf("Topology destroyed\r\n");
    }
}

int Hex2Bin(const char *aHex, uint8_t *aBin, uint16_t aBinLength)
{
    size_t hexLength = strlen(aHex);
    const char *hexEnd = aHex + hexLength;
    uint8_t *cur = aBin;
    uint8_t numChars = hexLength & 1;
    uint8_t byte = 0;

    if ((hexLength + 1) / 2 > aBinLength)
    {
        return -1;
    }

    while (aHex < hexEnd)
    {
        if ('A' <= *aHex && *aHex <= 'F')
        {
            byte |= 10 + (*aHex - 'A');
        }
        else if ('a' <= *aHex && *aHex <= 'f')
        {
            byte |= 10 + (*aHex - 'a');
        }
        else if ('0' <= *aHex && *aHex <= '9')
        {
            byte |= *aHex - '0';
        }
        else
        {
            return -1;
        }

        aHex++;
        numChars++;

        if (numChars >= 2)
        {
            numChars = 0;
            *cur++ = byte;
            byte = 0;
        }
        else
        {
            byte <<= 4;
        }
    }

    return static_cast<int>(cur - aBin);
}

typedef struct otPingHandler
{
    otNode*         mParentNode;
    bool            mActive;
    otIp6Address    mAddress;
    SOCKET          mSocket;
    CHAR            mRecvBuffer[1500];
    WSAOVERLAPPED   mOverlapped;
    WSABUF          mWSARecvBuffer;
    DWORD           mNumBytesReceived;
    SOCKADDR_IN6    mSourceAddr6;
    int             mSourceAddr6Len;

} otPingHandler;

typedef struct otNode
{
    uint32_t                mId;
    DWORD                   mBusIndex;
    otInstance*             mInstance;
    HANDLE                  mEnergyScanEvent;
    HANDLE                  mPanIdConflictEvent;
    vector<otPingHandler*>  mPingHandlers;
} otNode;

const char* otDeviceRoleToString(otDeviceRole role)
{
    switch (role)
    {
    case kDeviceRoleOffline:  return "offline";
    case kDeviceRoleDisabled: return "disabled";
    case kDeviceRoleDetached: return "detached";
    case kDeviceRoleChild:    return "child";
    case kDeviceRoleRouter:   return "router";
    case kDeviceRoleLeader:   return "leader";
    default:                  return "invalid";
    }
}

const USHORT CertificationPingPort = htons(12345);

const IN6_ADDR LinkLocalAllNodesAddress    = { { 0xFF, 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x01 } };
const IN6_ADDR LinkLocalAllRoutersAddress  = { { 0xFF, 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x02 } };
const IN6_ADDR RealmLocalAllNodesAddress   = { { 0xFF, 0x03, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x01 } };
const IN6_ADDR RealmLocalAllRoutersAddress = { { 0xFF, 0x03, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x02 } };

void
CALLBACK 
PingHandlerRecvCallback(
    _In_ DWORD dwError,
    _In_ DWORD cbTransferred,
    _In_ LPWSAOVERLAPPED lpOverlapped,
    _In_ DWORD dwFlags
    )
{
    otPingHandler *aPingHandler = (otPingHandler*)lpOverlapped->hEvent;
    if (dwError != ERROR_SUCCESS) return;

    int result;

    // Make sure it didn't come from our address
    if (memcmp(&aPingHandler->mSourceAddr6.sin6_addr, &aPingHandler->mAddress, sizeof(IN6_ADDR)) != 0)
    {
        CHAR szIpAddress[46] = { 0 };
        RtlIpv6AddressToStringA(&aPingHandler->mSourceAddr6.sin6_addr, szIpAddress);
        
#if DEBUG_PING
        printf("received ping (%d bytes) from %s\r\n", cbTransferred, szIpAddress);
#endif

        bool shouldReply = true;

        // TODO - Fix this hack...
        auto RecvDest = (const otIp6Address*)aPingHandler->mRecvBuffer;
        if (memcmp(RecvDest, &LinkLocalAllRoutersAddress, sizeof(IN6_ADDR)) == 0 ||
            memcmp(RecvDest, &RealmLocalAllNodesAddress, sizeof(IN6_ADDR)) == 0)
        {
            auto Role = otGetDeviceRole(aPingHandler->mParentNode->mInstance);
            if (Role != kDeviceRoleLeader && Role != kDeviceRoleRouter)
                shouldReply = false;
        }

        if (shouldReply)
        {
            // Send the received data back
            result = 
                sendto(
                    aPingHandler->mSocket, 
                    aPingHandler->mRecvBuffer, cbTransferred, 0, 
                    (SOCKADDR*)&aPingHandler->mSourceAddr6, aPingHandler->mSourceAddr6Len
                    );
            if (result == SOCKET_ERROR)
            {
                printf("sendto failed, 0x%x\r\n", WSAGetLastError());
            }
        }
    }

    // Post another recv
    dwFlags = MSG_PARTIAL;
    aPingHandler->mSourceAddr6Len = sizeof(aPingHandler->mSourceAddr6);
    result = 
        WSARecvFrom(
            aPingHandler->mSocket, 
            &aPingHandler->mWSARecvBuffer, 1, &aPingHandler->mNumBytesReceived, &dwFlags, 
            (SOCKADDR*)&aPingHandler->mSourceAddr6, &aPingHandler->mSourceAddr6Len, 
            &aPingHandler->mOverlapped, PingHandlerRecvCallback
            );
    if (result != SOCKET_ERROR)
    {
        PingHandlerRecvCallback(NO_ERROR, aPingHandler->mNumBytesReceived, &aPingHandler->mOverlapped, dwFlags);
    }
    else
    {
        result = WSAGetLastError();
        if (result != WSA_IO_PENDING)
        {
            printf("WSARecvFrom failed, 0x%x\r\n", result);
        }
    }
}

void AddPingHandler(otNode *aNode, const otIp6Address *aAddress)
{
    otPingHandler *aPingHandler = new otPingHandler();
    aPingHandler->mParentNode = aNode;
    aPingHandler->mAddress = *aAddress;
    aPingHandler->mSocket = INVALID_SOCKET;
    aPingHandler->mOverlapped.hEvent = aPingHandler;
    aPingHandler->mWSARecvBuffer = { 1500, aPingHandler->mRecvBuffer };
    aPingHandler->mActive = true;
    
    SOCKADDR_IN6 addr6 = { 0 };
    addr6.sin6_family = AF_INET6;
    addr6.sin6_port = CertificationPingPort;
    memcpy(&addr6.sin6_addr, aAddress, sizeof(IN6_ADDR));
    
    CHAR szIpAddress[46] = { 0 };
    RtlIpv6AddressToStringA(&addr6.sin6_addr, szIpAddress);

#if DEBUG_PING
    printf("%d: starting ping handler for %s\r\n", aNode->mId, szIpAddress);
#endif

    // Put the current thead in the correct compartment
    bool RevertCompartmentOnExit = false;
    ULONG OriginalCompartmentID = GetCurrentThreadCompartmentId();
    if (OriginalCompartmentID != otGetCompartmentId(aNode->mInstance))
    {
        DWORD dwError = ERROR_SUCCESS;
        if ((dwError = SetCurrentThreadCompartmentId(otGetCompartmentId(aNode->mInstance))) != ERROR_SUCCESS)
        {
            printf("SetCurrentThreadCompartmentId failed, 0x%x\r\n", dwError);
        }
        RevertCompartmentOnExit = true;
    }

    int result;
    DWORD Flag = FALSE;
    IPV6_MREQ MCReg;
    MCReg.ipv6mr_interface = otGetDeviceIfIndex(aNode->mInstance);
    
    // Create the socket
    aPingHandler->mSocket = WSASocket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (aPingHandler->mSocket == INVALID_SOCKET)
    {
        printf("WSASocket failed, 0x%x\r\n", WSAGetLastError());
        goto exit;
    }

    // Bind the socket to the address
    result = bind(aPingHandler->mSocket, (sockaddr*)&addr6, sizeof(addr6));
    if (result == SOCKET_ERROR)
    {
        printf("bind failed, 0x%x\r\n", WSAGetLastError());
        goto exit;
    }
    
    // Block our own sends from getting called as receives
    result = setsockopt(aPingHandler->mSocket, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, (char *)&Flag, sizeof(Flag));
    if (result == SOCKET_ERROR)
    {
        printf("setsockopt (IPV6_MULTICAST_LOOP) failed, 0x%x\r\n", WSAGetLastError());
        goto exit;
    }

    // Bind to the multicast addresses
    if (IN6_IS_ADDR_LINKLOCAL(&addr6.sin6_addr))
    {
        // All nodes address
        MCReg.ipv6mr_multiaddr = LinkLocalAllNodesAddress;
        result = setsockopt(aPingHandler->mSocket, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, (char *)&MCReg, sizeof(MCReg));
        if (result == SOCKET_ERROR)
        {
            printf("setsockopt (IPV6_ADD_MEMBERSHIP) failed, 0x%x\r\n", WSAGetLastError());
            goto exit;
        }

        // All routers address
        MCReg.ipv6mr_multiaddr = LinkLocalAllRoutersAddress;
        result = setsockopt(aPingHandler->mSocket, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, (char *)&MCReg, sizeof(MCReg));
        if (result == SOCKET_ERROR)
        {
            printf("setsockopt (IPV6_ADD_MEMBERSHIP) failed, 0x%x\r\n", WSAGetLastError());
            goto exit;
        }
    }
    else
    {
        // All nodes address
        MCReg.ipv6mr_multiaddr = RealmLocalAllNodesAddress;
        result = setsockopt(aPingHandler->mSocket, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, (char *)&MCReg, sizeof(MCReg));
        if (result == SOCKET_ERROR)
        {
            printf("setsockopt (IPV6_ADD_MEMBERSHIP) failed, 0x%x\r\n", WSAGetLastError());
            goto exit;
        }
        
        // All routers address
        MCReg.ipv6mr_multiaddr = RealmLocalAllRoutersAddress;
        result = setsockopt(aPingHandler->mSocket, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, (char *)&MCReg, sizeof(MCReg));
        if (result == SOCKET_ERROR)
        {
            printf("setsockopt (IPV6_ADD_MEMBERSHIP) failed, 0x%x\r\n", WSAGetLastError());
            goto exit;
        }
    }

    // Start the receive
    Flag = MSG_PARTIAL;
    aPingHandler->mSourceAddr6Len = sizeof(aPingHandler->mSourceAddr6);
    result = 
        WSARecvFrom(
            aPingHandler->mSocket, 
            &aPingHandler->mWSARecvBuffer, 1, &aPingHandler->mNumBytesReceived, &Flag, 
            (SOCKADDR*)&aPingHandler->mSourceAddr6, &aPingHandler->mSourceAddr6Len, 
            &aPingHandler->mOverlapped, PingHandlerRecvCallback
            );
    if (result != SOCKET_ERROR)
    {
        PingHandlerRecvCallback(NO_ERROR, aPingHandler->mNumBytesReceived, &aPingHandler->mOverlapped, Flag);
    }
    else
    {
        result = WSAGetLastError();
        if (result != WSA_IO_PENDING)
        {
            printf("WSARecvFrom failed, 0x%x\r\n", result);
            goto exit;
        }
    }

    aNode->mPingHandlers.push_back(aPingHandler);
    aPingHandler = nullptr;

exit:
    
    // Revert the comparment if necessary
    if (RevertCompartmentOnExit)
    {
        (VOID)SetCurrentThreadCompartmentId(OriginalCompartmentID);
    }

    // Clean up ping handler if necessary
    if (aPingHandler)
    {
        if (aPingHandler->mSocket != INVALID_SOCKET) 
            closesocket(aPingHandler->mSocket);
        delete aPingHandler;
    }
}

void HandleAddressChanges(otNode *aNode)
{
    auto addrs = otGetUnicastAddresses(aNode->mInstance);
        
    // Invalidate all handlers
    for (ULONG i = 0; i < aNode->mPingHandlers.size(); i++)
        aNode->mPingHandlers[i]->mActive = false;

    // Search for matches
    for (auto addr = addrs; addr; addr = addr->mNext)
    {
        bool found = false;
        for (ULONG i = 0; i < aNode->mPingHandlers.size(); i++)
            if (!aNode->mPingHandlers[i]->mActive &&
                memcmp(&addr->mAddress, &aNode->mPingHandlers[i]->mAddress, sizeof(otIp6Address)) == 0)
            {
                found = true;
                aNode->mPingHandlers[i]->mActive = true;
                break;
            }
        if (!found) AddPingHandler(aNode, &addr->mAddress);
    }
        
    // Release all left over handlers
    for (int i = aNode->mPingHandlers.size() - 1; i >= 0; i--)
        if (aNode->mPingHandlers[i]->mActive == false)
        {
            auto aPingHandler = aNode->mPingHandlers[i];
            aNode->mPingHandlers.erase(aNode->mPingHandlers.begin() + i);
                
            closesocket(aPingHandler->mSocket);
            delete aPingHandler;
        }

    if (addrs) otFreeMemory(addrs);
}

void OTCALL otNodeStateChangedCallback(uint32_t aFlags, void *aContext)
{
    otNode* aNode = (otNode*)aContext;

    if ((aFlags & OT_NET_ROLE) != 0)
    {
        auto Role = otGetDeviceRole(aNode->mInstance);
        printf("%d: new role: %s\r\n", aNode->mId, otDeviceRoleToString(Role));
    }

    if ((aFlags & OT_IP6_ADDRESS_ADDED) != 0 || (aFlags & OT_IP6_ADDRESS_REMOVED) != 0)
    {
        HandleAddressChanges(aNode);
    }
}

OTNODEAPI int32_t OTCALL otNodeLog(const char *aMessage)
{
    LogInfo(OT_API, "%s", aMessage);
    return 0;
}

OTNODEAPI otNode* OTCALL otNodeInit(uint32_t id)
{
    otLogFuncEntry();

    auto ApiInstance = GetApiInstance();
    if (ApiInstance == nullptr)
    {
        printf("GetApiInstance failed!\r\n");
        otLogFuncExitMsg("GetApiInstance failed");
        return nullptr;
    }

    DWORD newBusIndex;
    NET_IFINDEX ifIndex = {};
    
    DWORD dwError;
    DWORD tries = 0;
    while (tries < 1000)
    {
        newBusIndex = (gNextBusNumber + tries) % 1000;
        if (newBusIndex == 0) newBusIndex++;

        dwError = otvmpAddVirtualBus(gVmpHandle, &newBusIndex, &ifIndex);
        if (dwError == ERROR_SUCCESS)
        {
            gNextBusNumber = newBusIndex + 1;
            break;
        }
        else if (dwError == ERROR_INVALID_PARAMETER || dwError == ERROR_FILE_NOT_FOUND)
        {
            tries++;
        }
        else
        {
            printf("otvmpAddVirtualBus failed, 0x%x!\r\n", dwError);
            otLogFuncExitMsg("otvmpAddVirtualBus failed");
            return nullptr;
        }
    }

    if (tries == 1000)
    {
        printf("otvmpAddVirtualBus failed to find an empty bus!\r\n");
        otLogFuncExitMsg("otvmpAddVirtualBus failed to find an empty bus");
        return nullptr;
    }

    if ((dwError = otvmpSetAdapterTopologyGuid(gVmpHandle, newBusIndex, &gTopologyGuid)) != ERROR_SUCCESS)
    {
        printf("otvmpSetAdapterTopologyGuid failed, 0x%x!\r\n", dwError);
        otLogFuncExitMsg("otvmpSetAdapterTopologyGuid failed");
        otvmpRemoveVirtualBus(gVmpHandle, newBusIndex);
        return nullptr;
    }

    NET_LUID ifLuid = {};
    if (ERROR_SUCCESS != ConvertInterfaceIndexToLuid(ifIndex, &ifLuid))
    {
        printf("ConvertInterfaceIndexToLuid(%u) failed!\r\n", ifIndex);
        otLogFuncExitMsg("ConvertInterfaceIndexToLuid failed");
        otvmpRemoveVirtualBus(gVmpHandle, newBusIndex);
        return nullptr;
    }

    GUID ifGuid = {};
    if (ERROR_SUCCESS != ConvertInterfaceLuidToGuid(&ifLuid, &ifGuid))
    {
        printf("ConvertInterfaceLuidToGuid failed!\r\n");
        otLogFuncExitMsg("ConvertInterfaceLuidToGuid failed");
        otvmpRemoveVirtualBus(gVmpHandle, newBusIndex);
        return nullptr;
    }
    
    auto instance = otInstanceInit(ApiInstance, &ifGuid);
    if (instance == nullptr)
    {
        printf("otInstanceInit failed!\r\n");
        otLogFuncExitMsg("otInstanceInit failed");
        otvmpRemoveVirtualBus(gVmpHandle, newBusIndex);
        return nullptr;
    }

    InterlockedIncrement(&gNumberOfInterfaces);

    GUID DeviceGuid = otGetDeviceGuid(instance);
    uint32_t Compartment = otGetCompartmentId(instance);

    otNode *node = new otNode();
    printf("%d: New Device " GUID_FORMAT " in compartment %d\r\n", id, GUID_ARG(DeviceGuid), Compartment);

    node->mId = id;
    node->mBusIndex = newBusIndex;
    node->mInstance = instance;

    node->mEnergyScanEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    node->mPanIdConflictEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

    otSetStateChangedCallback(instance, otNodeStateChangedCallback, node);

    HandleAddressChanges(node);

    otLogFuncExitMsg("success");

    return node;
}

OTNODEAPI int32_t OTCALL otNodeFinalize(otNode* aNode)
{
    otLogFuncEntry();
    if (aNode != nullptr)
    {
        printf("%d: Removing Device\r\n", aNode->mId);

        CloseHandle(aNode->mPanIdConflictEvent);
        CloseHandle(aNode->mEnergyScanEvent);
        otSetStateChangedCallback(aNode->mInstance, nullptr, nullptr);
        otFreeMemory(aNode->mInstance);
        aNode->mInstance = nullptr;
        HandleAddressChanges(aNode);
        otvmpRemoveVirtualBus(gVmpHandle, aNode->mBusIndex);
        delete aNode;
        
        if (0 == InterlockedDecrement(&gNumberOfInterfaces))
        {
            Unload();
        }
    }
    otLogFuncExit();
    return 0;
}

OTNODEAPI int32_t OTCALL otNodeSetMode(otNode* aNode, const char *aMode)
{
    otLogFuncEntryMsg("[%d] %s", aNode->mId, aMode);
    printf("%d: mode %s\r\n", aNode->mId, aMode);

    otLinkModeConfig linkMode = {0};

    const char *index = aMode;
    while (*index)
    {
        switch (*index)
        {
        case 'r':
            linkMode.mRxOnWhenIdle = true;
            break;
        case 's':
            linkMode.mSecureDataRequests = true;
            break;
        case 'd':
            linkMode.mDeviceType = true;
            break;
        case 'n':
            linkMode.mNetworkData = true;
            break;
        }

        index++;
    }

    auto result = otSetLinkMode(aNode->mInstance, linkMode);
    
    otLogFuncExit();
    return result;
}

OTNODEAPI int32_t OTCALL otNodeInterfaceUp(otNode* aNode)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: ifconfig up\r\n", aNode->mId);

    auto error = otInterfaceUp(aNode->mInstance);
    
    otLogFuncExit();
    return error;
}

OTNODEAPI int32_t OTCALL otNodeInterfaceDown(otNode* aNode)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: ifconfig down\r\n", aNode->mId);

    (void)otInterfaceDown(aNode->mInstance);
    
    otLogFuncExit();
    return 0;
}

OTNODEAPI int32_t OTCALL otNodeThreadStart(otNode* aNode)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: thread start\r\n", aNode->mId);

    auto error = otThreadStart(aNode->mInstance);
    
    otLogFuncExit();
    return error;
}

OTNODEAPI int32_t OTCALL otNodeThreadStop(otNode* aNode)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: thread stop\r\n", aNode->mId);

    (void)otThreadStop(aNode->mInstance);
    
    otLogFuncExit();
    return 0;
}

OTNODEAPI int32_t OTCALL otNodeCommissionerStart(otNode* aNode, const char *aPSKd, const char *aProvisioningUrl)
{
    otLogFuncEntryMsg("[%d] %s %s", aNode->mId, aPSKd, aProvisioningUrl);
    printf("%d: commissioner start %s %s\r\n", aNode->mId, aPSKd, aProvisioningUrl);

    auto error = otCommissionerStart(aNode->mInstance, aPSKd, aProvisioningUrl);
    
    otLogFuncExit();
    return error;
}

OTNODEAPI int32_t OTCALL otNodeCommissionerStop(otNode* aNode)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: commissioner stop\r\n", aNode->mId);

    (void)otCommissionerStop(aNode->mInstance);
    
    otLogFuncExit();
    return 0;
}

OTNODEAPI int32_t OTCALL otNodeJoinerStart(otNode* aNode, const char *aPSKd, const char *aProvisioningUrl)
{
    otLogFuncEntryMsg("[%d] %s %s", aNode->mId, aPSKd, aProvisioningUrl);
    printf("%d: joiner start %s %s\r\n", aNode->mId, aPSKd, aProvisioningUrl);

    auto error = otJoinerStart(aNode->mInstance, aPSKd, aProvisioningUrl);
    
    otLogFuncExit();
    return error;
}

OTNODEAPI int32_t OTCALL otNodeJoinerStop(otNode* aNode)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: joiner stop\r\n", aNode->mId);

    (void)otJoinerStop(aNode->mInstance);
    
    otLogFuncExit();
    return 0;
}

OTNODEAPI int32_t OTCALL otNodeClearWhitelist(otNode* aNode)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: whitelist clear\r\n", aNode->mId);

    otClearMacWhitelist(aNode->mInstance);
    otLogFuncExit();
    return 0;
}

OTNODEAPI int32_t OTCALL otNodeEnableWhitelist(otNode* aNode)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: whitelist enable\r\n", aNode->mId);

    otEnableMacWhitelist(aNode->mInstance);
    otLogFuncExit();
    return 0;
}

OTNODEAPI int32_t OTCALL otNodeDisableWhitelist(otNode* aNode)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: whitelist disable\r\n", aNode->mId);

    otDisableMacWhitelist(aNode->mInstance);
    otLogFuncExit();
    return 0;
}

OTNODEAPI int32_t OTCALL otNodeAddWhitelist(otNode* aNode, const char *aExtAddr, int8_t aRssi)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    if (aRssi == 0)
        printf("%d: whitelist add %s\r\n", aNode->mId, aExtAddr);
    else printf("%d: whitelist add %s %d\r\n", aNode->mId, aExtAddr, aRssi);

    uint8_t extAddr[8];
    if (Hex2Bin(aExtAddr, extAddr, sizeof(extAddr)) != sizeof(extAddr))
        return kThreadError_Parse;

    ThreadError error;
    if (aRssi == 0)
    {
        error = otAddMacWhitelist(aNode->mInstance, extAddr);
    }
    else
    {
        error = otAddMacWhitelistRssi(aNode->mInstance, extAddr, aRssi);
    }
    otLogFuncExit();
    return error;
}

OTNODEAPI int32_t OTCALL otNodeRemoveWhitelist(otNode* aNode, const char *aExtAddr)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: whitelist remove %s\r\n", aNode->mId, aExtAddr);

    uint8_t extAddr[8];
    if (Hex2Bin(aExtAddr, extAddr, sizeof(extAddr)) != sizeof(extAddr))
        return kThreadError_InvalidArgs;

    otRemoveMacWhitelist(aNode->mInstance, extAddr);
    otLogFuncExit();
    return 0;
}

OTNODEAPI uint16_t OTCALL otNodeGetAddr16(otNode* aNode)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    auto result = otGetRloc16(aNode->mInstance);
    printf("%d: rloc16\r\n%04x\r\n", aNode->mId, result);
    otLogFuncExit();
    return result;
}

OTNODEAPI const char* OTCALL otNodeGetAddr64(otNode* aNode)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    auto extAddr = otGetExtendedAddress(aNode->mInstance);
    char* str = (char*)malloc(18);
    for (int i = 0; i < 8; i++)
        sprintf_s(str + i * 2, 18 - (2 * i), "%02x", extAddr[i]);
    printf("%d: extaddr\r\n%s\r\n", aNode->mId, str);
    otLogFuncExit();
    return str;
}

OTNODEAPI int32_t OTCALL otNodeSetChannel(otNode* aNode, uint8_t aChannel)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: channel %d\r\n", aNode->mId, aChannel);
    auto result = otSetChannel(aNode->mInstance, aChannel);
    otLogFuncExit();
    return result;
}

OTNODEAPI uint32_t OTCALL otNodeGetKeySequence(otNode* aNode)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    auto result = otGetKeySequenceCounter(aNode->mInstance);
    printf("%d: key sequence\r\n%d\r\n", aNode->mId, result);
    otLogFuncExit();
    return result;
}

OTNODEAPI int32_t OTCALL otNodeSetKeySequence(otNode* aNode, uint32_t aSequence)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: key sequence %d\r\n", aNode->mId, aSequence);
    otSetKeySequenceCounter(aNode->mInstance, aSequence);
    otLogFuncExit();
    return 0;
}

OTNODEAPI int32_t OTCALL otNodeSetNetworkIdTimeout(otNode* aNode, uint8_t aTimeout)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: network id timeout %d\r\n", aNode->mId, aTimeout);
    otSetNetworkIdTimeout(aNode->mInstance, aTimeout);
    otLogFuncExit();
    return 0;
}

OTNODEAPI int32_t OTCALL otNodeSetNetworkName(otNode* aNode, const char *aName)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: network name %s\r\n", aNode->mId, aName);
    auto result = otSetNetworkName(aNode->mInstance, aName);
    otLogFuncExit();
    return result;
}

OTNODEAPI uint16_t OTCALL otNodeGetPanId(otNode* aNode)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    auto result = otGetPanId(aNode->mInstance);
    printf("%d: panid\r\n0x%04x\r\n", aNode->mId, result);
    otLogFuncExit();
    return result;
}

OTNODEAPI int32_t OTCALL otNodeSetPanId(otNode* aNode, uint16_t aPanId)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: panid 0x%04x\r\n", aNode->mId, aPanId);
    auto result = otSetPanId(aNode->mInstance, aPanId);
    otLogFuncExit();
    return result;
}

OTNODEAPI int32_t OTCALL otNodeSetRouterUpgradeThreshold(otNode* aNode, uint8_t aThreshold)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: router upgrade threshold %d\r\n", aNode->mId, aThreshold);
    otSetRouterUpgradeThreshold(aNode->mInstance, aThreshold);
    otLogFuncExit();
    return 0;
}

OTNODEAPI int32_t OTCALL otNodeReleaseRouterId(otNode* aNode, uint8_t aRouterId)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: release router id %d\r\n", aNode->mId, aRouterId);
    auto result = otReleaseRouterId(aNode->mInstance, aRouterId);
    otLogFuncExit();
    return result;
}

OTNODEAPI const char* OTCALL otNodeGetState(otNode* aNode)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    auto role = otGetDeviceRole(aNode->mInstance);
    auto result = _strdup(otDeviceRoleToString(role));
    printf("%d: state\r\n%s\r\n", aNode->mId, result);
    otLogFuncExit();
    return result;
}

OTNODEAPI int32_t OTCALL otNodeSetState(otNode* aNode, const char *aState)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: state %s\r\n", aNode->mId, aState);

    ThreadError error;
    if (strcmp(aState, "detached") == 0)
    {
        error = otBecomeDetached(aNode->mInstance);
    }
    else if (strcmp(aState, "child") == 0)
    {
        error = otBecomeChild(aNode->mInstance, kMleAttachAnyPartition);
    }
    else if (strcmp(aState, "router") == 0)
    {
        error = otBecomeRouter(aNode->mInstance);
    }
    else if (strcmp(aState, "leader") == 0)
    {
        error = otBecomeLeader(aNode->mInstance);
    }
    else
    {
        error = kThreadError_InvalidArgs;
    }
    otLogFuncExit();
    return error;
}

OTNODEAPI uint32_t OTCALL otNodeGetTimeout(otNode* aNode)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    auto result = otGetChildTimeout(aNode->mInstance);
    otLogFuncExit();
    return result;
}

OTNODEAPI int32_t OTCALL otNodeSetTimeout(otNode* aNode, uint32_t aTimeout)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: timeout %d\r\n", aNode->mId, aTimeout);
    otSetChildTimeout(aNode->mInstance, aTimeout);
    otLogFuncExit();
    return 0;
}

OTNODEAPI uint8_t OTCALL otNodeGetWeight(otNode* aNode)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    auto result = otGetLeaderWeight(aNode->mInstance);
    otLogFuncExit();
    return result;
}

OTNODEAPI int32_t OTCALL otNodeSetWeight(otNode* aNode, uint8_t aWeight)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: leader weight %d\r\n", aNode->mId, aWeight);
    otSetLocalLeaderWeight(aNode->mInstance, aWeight);
    otLogFuncExit();
    return 0;
}

OTNODEAPI int32_t OTCALL otNodeAddIpAddr(otNode* aNode, const char *aAddr)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: add ipaddr %s\r\n", aNode->mId, aAddr);

    otNetifAddress aAddress;
    auto error = otIp6AddressFromString(aAddr, &aAddress.mAddress);
    if (error != kThreadError_None) return error;

    aAddress.mPrefixLength = 64;
    aAddress.mPreferredLifetime = 0xffffffff;
    aAddress.mValidLifetime = 0xffffffff;
    auto result = otAddUnicastAddress(aNode->mInstance, &aAddress);
    otLogFuncExit();
    return result;
}

inline uint16_t Swap16(uint16_t v)
{
    return
        (((v & 0x00ffU) << 8) & 0xff00) |
        (((v & 0xff00U) >> 8) & 0x00ff);
}

OTNODEAPI const char* OTCALL otNodeGetAddrs(otNode* aNode)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    auto addrs = otGetUnicastAddresses(aNode->mInstance);
    if (addrs == nullptr) return nullptr;

    char* str = (char*)malloc(512);
    RtlZeroMemory(str, 512);

    char* cur = str;
    
    for (const otNetifAddress *addr = addrs; addr; addr = addr->mNext)
    {
        if (cur != str)
        {
            *cur = '\n';
            cur++;
        }

        cur += 
            sprintf_s(
                cur, 512 - (cur - str),
                "%x:%x:%x:%x:%x:%x:%x:%x",
                Swap16(addr->mAddress.mFields.m16[0]),
                Swap16(addr->mAddress.mFields.m16[1]),
                Swap16(addr->mAddress.mFields.m16[2]),
                Swap16(addr->mAddress.mFields.m16[3]),
                Swap16(addr->mAddress.mFields.m16[4]),
                Swap16(addr->mAddress.mFields.m16[5]),
                Swap16(addr->mAddress.mFields.m16[6]),
                Swap16(addr->mAddress.mFields.m16[7]));
    }

    otFreeMemory(addrs);
    
    printf("%d: ipaddr\r\n%s\r\n", aNode->mId, str);
    otLogFuncExit();

    return str;
}

OTNODEAPI uint32_t OTCALL otNodeGetContextReuseDelay(otNode* aNode)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    auto result = otGetContextIdReuseDelay(aNode->mInstance);
    otLogFuncExit();
    return result;
}

OTNODEAPI int32_t OTCALL otNodeSetContextReuseDelay(otNode* aNode, uint32_t aDelay)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    otSetContextIdReuseDelay(aNode->mInstance, aDelay);
    otLogFuncExit();
    return 0;
}

OTNODEAPI int32_t OTCALL otNodeAddPrefix(otNode* aNode, const char *aPrefix, const char *aFlags, const char *aPreference)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    otBorderRouterConfig config = {0};
    char *prefixLengthStr;
    char *endptr;

    if ((prefixLengthStr = (char*)strchr(aPrefix, '/')) == NULL)
        return kThreadError_InvalidArgs;

    *prefixLengthStr++ = '\0';
    
    auto error = otIp6AddressFromString(aPrefix, &config.mPrefix.mPrefix);
    if (error != kThreadError_None) return error;

    config.mPrefix.mLength = static_cast<uint8_t>(strtol(prefixLengthStr, &endptr, 0));
    
    if (*endptr != '\0') return kThreadError_Parse;
    
    const char *index = aFlags;
    while (*index)
    {
        switch (*index)
        {
        case 'p':
            config.mPreferred = true;
            break;
        case 'a':
            config.mSlaac = true;
            break;
        case 'd':
            config.mDhcp = true;
            break;
        case 'c':
            config.mConfigure = true;
            break;
        case 'r':
            config.mDefaultRoute = true;
            break;
        case 'o':
            config.mOnMesh = true;
            break;
        case 's':
            config.mStable = true;
            break;
        default:
            return kThreadError_InvalidArgs;
        }

        index++;
    }
    
    if (strcmp(aPreference, "high") == 0)
    {
        config.mPreference = 1;
    }
    else if (strcmp(aPreference, "med") == 0)
    {
        config.mPreference = 1;
    }
    else if (strcmp(aPreference, "low") == 0)
    {
        config.mPreference = -1;
    }
    else
    {
        return kThreadError_InvalidArgs;
    }

    auto result = otAddBorderRouter(aNode->mInstance, &config);
    otLogFuncExit();
    return result;
}

OTNODEAPI int32_t OTCALL otNodeRemovePrefix(otNode* aNode, const char *aPrefix)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    struct otIp6Prefix prefix;
    char *prefixLengthStr;
    char *endptr;

    if ((prefixLengthStr = (char *)strchr(aPrefix, '/')) == NULL)
        return kThreadError_InvalidArgs;

    *prefixLengthStr++ = '\0';
    
    auto error = otIp6AddressFromString(aPrefix, &prefix.mPrefix);
    if (error != kThreadError_None) return error;

    prefix.mLength = static_cast<uint8_t>(strtol(prefixLengthStr, &endptr, 0));

    if (*endptr != '\0') return kThreadError_Parse;

    auto result = otRemoveBorderRouter(aNode->mInstance, &prefix);
    otLogFuncExit();
    return result;
}

OTNODEAPI int32_t OTCALL otNodeAddRoute(otNode* aNode, const char *aPrefix, const char *aPreference)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    otExternalRouteConfig config = {0};
    char *prefixLengthStr;
    char *endptr;

    if ((prefixLengthStr = (char*)strchr(aPrefix, '/')) == NULL)
        return kThreadError_InvalidArgs;

    *prefixLengthStr++ = '\0';
    
    auto error = otIp6AddressFromString(aPrefix, &config.mPrefix.mPrefix);
    if (error != kThreadError_None) return error;

    config.mPrefix.mLength = static_cast<uint8_t>(strtol(prefixLengthStr, &endptr, 0));
    
    if (*endptr != '\0') return kThreadError_Parse;
    
    if (strcmp(aPreference, "high") == 0)
    {
        config.mPreference = 1;
    }
    else if (strcmp(aPreference, "med") == 0)
    {
        config.mPreference = 1;
    }
    else if (strcmp(aPreference, "low") == 0)
    {
        config.mPreference = -1;
    }
    else
    {
        return kThreadError_InvalidArgs;
    }

    auto result = otAddExternalRoute(aNode->mInstance, &config);
    otLogFuncExit();
    return result;
}

OTNODEAPI int32_t OTCALL otNodeRemoveRoute(otNode* aNode, const char *aPrefix)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    struct otIp6Prefix prefix;
    char *prefixLengthStr;
    char *endptr;

    if ((prefixLengthStr = (char *)strchr(aPrefix, '/')) == NULL)
        return kThreadError_InvalidArgs;

    *prefixLengthStr++ = '\0';
    
    auto error = otIp6AddressFromString(aPrefix, &prefix.mPrefix);
    if (error != kThreadError_None) return error;

    prefix.mLength = static_cast<uint8_t>(strtol(prefixLengthStr, &endptr, 0));

    if (*endptr != '\0') return kThreadError_Parse;

    auto result = otRemoveExternalRoute(aNode->mInstance, &prefix);
    otLogFuncExit();
    return result;
}

OTNODEAPI int32_t OTCALL otNodeRegisterNetdata(otNode* aNode)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    auto result = otSendServerData(aNode->mInstance);
    otLogFuncExit();
    return result;
}

void OTCALL otNodeCommissionerEnergyReportCallback(uint32_t aChannelMask, const uint8_t *aEnergyList, uint8_t aEnergyListLength, void *aContext)
{
    otNode* aNode = (otNode*)aContext;

    printf("Energy: 0x%08x\r\n", aChannelMask);
    for (uint8_t i = 0; i < aEnergyListLength; i++)
        printf("%d ", aEnergyList[i]);
    printf("\r\n");

    SetEvent(aNode->mEnergyScanEvent);
}

OTNODEAPI int32_t OTCALL otNodeEnergyScan(otNode* aNode, uint32_t aMask, uint8_t aCount, uint16_t aPeriod, uint16_t aDuration, const char *aAddr)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: energy scan 0x%x %d %d %d %s\r\n", aNode->mId, aMask, aCount, aPeriod, aDuration, aAddr);

    otIp6Address address = {0};
    auto error = otIp6AddressFromString(aAddr, &address);
    if (error != kThreadError_None)
    {
        printf("otIp6AddressFromString(%s) failed, 0x%x!\r\n", aAddr, error);
        return error;
    }
    
    ResetEvent(aNode->mEnergyScanEvent);

    error = otCommissionerEnergyScan(aNode->mInstance, aMask, aCount, aPeriod, aDuration, &address, otNodeCommissionerEnergyReportCallback, aNode);
    if (error != kThreadError_None)
    {
        printf("otCommissionerEnergyScan failed, 0x%x!\r\n", error);
        return error;
    }

    auto result = WaitForSingleObject(aNode->mEnergyScanEvent, 8000) == WAIT_OBJECT_0 ? kThreadError_None : kThreadError_NotFound;
    otLogFuncExit();
    return result;
}

void OTCALL otNodeCommissionerPanIdConflictCallback(uint16_t aPanId, uint32_t aChannelMask, void *aContext)
{
    otNode* aNode = (otNode*)aContext;
    printf("Conflict: 0x%04x, 0x%08x\r\n", aPanId, aChannelMask);
    SetEvent(aNode->mPanIdConflictEvent);
}

OTNODEAPI int32_t OTCALL otNodePanIdQuery(otNode* aNode, uint16_t aPanId, uint32_t aMask, const char *aAddr)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    printf("%d: panid query 0x%04x 0x%x %s\r\n", aNode->mId, aPanId, aMask, aAddr);

    otIp6Address address = {0};
    auto error = otIp6AddressFromString(aAddr, &address);
    if (error != kThreadError_None)
    {
        printf("otIp6AddressFromString(%s) failed, 0x%x!\r\n", aAddr, error);
        return error;
    }
    
    ResetEvent(aNode->mPanIdConflictEvent);

    error = otCommissionerPanIdQuery(aNode->mInstance, aPanId, aMask, &address, otNodeCommissionerPanIdConflictCallback, aNode);
    if (error != kThreadError_None)
    {
        printf("otCommissionerPanIdQuery failed, 0x%x!\r\n", error);
        return error;
    }

    auto result = WaitForSingleObject(aNode->mPanIdConflictEvent, 8000) == WAIT_OBJECT_0 ? kThreadError_None : kThreadError_NotFound;
    otLogFuncExit();
    return result;
}

OTNODEAPI const char* OTCALL otNodeScan(otNode* aNode)
{
    otLogFuncEntryMsg("[%d]", aNode->mId);
    UNREFERENCED_PARAMETER(aNode);
    otLogFuncExit();
    return nullptr;
}

OTNODEAPI uint32_t OTCALL otNodePing(otNode* aNode, const char *aAddr, uint16_t aSize, uint32_t aMinReplies)
{
    otLogFuncEntryMsg("[%d] %s (%d bytes)", aNode->mId, aAddr, aSize);
    printf("%d: ping %s (%d bytes)\r\n", aNode->mId, aAddr, aSize);

    // Convert string to destination address
    otIp6Address otDestinationAddress = {0};
    auto error = otIp6AddressFromString(aAddr, &otDestinationAddress);
    if (error != kThreadError_None)
    {
        printf("otIp6AddressFromString(%s) failed!\r\n", aAddr);
        return 0;
    }
    
    // Get ML-EID as source address for ping
    auto otSourceAddress = otGetMeshLocalEid(aNode->mInstance);

    sockaddr_in6 SourceAddress = { AF_INET6, (USHORT)(CertificationPingPort + 1) };
    sockaddr_in6 DestinationAddress = { AF_INET6, CertificationPingPort };

    memcpy(&SourceAddress.sin6_addr, otSourceAddress, sizeof(IN6_ADDR));
    memcpy(&DestinationAddress.sin6_addr, &otDestinationAddress, sizeof(IN6_ADDR));

    otFreeMemory(otSourceAddress);
    otSourceAddress = nullptr;
    
    // Put the current thead in the correct compartment
    bool RevertCompartmentOnExit = false;
    ULONG OriginalCompartmentID = GetCurrentThreadCompartmentId();
    if (OriginalCompartmentID != otGetCompartmentId(aNode->mInstance))
    {
        DWORD dwError = ERROR_SUCCESS;
        if ((dwError = SetCurrentThreadCompartmentId(otGetCompartmentId(aNode->mInstance))) != ERROR_SUCCESS)
        {
            printf("SetCurrentThreadCompartmentId failed, 0x%x\r\n", dwError);
        }
        RevertCompartmentOnExit = true;
    }

    int result = 0;

    auto SendBuffer = (PCHAR)malloc(aSize);
    auto RecvBuffer = (PCHAR)malloc(aSize);

    WSABUF WSARecvBuffer = { aSize, RecvBuffer };
    
    WSAOVERLAPPED Overlapped = { 0 };
    Overlapped.hEvent = WSACreateEvent();

    DWORD numberOfReplies = 0;
    bool isPending = false;
    DWORD Flags;
    DWORD cbReceived;
    int cbDestinationAddress = sizeof(DestinationAddress);

    SOCKET Socket = WSASocket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (Socket == INVALID_SOCKET)
    {
        printf("WSASocket failed, 0x%x\r\n", WSAGetLastError());
        goto exit;
    }

    // Bind the socket to the address
    result = bind(Socket, (sockaddr*)&SourceAddress, sizeof(SourceAddress));
    if (result == SOCKET_ERROR)
    {
        printf("bind failed, 0x%x\r\n", WSAGetLastError());
        goto exit;
    }

    // Initialize the send buffer pattern.
    for (uint32_t i = 0; i < aSize; i++)
        SendBuffer[i] = (char)('a' + (i % 23));

    // Hack to retrieve destination on other end
    memcpy_s(SendBuffer, aSize, &otDestinationAddress, sizeof(IN6_ADDR));

    // Send the buffer
    result = sendto(Socket, SendBuffer, sizeof(SendBuffer), 0, (SOCKADDR*)&DestinationAddress, sizeof(DestinationAddress));
    if (result == SOCKET_ERROR)
    {
        printf("sendto failed, 0x%x\r\n", WSAGetLastError());
        goto exit;
    }

    auto StartTick = GetTickCount64();
    
    while (numberOfReplies < aMinReplies)
    {
        Flags = 0; //MSG_PARTIAL;
        result = WSARecvFrom(Socket, &WSARecvBuffer, 1, &cbReceived, &Flags, (SOCKADDR*)&DestinationAddress, &cbDestinationAddress, &Overlapped, NULL);
        if (result == SOCKET_ERROR)
        {
            result = WSAGetLastError();
            if (result == WSA_IO_PENDING)
            {
                isPending = true;
            }
            else
            {
                printf("WSARecvFrom failed, 0x%x\r\n", result);
                goto exit;
            }
        }

        if (isPending)
        {
            //printf("waiting for completion event...\r\n");
            // Wait for the receive to complete
            result = WSAWaitForMultipleEvents(1, &Overlapped.hEvent, TRUE, (DWORD)(4000 - (GetTickCount64() - StartTick)), TRUE);
            if (result == WSA_WAIT_TIMEOUT)
            {
                //printf("recv timeout\r\n");
                goto exit;
            }
            else if (result == WSA_WAIT_FAILED)
            {
                printf("recv failed\r\n");
                goto exit;
            }
        }

        result = WSAGetOverlappedResult(Socket, &Overlapped, &cbReceived, TRUE, &Flags);
        if (result == FALSE)
        {
            printf("WSAGetOverlappedResult failed, 0x%x\r\n", WSAGetLastError());
            goto exit;
        }

        numberOfReplies++;
    }

exit:
    
    // Revert the comparment if necessary
    if (RevertCompartmentOnExit)
    {
        (VOID)SetCurrentThreadCompartmentId(OriginalCompartmentID);
    }

    free(RecvBuffer);
    free(SendBuffer);

    WSACloseEvent(Overlapped.hEvent);

    if (Socket != INVALID_SOCKET) closesocket(Socket);

    otLogFuncExit();

    return numberOfReplies;
}