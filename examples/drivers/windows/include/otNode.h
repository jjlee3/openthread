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
 *  This file defines a node interface for openthread.h to be used for certification tests
 */

#ifndef OTNODE_H_
#define OTNODE_H_

#include <openthread.h>

#ifndef OTNODEAPI
#define OTNODEAPI __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represents a virtual node for an openthread interface
 */
typedef struct otNode otNode;

/**
 * Logs a WPP message
 */
OTNODEAPI int32_t OTCALL otNodeLog(const char *aMessage);

/**
 * Allocates a new virtual node
 */
OTNODEAPI otNode* OTCALL otNodeInit(uint32_t id);

/**
 * Frees a node
 */
OTNODEAPI int32_t OTCALL otNodeFinalize(otNode* aNode);

/**
 * Sets the link mode
 */
OTNODEAPI int32_t OTCALL otNodeSetMode(otNode* aNode, const char *aMode);

/**
 * Starts the node
 */
OTNODEAPI int32_t OTCALL otNodeStart(otNode* aNode);

/**
 * Stops the node
 */
OTNODEAPI int32_t OTCALL otNodeStop(otNode* aNode);

/**
 * Clears the node's whitelist
 */
OTNODEAPI int32_t OTCALL otNodeClearWhitelist(otNode* aNode);

/**
 * Enables the node's whitelist
 */
OTNODEAPI int32_t OTCALL otNodeEnableWhitelist(otNode* aNode);

/**
 * Disables the node's whitelist
 */
OTNODEAPI int32_t OTCALL otNodeDisableWhitelist(otNode* aNode);

/**
 * Adds an entry to the node's whitelist
 */
OTNODEAPI int32_t OTCALL otNodeAddWhitelist(otNode* aNode, const char *aExtAddr, int8_t aRssi);

/**
 * Removes an entry to the node's whitelist
 */
OTNODEAPI int32_t OTCALL otNodeRemoveWhitelist(otNode* aNode, const char *aExtAddr);

/**
 * Gets the node's short mac address (Rloc16)
 */
OTNODEAPI uint16_t OTCALL otNodeGetAddr16(otNode* aNode);

/**
 * Gets the node's extended mac address
 */
OTNODEAPI const char* OTCALL otNodeGetAddr64(otNode* aNode);

/**
 * Sets the channel for the node
 */
OTNODEAPI int32_t OTCALL otNodeSetChannel(otNode* aNode, uint8_t aChannel);

/**
 * Gets the key sequance for the node
 */
OTNODEAPI uint32_t OTCALL otNodeGetKeySequence(otNode* aNode);

/**
 * Sets the network id timeout for the node
 */
OTNODEAPI int32_t OTCALL otNodeSetKeySequence(otNode* aNode, uint32_t aSequence);

/**
 * Sets the network id timeout for the node
 */
OTNODEAPI int32_t OTCALL otNodeSetNetworkIdTimeout(otNode* aNode, uint8_t aTimeout);

/**
 * Sets the network name for the node
 */
OTNODEAPI int32_t OTCALL otNodeSetNetworkName(otNode* aNode, const char *aName);

/**
 * Gets the pan id for the node
 */
OTNODEAPI uint16_t OTCALL otNodeGetPanId(otNode* aNode);

/**
 * Sets the pan id for the node
 */
OTNODEAPI int32_t OTCALL otNodeSetPanId(otNode* aNode, uint16_t aPanId);

/**
 * Sets the router upgrade threshold for the node
 */
OTNODEAPI int32_t OTCALL otNodeSetRouterUpgradeThreshold(otNode* aNode, uint8_t aThreshold);

/**
 * Releases a router id for the node
 */
OTNODEAPI int32_t OTCALL otNodeReleaseRouterId(otNode* aNode, uint8_t aRouterId);

/**
 * Gets the node's state
 */
OTNODEAPI const char* OTCALL otNodeGetState(otNode* aNode);

/**
 * Sets the node's state
 */
OTNODEAPI int32_t OTCALL otNodeSetState(otNode* aNode, const char *aState);

/**
 * Gets the child timeout for the node
 */
OTNODEAPI uint32_t OTCALL otNodeGetTimeout(otNode* aNode);

/**
 * Sets the child timeout for the node
 */
OTNODEAPI int32_t OTCALL otNodeSetTimeout(otNode* aNode, uint32_t aTimeout);

/**
 * Gets the leader weight for the node
 */
OTNODEAPI uint8_t OTCALL otNodeGetWeight(otNode* aNode);

/**
 * Sets the leader weight for the node
 */
OTNODEAPI int32_t OTCALL otNodeSetWeight(otNode* aNode, uint8_t aWeight);

/**
 * Adds an IP address for the node
 */
OTNODEAPI int32_t OTCALL otNodeAddIpAddr(otNode* aNode, const char *aAddr);

/**
 * Gets the IP address for the node
 */
OTNODEAPI const char* OTCALL otNodeGetAddrs(otNode* aNode);

/**
 * Gets the context reuse delay for the node
 */
OTNODEAPI uint32_t OTCALL otNodeGetContextReuseDelay(otNode* aNode);

/**
 * Sets the context reuse delay for the node
 */
OTNODEAPI int32_t OTCALL otNodeSetContextReuseDelay(otNode* aNode, uint32_t aDelay);

/**
 * Adds an IP prefix for the node
 */
OTNODEAPI int32_t OTCALL otNodeAddPrefix(otNode* aNode, const char *aPrefix, const char *aFlags, const char *aPreference);

/**
 * Removes an IP prefix from the node
 */
OTNODEAPI int32_t OTCALL otNodeRemovePrefix(otNode* aNode, const char *aPrefix);

/**
 * Adds an IP route for the node
 */
OTNODEAPI int32_t OTCALL otNodeAddRoute(otNode* aNode, const char *aPrefix, const char *aPreference);

/**
 * Removes an IP route from the node
 */
OTNODEAPI int32_t OTCALL otNodeRemoveRoute(otNode* aNode, const char *aPrefix);

/**
 * Registers the net data for the node
 */
OTNODEAPI int32_t OTCALL otNodeRegisterNetdata(otNode* aNode);

/**
 * Performs an energy scan for the node
 */
OTNODEAPI int32_t OTCALL otNodeEnergyScan(otNode* aNode, uint32_t aMask, uint8_t aCount, uint16_t aPeriod, uint16_t aDuration, const char *aAddr);

/**
 * Performs a panid query for the node
 */
OTNODEAPI int32_t OTCALL otNodePanIdQuery(otNode* aNode, uint16_t aPanId, uint32_t aMask, const char *aAddr);

/**
 * Performs an scan for the node
 */
OTNODEAPI const char* OTCALL otNodeScan(otNode* aNode);

/**
 * Performs an scan for the node
 */
OTNODEAPI uint32_t OTCALL otNodePing(otNode* aNode, const char *aAddr, uint16_t aSize, uint32_t aMinReplies);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OTNODE_H_
