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

/**
 * @file
 * @brief
 *  This file implements the logging function required for the OpenThread library.
 */

#include "precomp.h"
#include "radio.tmh"

void 
otPlatReset(
    _In_ otInstance *otCtx
    )
{
    NT_ASSERT(otCtx);
    // This function does nothing currently.
    UNREFERENCED_PARAMETER(otCtx);
}

otPlatResetReason 
otPlatGetResetReason(
    _In_ otInstance *otCtx
    )
{
    NT_ASSERT(otCtx);
    UNREFERENCED_PARAMETER(otCtx);
    return kPlatResetReason_PowerOn;
}

VOID 
otLwfRadioSendPacket(
    _In_ PMS_FILTER     pFilter,
    _In_ RadioPacket*   Packet
    );

VOID 
otLwfRadioInit(
    _In_ PMS_FILTER pFilter
    )
{
    LogFuncEntry(DRIVER_DEFAULT);

    pFilter->otPhyState = kStateDisabled;
    pFilter->otCurrentListenChannel = 0xFF;
    pFilter->otPromiscuous = false;

    pFilter->otReceiveFrame.mPsdu = pFilter->otReceiveMessage;
    pFilter->otTransmitFrame.mPsdu = pFilter->otTransmitMessage;
    
    pFilter->otPendingMacOffloadEnabled = FALSE;
    pFilter->otPendingShortAddressCount = 0;
    pFilter->otPendingExtendedAddressCount = 0;
    
    LogInfo(DRIVER_DEFAULT, "Filter %p PhyState = kStateDisabled.", pFilter);
    
    LogFuncExit(DRIVER_DEFAULT);
}

void otPlatRadioGetIeeeEui64(otInstance *otCtx, uint8_t *aIeeeEui64)
{
    NT_ASSERT(otCtx);
    PMS_FILTER pFilter = otCtxToFilter(otCtx);
    NDIS_STATUS status;
    ULONG bytesProcessed;
    OT_FACTORY_EXTENDED_ADDRESS OidBuffer = { {0} };

    RtlZeroMemory(aIeeeEui64, sizeof(ULONGLONG));

    // Query the MP for the address
    status = 
        otLwfSendInternalRequest(
            pFilter,
            NdisRequestQueryInformation,
            OID_OT_FACTORY_EXTENDED_ADDRESS,
            &OidBuffer,
            sizeof(OT_FACTORY_EXTENDED_ADDRESS),
            &bytesProcessed
            );
    if (status != NDIS_STATUS_SUCCESS)
    {
        LogError(DRIVER_DEFAULT, "Query for OID_FACTORY_EXTENDED_ADDRESS failed, %!NDIS_STATUS!", status);
        return;
    }

    // Validate the return header
    if (bytesProcessed != SIZEOF_OT_FACTORY_EXTENDED_ADDRESS_REVISION_1 ||
        pFilter->MiniportCapabilities.Header.Type != NDIS_OBJECT_TYPE_DEFAULT ||
        pFilter->MiniportCapabilities.Header.Revision != OT_FACTORY_EXTENDED_ADDRESS_REVISION_1 ||
        pFilter->MiniportCapabilities.Header.Size != SIZEOF_OT_FACTORY_EXTENDED_ADDRESS_REVISION_1)
    {
        LogError(DRIVER_DEFAULT, "Query for OID_OT_FACTORY_EXTENDED_ADDRESS returned invalid data");
        return;
    }

    LogInfo(DRIVER_DEFAULT, "Interface %!GUID! get factory Extended Mac Address: %llX", &pFilter->InterfaceGuid, OidBuffer.ExtendedAddress);

    memcpy(aIeeeEui64, &OidBuffer.ExtendedAddress, sizeof(ULONGLONG));
}

