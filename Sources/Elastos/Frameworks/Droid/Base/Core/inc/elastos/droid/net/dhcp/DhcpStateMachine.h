//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_NET_DHCP_DHCPSTATEMACHINE_H__
#define __ELASTOS_DROID_NET_DHCP_DHCPSTATEMACHINE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Net::IInetAddress;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Dhcp {

/**
 * This class defines the "next steps" which occur after a given DHCP
 * packet has been received.
 */
class DhcpStateMachine
    : public Object
{
public:
    /**
     * Signals that an offer packet has been received with the specified
     * parameters.
     */
    virtual CARAPI OnOfferReceived(
        /* [in] */ Boolean broadcast,
        /* [in] */ Int32 transactionId,
        /* [in] */ ArrayOf<Byte>* myMac,
        /* [in] */ IInetAddress* offeredIpAddress,
        /* [in] */ IInetAddress* serverIpAddress) = 0;

    /**
     * Signals that a NAK packet has been received.
     */
    virtual CARAPI OnNakReceived() = 0;

    /**
     * Signals that the final ACK has been received from the server.
     */
    virtual CARAPI OnAckReceived(
        /* [in] */ IInetAddress* myIpAddress,
        /* [in] */ IInetAddress* myNetMask,
        /* [in] */ IInetAddress* myGateway,
        /* [in] */ IList* myDnsServers,
        /* [in] */ IInetAddress* myDhcpServer,
        /* [in] */ Int32 leaseTime) = 0;

    /**
     * Signals that a client's DISCOVER packet has been received with the
     * specified parameters.
     */
    virtual CARAPI OnDiscoverReceived(
        /* [in] */ Boolean broadcast,
        /* [in] */ Int32 transactionId,
        /* [in] */ ArrayOf<Byte>* clientMac,
        /* [in] */ ArrayOf<Byte>* requestedParameterList) = 0;

    /**
     * Signals that a client's REQUEST packet has been received with the
     * specified parameters.
     */
    virtual CARAPI OnRequestReceived(
        /* [in] */ Boolean broadcast,
        /* [in] */ Int32 transactionId,
        /* [in] */ ArrayOf<Byte>* clientMac,
        /* [in] */ IInetAddress* requestedIp,
        /* [in] */ ArrayOf<Byte>* requestedParams,
        /* [in] */ const String& clientHostName) = 0;

    /**
     * Signals that a client's INFORM packet has been received with the
     * specified parameters.
     */
    virtual CARAPI OnInformReceived(
        /* [in] */ Int32 transactionId,
        /* [in] */ ArrayOf<Byte>* clientMac,
        /* [in] */ IInetAddress* preassignedIp,
        /* [in] */ ArrayOf<Byte>* requestedParams) = 0;

    /**
     * Signals that a client's DECLINE packet has been received with the
     * specified parameters.
     */
    virtual CARAPI OnDeclineReceived(
        /* [in] */ ArrayOf<Byte>* clientMac,
        /* [in] */ IInetAddress* declinedIp) = 0;
};

} // namespace Dhcp
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_DHCP_DHCPSTATEMACHINE_H__
