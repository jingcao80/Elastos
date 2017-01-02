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

#ifndef __ELASTOS_DROID_NET_DHCP_DHCPPACKET_H__
#define __ELASTOS_DROID_NET_DHCP_DHCPPACKET_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/dhcp/DhcpStateMachine.h"

using Elastos::Core::IInteger32;
using Elastos::IO::IByteBuffer;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInetAddress;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Dhcp {

/**
 * Defines basic data and operations needed to build and use packets for the
 * DHCP protocol.  Subclasses create the specific packets used at each
 * stage of the negotiation.
 */
class DhcpPacket
    : public Object
{
public:
    DhcpPacket();

    CARAPI constructor(
        /* [in] */ Int32 transId,
        /* [in] */ IInetAddress* clientIp,
        /* [in] */ IInetAddress* yourIp,
        /* [in] */ IInetAddress* nextIp,
        /* [in] */ IInetAddress* relayIp,
        /* [in] */ ArrayOf<Byte>* clientMac,
        /* [in] */ Boolean broadcast);

    /**
     * Asks the packet object to signal the next operation in the DHCP
     * protocol.  The available actions are methods defined in the
     * DhcpStateMachine interface.
     */
    virtual CARAPI DoNextOp(
        /* [in] */ DhcpStateMachine* stateMachine) = 0;

    /**
     * Asks the packet object to create a ByteBuffer serialization of
     * the packet for transmission.
     */
    virtual CARAPI BuildPacket(
        /* [in] */ Int32 encap,
        /* [in] */ Int16 destUdp,
        /* [in] */ Int16 srcUdp,
        /* [out] */ IByteBuffer** result) = 0;

    /**
     * Allows the concrete class to fill in packet-type-specific details,
     * typically optional parameters at the end of the packet.
     */
    virtual CARAPI FinishPacket(
        /* [in] */ IByteBuffer* buffer) = 0;

    /**
     * Returns the transaction ID.
     */
    CARAPI GetTransactionId(
        /* [out] */ Int32* result);

    /**
     * Creates a new L3 packet (including IP header) containing the
     * DHCP udp packet.  This method relies upon the delegated method
     * finishPacket() to insert the per-packet contents.
     */
    CARAPI FillInPacket(
        /* [in] */ Int32 encap,
        /* [in] */ IInetAddress* destIp,
        /* [in] */ IInetAddress* srcIp,
        /* [in] */ Int16 destUdp,
        /* [in] */ Int16 srcUdp,
        /* [in] */ IByteBuffer* buf,
        /* [in] */ Byte requestCode,
        /* [in] */ Boolean broadcast);

    /**
     * Adds an optional parameter containing a single byte value.
     */
    CARAPI AddTlv(
        /* [in] */ IByteBuffer* buf,
        /* [in] */ Byte type,
        /* [in] */ Byte value);

    /**
     * Adds an optional parameter containing an array of bytes.
     */
    CARAPI AddTlv(
        /* [in] */ IByteBuffer* buf,
        /* [in] */ Byte type,
        /* [in] */ ArrayOf<Byte>* payload);

    /**
     * Adds an optional parameter containing an IP address.
     */
    CARAPI AddTlv(
        /* [in] */ IByteBuffer* buf,
        /* [in] */ Byte type,
        /* [in] */ IInetAddress* addr);

    /**
     * Adds an optional parameter containing a list of IP addresses.
     */
    CARAPI AddTlv(
        /* [in] */ IByteBuffer* buf,
        /* [in] */ Byte type,
        /* [in] */ IList* addrs);

    /**
     * Adds an optional parameter containing a simple integer
     */
    CARAPI AddTlv(
        /* [in] */ IByteBuffer* buf,
        /* [in] */ Byte type,
        /* [in] */ IInteger32* value);

    /**
     * Adds an optional parameter containing and ASCII string.
     */
    CARAPI AddTlv(
        /* [in] */ IByteBuffer* buf,
        /* [in] */ Byte type,
        /* [in] */ const String& str);

    /**
     * Adds the special end-of-optional-parameters indicator.
     */
    CARAPI AddTlvEnd(
        /* [in] */ IByteBuffer* buf);

    /**
     * Converts a MAC from an array of octets to an ASCII string.
     */
    static CARAPI MacToString(
        /* [in] */ ArrayOf<Byte>* mac,
        /* [out] */ String* result);

    virtual CARAPI ToString(
        /* [out] */ String* result);

    /**
     * Creates a concrete DhcpPacket from the supplied ByteBuffer.  The
     * buffer may have an L2 encapsulation (which is the full EthernetII
     * format starting with the source-address MAC) or an L3 encapsulation
     * (which starts with the IP header).
     * <br>
     * A subset of the optional parameters are parsed and are stored
     * in object fields.
     */
    static CARAPI DecodeFullPacket(
        /* [in] */ IByteBuffer* packet,
        /* [in] */ Int32 pktType,
        /* [out] */ DhcpPacket** result);

    /**
     * Parse a packet from an array of bytes.
     */
    static CARAPI DecodeFullPacket(
        /* [in] */ ArrayOf<Byte>* packet,
        /* [in] */ Int32 pktType,
        /* [out] */ DhcpPacket** result);

    /**
     * Builds a DHCP-DISCOVER packet from the required specified
     * parameters.
     */
    static CARAPI BuildDiscoverPacket(
        /* [in] */ Int32 encap,
        /* [in] */ Int32 transactionId,
        /* [in] */ ArrayOf<Byte>* clientMac,
        /* [in] */ Boolean broadcast,
        /* [in] */ ArrayOf<Byte>* expectedParams,
        /* [out] */ IByteBuffer** result);

    /**
     * Builds a DHCP-OFFER packet from the required specified
     * parameters.
     */
    static CARAPI BuildOfferPacket(
        /* [in] */ Int32 encap,
        /* [in] */ Int32 transactionId,
        /* [in] */ Boolean broadcast,
        /* [in] */ IInetAddress* serverIpAddr,
        /* [in] */ IInetAddress* clientIpAddr,
        /* [in] */ ArrayOf<Byte>* mac,
        /* [in] */ IInteger32* timeout,
        /* [in] */ IInetAddress* netMask,
        /* [in] */ IInetAddress* bcAddr,
        /* [in] */ IInetAddress* gateway,
        /* [in] */ IList* dnsServers,
        /* [in] */ IInetAddress* dhcpServerIdentifier,
        /* [in] */ const String& domainName,
        /* [out] */ IByteBuffer** result);

    /**
     * Builds a DHCP-ACK packet from the required specified parameters.
     */
    static CARAPI BuildAckPacket(
        /* [in] */ Int32 encap,
        /* [in] */ Int32 transactionId,
        /* [in] */ Boolean broadcast,
        /* [in] */ IInetAddress* serverIpAddr,
        /* [in] */ IInetAddress* clientIpAddr,
        /* [in] */ ArrayOf<Byte>* mac,
        /* [in] */ IInteger32* timeout,
        /* [in] */ IInetAddress* netMask,
        /* [in] */ IInetAddress* bcAddr,
        /* [in] */ IInetAddress* gateway,
        /* [in] */ IList* dnsServers,
        /* [in] */ IInetAddress* dhcpServerIdentifier,
        /* [in] */ const String& domainName,
        /* [out] */ IByteBuffer** result);

    /**
     * Builds a DHCP-NAK packet from the required specified parameters.
     */
    static CARAPI BuildNakPacket(
        /* [in] */ Int32 encap,
        /* [in] */ Int32 transactionId,
        /* [in] */ IInetAddress* serverIpAddr,
        /* [in] */ IInetAddress* clientIpAddr,
        /* [in] */ ArrayOf<Byte>* mac,
        /* [out] */ IByteBuffer** result);

    /**
     * Builds a DHCP-REQUEST packet from the required specified parameters.
     */
    static CARAPI BuildRequestPacket(
        /* [in] */ Int32 encap,
        /* [in] */ Int32 transactionId,
        /* [in] */ IInetAddress* clientIp,
        /* [in] */ Boolean broadcast,
        /* [in] */ ArrayOf<Byte>* clientMac,
        /* [in] */ IInetAddress* requestedIpAddress,
        /* [in] */ IInetAddress* serverIdentifier,
        /* [in] */ ArrayOf<Byte>* requestedParams,
        /* [in] */ const String& hostName,
        /* [out] */ IByteBuffer** result);

private:
    /**
     * Converts a signed short value to an unsigned int value.  Needed
     * because Java does not have unsigned types.
     */
    CARAPI_(Int32) IntAbs(
        /* [in] */ Int16 v);

    /**
     * Performs an IP checksum (used in IP header and across UDP
     * payload) on the specified portion of a ByteBuffer.  The seed
     * allows the checksum to commence with a specified value.
     */
    CARAPI_(Int32) Checksum(
        /* [in] */ IByteBuffer* buf,
        /* [in] */ Int32 seed,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * Reads a four-octet value from a ByteBuffer and construct
     * an IPv4 address from that value.
     */
    static CARAPI ReadIpAddress(
        /* [in] */ IByteBuffer* packet,
        /* [out] */ IInetAddress** result);

    /**
     * Reads a string of specified length from the buffer.
     */
    static CARAPI ReadAsciiString(
        /* [in] */ IByteBuffer* buf,
        /* [in] */ Int32 byteCount,
        /* [out] */ String* result);

public:
    /**
     * Packet encapsulations.
     */
    static const Int32 ENCAP_L2;    // EthernetII header included
    static const Int32 ENCAP_L3;    // IP/UDP header included
    static const Int32 ENCAP_BOOTP; // BOOTP contents only

    /**
     * The maximum length of a packet that can be constructed.
     */
    static const Int32 MAX_LENGTH;

    /**
     * DHCP Optional Type: DHCP Subnet Mask
     */
    static const Byte DHCP_SUBNET_MASK;

    AutoPtr<IInetAddress> mSubnetMask;

    /**
     * DHCP Optional Type: DHCP Router
     */
    static const Byte DHCP_ROUTER;

    AutoPtr<IInetAddress> mGateway;

    /**
     * DHCP Optional Type: DHCP DNS Server
     */
    static const Byte DHCP_DNS_SERVER;

    AutoPtr<IList> mDnsServers;

    /**
     * DHCP Optional Type: DHCP Host Name
     */
    static const Byte DHCP_HOST_NAME;

    String mHostName;

    /**
     * DHCP Optional Type: DHCP DOMAIN NAME
     */
    static const Byte DHCP_DOMAIN_NAME;

    String mDomainName;

    /**
     * DHCP Optional Type: DHCP BROADCAST ADDRESS
     */
    static const Byte DHCP_BROADCAST_ADDRESS;

    AutoPtr<IInetAddress> mBroadcastAddress;

    /**
     * DHCP Optional Type: DHCP Requested IP Address
     */
    static const Byte DHCP_REQUESTED_IP;

    AutoPtr<IInetAddress> mRequestedIp;

    /**
     * DHCP Optional Type: DHCP Lease Time
     */
    static const Byte DHCP_LEASE_TIME;

    AutoPtr<IInteger32> mLeaseTime;

    /**
     * DHCP Optional Type: DHCP Message Type
     */
    static const Byte DHCP_MESSAGE_TYPE;

    // the actual type values
    static const Byte DHCP_MESSAGE_TYPE_DISCOVER;

    static const Byte DHCP_MESSAGE_TYPE_OFFER;

    static const Byte DHCP_MESSAGE_TYPE_REQUEST;

    static const Byte DHCP_MESSAGE_TYPE_DECLINE;

    static const Byte DHCP_MESSAGE_TYPE_ACK;

    static const Byte DHCP_MESSAGE_TYPE_NAK;

    static const Byte DHCP_MESSAGE_TYPE_INFORM;

    /**
     * DHCP Optional Type: DHCP Server Identifier
     */
    static const Byte DHCP_SERVER_IDENTIFIER;

    AutoPtr<IInetAddress> mServerIdentifier;

    /**
     * DHCP Optional Type: DHCP Parameter List
     */
    static const Byte DHCP_PARAMETER_LIST;

    AutoPtr<ArrayOf<Byte> > mRequestedParams;

    /**
     * DHCP Optional Type: DHCP MESSAGE
     */
    static const Byte DHCP_MESSAGE;

    String mMessage;

    /**
     * DHCP Optional Type: DHCP Renewal Time Value
     */
    static const Byte DHCP_RENEWAL_TIME;

    /**
     * DHCP Optional Type: Vendor Class Identifier
     */
    static const Byte DHCP_VENDOR_CLASS_ID;

    /**
     * DHCP Optional Type: DHCP Client Identifier
     */
    static const Byte DHCP_CLIENT_IDENTIFIER;

    /**
     * The transaction identifier used in this particular DHCP negotiation
     */
    Int32 mTransId;

    /**
     * The IP address of the client host.  This address is typically
     * proposed by the client (from an earlier DHCP negotiation) or
     * supplied by the server.
     */
    /* const */ AutoPtr<IInetAddress> mClientIp;

    /* const */ AutoPtr<IInetAddress> mYourIp;

    /**
     * Does the client request a broadcast response?
     */
    Boolean mBroadcast;

    /**
     * The six-octet MAC of the client.
     */
    /* const */ AutoPtr<ArrayOf<Byte> > mClientMac;

    /**
     * The client DHCP port.
     */
    static const Int16 DHCP_CLIENT;

    /**
     * The server DHCP port.
     */
    static const Int16 DHCP_SERVER;

protected:
    static const String TAG;

    /**
     * The message op code indicating a request from a client.
     */
    static const Byte DHCP_BOOTREQUEST;

    /**
     * The message op code indicating a response from the server.
     */
    static const Byte DHCP_BOOTREPLY;

    /**
     * The code type used to identify an Ethernet MAC address in the
     * Client-ID field.
     */
    static const Byte CLIENT_ID_ETHER;

private:
    /**
     * IP layer definitions.
     */
    static const Byte IP_TYPE_UDP;

    /**
     * IP: Version 4, Header Length 20 bytes
     */
    static const Byte IP_VERSION_HEADER_LEN;

    /**
     * IP: Flags 0, Fragment Offset 0, Don't Fragment
     */
    static const Int16 IP_FLAGS_OFFSET;

    /**
     * IP: TOS
     */
    static const Byte IP_TOS_LOWDELAY;

    /**
     * IP: TTL -- use default 64 from RFC1340
     */
    static const Byte IP_TTL;

    /* const */ AutoPtr<IInetAddress> mNextIp;

    /* const */ AutoPtr<IInetAddress> mRelayIp;
};

} // namespace Dhcp
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_DHCP_DHCPPACKET_H__