ThreadError otPlatRadioSetPanId(_In_ otInstance *otCtx, uint16_t panid)
{
    NT_ASSERT(otCtx);
    PMS_FILTER pFilter = otCtxToFilter(otCtx);
    NDIS_STATUS status;
    ULONG bytesProcessed;
    OT_PAND_ID OidBuffer = { {NDIS_OBJECT_TYPE_DEFAULT, OT_PAND_ID_REVISION_1, SIZEOF_OT_PAND_ID_REVISION_1}, panid };
    
    NT_ASSERT(pFilter->otPhyState != kStateTransmit);
    if (pFilter->otPhyState == kStateTransmit) return kThreadError_Busy;

    LogInfo(DRIVER_DEFAULT, "Interface %!GUID! set PanID: %X", &pFilter->InterfaceGuid, panid);

    pFilter->otPanID = panid;

    // Indicate to the miniport
    status = 
        otLwfSendInternalRequest(
            pFilter,
            NdisRequestSetInformation,
            OID_OT_PAND_ID,
            &OidBuffer,
            sizeof(OidBuffer),
            &bytesProcessed
            );
    if (status != NDIS_STATUS_SUCCESS)
    {
        LogError(DRIVER_DEFAULT, "Set for OID_OT_PAND_ID failed, %!NDIS_STATUS!", status);
    }

    return kThreadError_None;
}

ThreadError otPlatRadioSetExtendedAddress(_In_ otInstance *otCtx, uint8_t *address)
{
    NT_ASSERT(otCtx);
    PMS_FILTER pFilter = otCtxToFilter(otCtx);
    NDIS_STATUS status;
    ULONG bytesProcessed;
    OT_EXTENDED_ADDRESS OidBuffer = { {NDIS_OBJECT_TYPE_DEFAULT, OT_EXTENDED_ADDRESS_REVISION_1, SIZEOF_OT_EXTENDED_ADDRESS_REVISION_1} };
    
    NT_ASSERT(pFilter->otPhyState != kStateTransmit);
    if (pFilter->otPhyState == kStateTransmit) return kThreadError_Busy;

    LogInfo(DRIVER_DEFAULT, "Interface %!GUID! set Extended Mac Address: %llX", &pFilter->InterfaceGuid, *(ULONGLONG*)address);

    pFilter->otExtendedAddress = *(ULONGLONG*)address;
    OidBuffer.ExtendedAddress = *(ULONGLONG*)address;

    // Indicate to the miniport
    status = 
        otLwfSendInternalRequest(
            pFilter,
            NdisRequestSetInformation,
            OID_OT_EXTENDED_ADDRESS,
            &OidBuffer,
            sizeof(OidBuffer),
            &bytesProcessed
            );
    if (status != NDIS_STATUS_SUCCESS)
    {
        LogError(DRIVER_DEFAULT, "Set for OID_OT_EXTENDED_ADDRESS failed, %!NDIS_STATUS!", status);
    }

    return kThreadError_None;
}

ThreadError otPlatRadioSetShortAddress(_In_ otInstance *otCtx, uint16_t address)
{
    NT_ASSERT(otCtx);
    PMS_FILTER pFilter = otCtxToFilter(otCtx);
    NDIS_STATUS status;
    ULONG bytesProcessed;
    OT_SHORT_ADDRESS OidBuffer = { {NDIS_OBJECT_TYPE_DEFAULT, OT_SHORT_ADDRESS_REVISION_1, SIZEOF_OT_SHORT_ADDRESS_REVISION_1}, address };
    
    //NT_ASSERT(pFilter->otPhyState != kStateTransmit);
    //if (pFilter->otPhyState == kStateTransmit) return kThreadError_Busy;

    LogInfo(DRIVER_DEFAULT, "Interface %!GUID! set Short Mac Address: %X", &pFilter->InterfaceGuid, address);

    pFilter->otShortAddress = address;

    // Indicate to the miniport
    status = 
        otLwfSendInternalRequest(
            pFilter,
            NdisRequestSetInformation,
            OID_OT_SHORT_ADDRESS,
            &OidBuffer,
            sizeof(OidBuffer),
            &bytesProcessed
            );
    if (status != NDIS_STATUS_SUCCESS)
    {
        LogError(DRIVER_DEFAULT, "Set for OID_OT_SHORT_ADDRESS failed, %!NDIS_STATUS!", status);
    }

    return kThreadError_None;
}

