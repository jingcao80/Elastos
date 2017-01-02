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

#ifndef __ELASTOS_DROID_NET_DHCP_DHCPACKPACKET_H__
#define __ELASTOS_DROID_NET_DHCP_DHCPACKPACKET_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/dhcp/DhcpPacket.h"

using Elastos::Net::IInet4Address;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Dhcp {

/**
 * This class implements the DHCP-ACK packet.
 */
class DhcpAckPacket
    : public DhcpPacket
{
public:
    DhcpAckPacket();

    CARAPI constructor(
        /* [in] */ Int32 transId,
        /* [in] */ Boolean broadcast,
        /* [in] */ IInetAddress* serverAddress,
        /* [in] */ IInetAddress* clientIp,
        /* [in] */ ArrayOf<Byte>* clientMac);

    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * Fills in a packet with the requested ACK parameters.
     */
    CARAPI BuildPacket(
        /* [in] */ Int32 encap,
        /* [in] */ Int16 destUdp,
        /* [in] */ Int16 srcUdp,
        /* [out] */ IByteBuffer** result);

    /**
     * Adds the optional parameters to the client-generated ACK packet.
     */
    CARAPI FinishPacket(
        /* [in] */ IByteBuffer* buffer);

    /**
     * Notifies the specified state machine of the ACK packet parameters.
     */
    CARAPI DoNextOp(
        /* [in] */ DhcpStateMachine* machine);

private:
    /**
     * Un-boxes an Integer, returning 0 if a null reference is supplied.
     */
    static CARAPI GetInt(
        /* [in] */ IInteger32* v,
        /* [out] */ Int32* result);

private:
    /**
     * The address of the server which sent this packet.
     */
    AutoPtr<IInetAddress> mSrcIp;
};

} // namespace Dhcp
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_DHCP_DHCPACKPACKET_H__