void otPlatRadioSetPromiscuous(_In_ otInstance *otCtx, int aEnable)
{
    NT_ASSERT(otCtx);
    PMS_FILTER pFilter = otCtxToFilter(otCtx);
    NDIS_STATUS status;
    ULONG bytesProcessed;
    OT_PROMISCUOUS_MODE OidBuffer = { {NDIS_OBJECT_TYPE_DEFAULT, OT_PROMISCUOUS_MODE_REVISION_1, SIZEOF_OT_PROMISCUOUS_MODE_REVISION_1}, (BOOLEAN)aEnable };

    pFilter->otPromiscuous = (BOOLEAN)aEnable;

    // Indicate to the miniport
    status = 
        otLwfSendInternalRequest(
            pFilter,
            NdisRequestSetInformation,
            OID_OT_PROMISCUOUS_MODE,
            &OidBuffer,
            sizeof(OidBuffer),
            &bytesProcessed
            );
    if (status != NDIS_STATUS_SUCCESS)
    {
        LogError(DRIVER_DEFAULT, "Set for OID_OT_PROMISCUOUS_MODE failed, %!NDIS_STATUS!", status);
    }
}

ThreadError otPlatRadioEnable(_In_ otInstance *otCtx)
{
    NT_ASSERT(otCtx);
    PMS_FILTER pFilter = otCtxToFilter(otCtx);
    
    NT_ASSERT(pFilter->otPhyState <= kStateSleep);
    if (pFilter->otPhyState > kStateSleep) return kThreadError_Busy;

    pFilter->otPhyState = kStateSleep;
    
    LogInfo(DRIVER_DEFAULT, "Filter %p PhyState = kStateSleep.", pFilter);

    return kThreadError_None;
}

ThreadError otPlatRadioDisable(_In_ otInstance *otCtx)
{
    NT_ASSERT(otCtx);
    PMS_FILTER pFilter = otCtxToFilter(otCtx);
    
    NT_ASSERT(pFilter->otPhyState <= kStateSleep);
    if (pFilter->otPhyState > kStateSleep) return kThreadError_Busy;

    pFilter->otPhyState = kStateDisabled;
    
    LogInfo(DRIVER_DEFAULT, "Filter %p PhyState = kStateDisabled.", pFilter);

    return kThreadError_None;
}

ThreadError otPlatRadioSleep(_In_ otInstance *otCtx)
{
    NT_ASSERT(otCtx);
    PMS_FILTER pFilter = otCtxToFilter(otCtx);
    
    NT_ASSERT(pFilter->otPhyState != kStateDisabled);
    //NT_ASSERT(pFilter->otPhyState == kStateSleep || pFilter->otPhyState == kStateReceive);
    if (pFilter->otPhyState != kStateSleep && pFilter->otPhyState != kStateReceive) 
        return kThreadError_Busy;

    if (pFilter->otPhyState != kStateSleep)
    {
        pFilter->otPhyState = kStateSleep;
        LogInfo(DRIVER_DEFAULT, "Filter %p PhyState = kStateSleep.", pFilter);
        
        if ((pFilter->MiniportCapabilities.RadioCapabilities & OT_RADIO_CAP_SLEEP) != 0)
        {
            NDIS_STATUS status;
            ULONG bytesProcessed;
            OT_SLEEP_MODE OidBuffer = { {NDIS_OBJECT_TYPE_DEFAULT, OT_SLEEP_MODE_REVISION_1, SIZEOF_OT_SLEEP_MODE_REVISION_1}, TRUE };

            // Indicate to the miniport
            status = 
                otLwfSendInternalRequest(
                    pFilter,
                    NdisRequestSetInformation,
                    OID_OT_SLEEP_MODE,
                    &OidBuffer,
                    sizeof(OidBuffer),
                    &bytesProcessed
                    );
            if (status != NDIS_STATUS_SUCCESS)
            {
                LogError(DRIVER_DEFAULT, "Set for OID_OT_SLEEP_MODE failed, %!NDIS_STATUS!", status);
            }
        }
    }

    return kThreadError_None;
}

ThreadError otPlatRadioReceive(_In_ otInstance *otCtx, uint8_t aChannel)
{
    NT_ASSERT(otCtx);
    PMS_FILTER pFilter = otCtxToFilter(otCtx);
    
    NT_ASSERT(pFilter->otPhyState != kStateDisabled);
    if (pFilter->otPhyState == kStateDisabled) return kThreadError_Busy;
    
    LogFuncEntryMsg(DRIVER_DATA_PATH, "Filter: %p", pFilter);
    
    // If we are currently in the sleep state and the minport supports sleep 
    // mode, come out of sleep mode now.
    if (pFilter->otPhyState == kStateSleep)
    {        
        if ((pFilter->MiniportCapabilities.RadioCapabilities & OT_RADIO_CAP_SLEEP) != 0)
        {
            NDIS_STATUS status;
            ULONG bytesProcessed;
            OT_SLEEP_MODE OidBuffer = { {NDIS_OBJECT_TYPE_DEFAULT, OT_SLEEP_MODE_REVISION_1, SIZEOF_OT_SLEEP_MODE_REVISION_1}, FALSE };

            // Indicate to the miniport
            status = 
                otLwfSendInternalRequest(
                    pFilter,
                    NdisRequestSetInformation,
                    OID_OT_SLEEP_MODE,
                    &OidBuffer,
                    sizeof(OidBuffer),
                    &bytesProcessed
                    );
            if (status != NDIS_STATUS_SUCCESS)
            {
                LogError(DRIVER_DEFAULT, "Set for OID_OT_SLEEP_MODE failed, %!NDIS_STATUS!", status);
            }
        }
    }

    // Update current channel if different
    if (pFilter->otCurrentListenChannel != aChannel)
    {
        NDIS_STATUS status;
        ULONG bytesProcessed;
        OT_CURRENT_CHANNEL OidBuffer = { {NDIS_OBJECT_TYPE_DEFAULT, OT_CURRENT_CHANNEL_REVISION_1, SIZEOF_OT_CURRENT_CHANNEL_REVISION_1}, aChannel };

        LogInfo(DRIVER_DEFAULT, "Filter %p new Listen Channel = %u.", pFilter, aChannel);
        pFilter->otCurrentListenChannel = aChannel;
        
        // Indicate to the miniport
        status = 
            otLwfSendInternalRequest(
                pFilter,
                NdisRequestSetInformation,
                OID_OT_CURRENT_CHANNEL,
                &OidBuffer,
                sizeof(OidBuffer),
                &bytesProcessed
                );
        if (status != NDIS_STATUS_SUCCESS)
        {
            LogError(DRIVER_DEFAULT, "Set for OID_OT_CURRENT_CHANNEL failed, %!NDIS_STATUS!", status);
        }
    }

    // Only transition to the receive state if we were sleeping; otherwise we 
    // are already in receive or transmit state.
    if (pFilter->otPhyState == kStateSleep)
    {
        pFilter->otPhyState = kStateReceive;
        LogInfo(DRIVER_DEFAULT, "Filter %p PhyState = kStateReceive.", pFilter);
    
        // Set the event to indicate we can process NBLs
        KeSetEvent(&pFilter->EventWorkerThreadProcessNBLs, 0, FALSE);
    }
    
    LogFuncExit(DRIVER_DATA_PATH);

    return kThreadError_None;
}

RadioPacket *otPlatRadioGetTransmitBuffer(_In_ otInstance *otCtx)
{
    NT_ASSERT(otCtx);
    PMS_FILTER pFilter = otCtxToFilter(otCtx);
    return &pFilter->otTransmitFrame;
}

int8_t otPlatRadioGetRssi(_In_ otInstance *otCtx)
{
    NT_ASSERT(otCtx);
    PMS_FILTER pFilter = otCtxToFilter(otCtx);
    UNREFERENCED_PARAMETER(pFilter);
    return 0;
}

otRadioCaps otPlatRadioGetCaps(_In_ otInstance *otCtx)
{
    NT_ASSERT(otCtx);
    return 
        ((otCtxToFilter(otCtx)->MiniportCapabilities.RadioCapabilities & OT_RADIO_CAP_ACK_TIMEOUT) != 0) ? 
            kRadioCapsAckTimeout : 
            kRadioCapsNone;
}

int otPlatRadioGetPromiscuous(_In_ otInstance *otCtx)
{
    NT_ASSERT(otCtx);
    PMS_FILTER pFilter = otCtxToFilter(otCtx);
    return pFilter->otPromiscuous;
}

VOID 
otLwfRadioReceiveFrame(
    _In_ PMS_FILTER pFilter
    )
{    
    NT_ASSERT(pFilter->otReceiveFrame.mChannel >= 11 && pFilter->otReceiveFrame.mChannel <= 26);

    LogFuncEntryMsg(DRIVER_DATA_PATH, "Filter: %p", pFilter);

    NT_ASSERT(pFilter->otPhyState > kStateDisabled);
    if (pFilter->otPhyState > kStateDisabled)
    {
        otPlatRadioReceiveDone(pFilter->otCtx, &pFilter->otReceiveFrame, kThreadError_None);
    }
    else
    {
        LogVerbose(DRIVER_DATA_PATH, "Mac frame dropped.");
    }
    
    LogFuncExit(DRIVER_DATA_PATH);
}

ThreadError otPlatRadioTransmit(_In_ otInstance *otCtx)
{
    NT_ASSERT(otCtx);
    PMS_FILTER pFilter = otCtxToFilter(otCtx);
    ThreadError error = kThreadError_Busy;
    
    LogFuncEntryMsg(DRIVER_DATA_PATH, "Filter: %p", pFilter);

    NT_ASSERT(pFilter->otPhyState == kStateReceive);
    if (pFilter->otPhyState == kStateReceive)
    {
        error = kThreadError_None;
        pFilter->otPhyState = kStateTransmit;
    
        LogInfo(DRIVER_DEFAULT, "Filter %p PhyState = kStateTransmit.", pFilter);
    }

    LogFuncExitMsg(DRIVER_DATA_PATH, "%u", error);

    return error;
}

VOID otLwfRadioTransmitFrame(_In_ PMS_FILTER pFilter)
{
    NT_ASSERT(pFilter->otPhyState == kStateTransmit);

    LogFuncEntryMsg(DRIVER_DATA_PATH, "Filter: %p", pFilter);

    otLwfRadioSendPacket(pFilter, &pFilter->otTransmitFrame);

    LogFuncExit(DRIVER_DATA_PATH);
}

VOID 
otLwfRadioSendPacket(
    _In_ PMS_FILTER     pFilter,
    _In_ RadioPacket*   Packet
    )
{
    SIZE_T BytesCopied = 0;
    PNET_BUFFER SendNetBuffer = NET_BUFFER_LIST_FIRST_NB(pFilter->SendNetBufferList);
    OT_NBL_CONTEXT NblContext = { 0 /* Flags */, Packet->mChannel, Packet->mPower, 0 /* Lqi */ };

    NT_ASSERT(NblContext.Channel >= 11 && NblContext.Channel <= 26);
    NT_ASSERT(!pFilter->SendPending);

    NT_ASSERT(Packet->mLength <= kMaxPHYPacketSize);

    // Copy to the NetBufferList
    RtlCopyBufferToMdl(
        Packet->mPsdu, 
        SendNetBuffer->CurrentMdl, 
        SendNetBuffer->CurrentMdlOffset, 
        Packet->mLength, 
        &BytesCopied
        );
    NT_ASSERT(BytesCopied == Packet->mLength);

    // Set the length field
    NET_BUFFER_DATA_LENGTH(SendNetBuffer) = Packet->mLength;

    // Set the context
    SetNBLContext(pFilter->SendNetBufferList, &NblContext);

    // Reset the completion event
    KeResetEvent(&pFilter->SendNetBufferListComplete);
    pFilter->SendPending = TRUE;

    // Send the NetBufferList
    NdisFSendNetBufferLists(
        pFilter->FilterHandle,
        pFilter->SendNetBufferList,
        NDIS_DEFAULT_PORT_NUMBER,
        0
        );
}

VOID 
otLwfRadioTransmitFrameDone(
    _In_ PMS_FILTER pFilter
    )
{
    LogFuncEntryMsg(DRIVER_DATA_PATH, "Filter: %p", pFilter);

    pFilter->SendPending = FALSE;
    
    NT_ASSERT(pFilter->otPhyState == kStateTransmit);

    // Now that we are completing a send, fall back to receive state and set the event
    pFilter->otPhyState = kStateReceive;
    LogInfo(DRIVER_DEFAULT, "Filter %p PhyState = kStateReceive.", pFilter);
    KeSetEvent(&pFilter->EventWorkerThreadProcessNBLs, 0, FALSE);

    if (STATUS_SUCCESS == pFilter->SendNetBufferList->Status)
    {
        POT_NBL_CONTEXT SendNblContext = GetNBLContext(pFilter->SendNetBufferList);
        BOOLEAN FramePending = (SendNblContext->Flags & OT_NBL_FLAG_ACK_FRAME_PENDING) != 0 || pFilter->CountPendingRecvNBLs != 0;

        otPlatRadioTransmitDone(pFilter->otCtx, FramePending, kThreadError_None);
    }
    else if (STATUS_DEVICE_BUSY == pFilter->SendNetBufferList->Status)
    {
        otPlatRadioTransmitDone(pFilter->otCtx, false, kThreadError_ChannelAccessFailure);
    }
    else if (STATUS_TIMEOUT == pFilter->SendNetBufferList->Status)
    {
        otPlatRadioTransmitDone(pFilter->otCtx, false, kThreadError_NoAck);
    }
    else
    {
        otPlatRadioTransmitDone(pFilter->otCtx, false, kThreadError_Abort);
    }

    LogFuncExit(DRIVER_DATA_PATH);
}

NDIS_STATUS 
otPlatRadioSendPendingMacOffload(
    _In_ PMS_FILTER pFilter
    )
{
    NDIS_STATUS status;
    ULONG bytesProcessed;
    UCHAR ShortAddressCount = pFilter->otPendingMacOffloadEnabled == TRUE ? pFilter->otPendingShortAddressCount : 0;
    UCHAR ExtendedAddressCount = pFilter->otPendingMacOffloadEnabled == TRUE ? pFilter->otPendingExtendedAddressCount : 0;
    USHORT OidBufferSize = COMPLETE_SIZEOF_OT_PENDING_MAC_OFFLOAD_REVISION_1(ShortAddressCount, ExtendedAddressCount);
    POT_PENDING_MAC_OFFLOAD OidBuffer = (POT_PENDING_MAC_OFFLOAD)FILTER_ALLOC_MEM(pFilter->FilterHandle, OidBufferSize);
    PUCHAR Offset = (PUCHAR)(OidBuffer + 1);
    ULONG BufferSizeLeft = OidBufferSize - sizeof(OT_PENDING_MAC_OFFLOAD);

    OidBuffer->Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
    OidBuffer->Header.Revision = OT_PENDING_MAC_OFFLOAD_REVISION_1;
    OidBuffer->Header.Size = OidBufferSize;

    OidBuffer->ShortAddressCount = ShortAddressCount;
    OidBuffer->ExtendedAddressCount = ExtendedAddressCount;

    if (ShortAddressCount != 0)
    {
        memcpy_s(Offset, BufferSizeLeft, pFilter->otPendingShortAddresses, sizeof(USHORT) * ShortAddressCount);
        Offset += sizeof(USHORT) * ShortAddressCount;
        BufferSizeLeft -= sizeof(USHORT) * ShortAddressCount;
    }

    if (ExtendedAddressCount != 0)
    {
        memcpy_s(Offset, BufferSizeLeft, pFilter->otPendingExtendedAddresses, sizeof(ULONGLONG) * ExtendedAddressCount);
    }

    LogInfo(DRIVER_DEFAULT, "Interface %!GUID! indicating updated Pending Mac Offload", &pFilter->InterfaceGuid);
    
    // Indicate to the miniport
    status = 
        otLwfSendInternalRequest(
            pFilter,
            NdisRequestSetInformation,
            OID_OT_PENDING_MAC_OFFLOAD,
            OidBuffer,
            OidBufferSize,
            &bytesProcessed
            );
    if (status != NDIS_STATUS_SUCCESS)
    {
        LogError(DRIVER_DEFAULT, "Set for OID_OT_PENDING_MAC_OFFLOAD failed, %!NDIS_STATUS!", status);
    }

    FILTER_FREE_MEM(OidBuffer);

    return status;
}

void otPlatRadioEnableSrcMatch(_In_ otInstance *otCtx, bool aEnable)
{
    NT_ASSERT(otCtx);
    PMS_FILTER pFilter = otCtxToFilter(otCtx);

    // Ignore if we are already in the correct state
    if (aEnable == pFilter->otPendingMacOffloadEnabled) return;

    // Set the new value and update the miniport
    pFilter->otPendingMacOffloadEnabled = aEnable ? TRUE : FALSE;
    otPlatRadioSendPendingMacOffload(pFilter);
}

ThreadError otPlatRadioAddSrcMatchShortEntry(_In_ otInstance *otCtx, const uint16_t aShortAddress)
{
    NT_ASSERT(otCtx);
    PMS_FILTER pFilter = otCtxToFilter(otCtx);
    
    // Make sure we are enabled
    NT_ASSERT(pFilter->otPendingMacOffloadEnabled);
    if (pFilter->otPendingMacOffloadEnabled == FALSE)
        return kThreadError_InvalidState;

    // Check to see if it is in the list
    BOOLEAN Found = false;
    for (ULONG i = 0; i < pFilter->otPendingShortAddressCount; i++)
    {
        if (aShortAddress == pFilter->otPendingShortAddresses[i])
        {
            Found = TRUE;
            break;
        }
    }

    // Already in the list, return success
    if (Found) return kThreadError_None;

    // Make sure we have room
    if (pFilter->otPendingShortAddressCount == MAX_PENDING_MAC_SIZE) return kThreadError_NoBufs;
    
    // Copy to the list
    pFilter->otPendingShortAddresses[pFilter->otPendingShortAddressCount] = aShortAddress;
    pFilter->otPendingShortAddressCount++;

    // Update the miniport
    otPlatRadioSendPendingMacOffload(pFilter);

    return kThreadError_None;
}

ThreadError otPlatRadioAddSrcMatchExtEntry(_In_ otInstance *otCtx, const uint8_t *aExtAddress)
{
    NT_ASSERT(otCtx);
    PMS_FILTER pFilter = otCtxToFilter(otCtx);
    
    // Make sure we are enabled
    NT_ASSERT(pFilter->otPendingMacOffloadEnabled);
    if (pFilter->otPendingMacOffloadEnabled == FALSE)
        return kThreadError_InvalidState;

    // Check to see if it is in the list
    BOOLEAN Found = false;
    for (ULONG i = 0; i < pFilter->otPendingExtendedAddressCount; i++)
    {
        if (memcmp(aExtAddress, &pFilter->otPendingExtendedAddresses[i], sizeof(ULONGLONG)) == 0)
        {
            Found = TRUE;
            break;
        }
    }

    // Already in the list, return success
    if (Found) return kThreadError_None;

    // Make sure we have room
    if (pFilter->otPendingExtendedAddressCount == MAX_PENDING_MAC_SIZE) return kThreadError_NoBufs;
    
    // Copy to the list
    memcpy(&pFilter->otPendingExtendedAddresses[pFilter->otPendingExtendedAddressCount], aExtAddress, sizeof(ULONGLONG));
    pFilter->otPendingExtendedAddressCount++;

    // Update the miniport
    otPlatRadioSendPendingMacOffload(pFilter);

    return kThreadError_None;
}

ThreadError otPlatRadioClearSrcMatchShortEntry(_In_ otInstance *otCtx, const uint16_t aShortAddress)
{
    NT_ASSERT(otCtx);
    PMS_FILTER pFilter = otCtxToFilter(otCtx);
    
    // Make sure we are enabled
    NT_ASSERT(pFilter->otPendingMacOffloadEnabled);
    if (pFilter->otPendingMacOffloadEnabled == FALSE)
        return kThreadError_InvalidState;

    // Check to see if it is in the list
    BOOLEAN Found = false;
    for (ULONG i = 0; i < pFilter->otPendingShortAddressCount; i++)
    {
        if (aShortAddress == pFilter->otPendingShortAddresses[i])
        {
            // Remove it from the list
            if (i + 1 != pFilter->otPendingShortAddressCount)
                pFilter->otPendingShortAddresses[i] = pFilter->otPendingShortAddresses[pFilter->otPendingShortAddressCount - 1];
            pFilter->otPendingShortAddressCount--;
            Found = TRUE;
            break;
        }
    }

    // Wasn't in the list, return failure
    if (Found == FALSE) return kThreadError_NotFound;

    // Update the miniport
    otPlatRadioSendPendingMacOffload(pFilter);

    return kThreadError_None;
}

ThreadError otPlatRadioClearSrcMatchExtEntry(_In_ otInstance *otCtx, const uint8_t *aExtAddress)
{
    NT_ASSERT(otCtx);
    PMS_FILTER pFilter = otCtxToFilter(otCtx);
    
    // Make sure we are enabled
    NT_ASSERT(pFilter->otPendingMacOffloadEnabled);
    if (pFilter->otPendingMacOffloadEnabled == FALSE)
        return kThreadError_InvalidState;

    // Check to see if it is in the list
    BOOLEAN Found = false;
    for (ULONG i = 0; i < pFilter->otPendingExtendedAddressCount; i++)
    {
        if (memcmp(aExtAddress, &pFilter->otPendingExtendedAddresses[i], sizeof(ULONGLONG)) == 0)
        {
            // Remove it from the list
            if (i + 1 != pFilter->otPendingExtendedAddressCount)
                pFilter->otPendingExtendedAddresses[i] = pFilter->otPendingExtendedAddresses[pFilter->otPendingExtendedAddressCount - 1];
            pFilter->otPendingExtendedAddressCount--;
            Found = TRUE;
            break;
        }
    }
    
    // Wasn't in the list, return failure
    if (Found == FALSE) return kThreadError_NotFound;

    // Update the miniport
    otPlatRadioSendPendingMacOffload(pFilter);

    return kThreadError_None;
}

void otPlatRadioClearSrcMatchShortEntries(_In_ otInstance *otCtx)
{
    NT_ASSERT(otCtx);
    PMS_FILTER pFilter = otCtxToFilter(otCtx);

    // Ignore if we are already in the correct state
    if (pFilter->otPendingShortAddressCount == 0) return;

    // Set the new value and update the miniport
    pFilter->otPendingShortAddressCount = 0;
    otPlatRadioSendPendingMacOffload(pFilter);
}

void otPlatRadioClearSrcMatchExtEntries(_In_ otInstance *otCtx)
{
    NT_ASSERT(otCtx);
    PMS_FILTER pFilter = otCtxToFilter(otCtx);

    // Ignore if we are already in the correct state
    if (pFilter->otPendingExtendedAddressCount == 0) return;

    // Set the new value and update the miniport
    pFilter->otPendingExtendedAddressCount = 0;
    otPlatRadioSendPendingMacOffload(pFilter);
}