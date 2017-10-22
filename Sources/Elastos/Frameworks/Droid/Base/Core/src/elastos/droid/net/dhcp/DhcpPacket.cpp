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

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/net/dhcp/DhcpPacket.h"
#include "elastos/droid/net/dhcp/DhcpDiscoverPacket.h"
#include "elastos/droid/net/dhcp/DhcpOfferPacket.h"
#include "elastos/droid/net/dhcp/DhcpRequestPacket.h"
#include "elastos/droid/net/dhcp/DhcpDeclinePacket.h"
#include "elastos/droid/net/dhcp/DhcpAckPacket.h"
#include "elastos/droid/net/dhcp/DhcpNakPacket.h"
#include "elastos/droid/net/dhcp/DhcpInformPacket.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Os::Build;

using Elastos::Core::CInteger32;
using Elastos::Core::CSystem;
using Elastos::Core::IInteger32;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::Charset::IStandardCharsets;
using Elastos::IO::IBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::ByteOrder_BIG_ENDIAN;
using Elastos::IO::IInt16Buffer;
using Elastos::IO::IPrintStream;
using Elastos::Net::CInetAddressHelper;
using Elastos::Net::IInet4Address;
using Elastos::Net::IInet4AddressHelper;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInetAddressHelper;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Dhcp {

const Int32 DhcpPacket::ENCAP_L2 = 0;
const Int32 DhcpPacket::ENCAP_L3 = 1;
const Int32 DhcpPacket::ENCAP_BOOTP = 2;
const Int16 DhcpPacket::DHCP_CLIENT = 68;
const Byte DhcpPacket::IP_TYPE_UDP = 0x11;
const Int16 DhcpPacket::DHCP_SERVER = 67;
const Byte DhcpPacket::DHCP_BOOTREQUEST = 1;
const Byte DhcpPacket::DHCP_BOOTREPLY = 2;
const Byte DhcpPacket::CLIENT_ID_ETHER = 1;
const Byte DhcpPacket::IP_VERSION_HEADER_LEN = 0x45;
const Int16 DhcpPacket::IP_FLAGS_OFFSET = 0x4000;
const Byte DhcpPacket::IP_TOS_LOWDELAY = 0x10;
const Byte DhcpPacket::IP_TTL = 0x40;
const String DhcpPacket::TAG("DhcpPacket");
const Int32 DhcpPacket::MAX_LENGTH = 1500;
const Byte DhcpPacket::DHCP_SUBNET_MASK = 1;
const Byte DhcpPacket::DHCP_ROUTER = 3;
const Byte DhcpPacket::DHCP_DNS_SERVER = 6;
const Byte DhcpPacket::DHCP_HOST_NAME = 12;
const Byte DhcpPacket::DHCP_DOMAIN_NAME = 15;
const Byte DhcpPacket::DHCP_BROADCAST_ADDRESS = 28;
const Byte DhcpPacket::DHCP_REQUESTED_IP = 50;
const Byte DhcpPacket::DHCP_LEASE_TIME = 51;
const Byte DhcpPacket::DHCP_MESSAGE_TYPE = 53;
const Byte DhcpPacket::DHCP_MESSAGE_TYPE_DISCOVER = 1;
const Byte DhcpPacket::DHCP_MESSAGE_TYPE_OFFER = 2;
const Byte DhcpPacket::DHCP_MESSAGE_TYPE_REQUEST = 3;
const Byte DhcpPacket::DHCP_MESSAGE_TYPE_DECLINE = 4;
const Byte DhcpPacket::DHCP_MESSAGE_TYPE_ACK = 5;
const Byte DhcpPacket::DHCP_MESSAGE_TYPE_NAK = 6;
const Byte DhcpPacket::DHCP_MESSAGE_TYPE_INFORM = 8;
const Byte DhcpPacket::DHCP_SERVER_IDENTIFIER = 54;
const Byte DhcpPacket::DHCP_PARAMETER_LIST = 55;
const Byte DhcpPacket::DHCP_MESSAGE = 56;
const Byte DhcpPacket::DHCP_RENEWAL_TIME = 58;
const Byte DhcpPacket::DHCP_VENDOR_CLASS_ID = 60;
const Byte DhcpPacket::DHCP_CLIENT_IDENTIFIER = 61;

DhcpPacket::DhcpPacket()
    : mTransId(0)
    , mBroadcast(FALSE)
{}

ECode DhcpPacket::constructor(
    /* [in] */ Int32 transId,
    /* [in] */ IInetAddress* clientIp,
    /* [in] */ IInetAddress* yourIp,
    /* [in] */ IInetAddress* nextIp,
    /* [in] */ IInetAddress* relayIp,
    /* [in] */ ArrayOf<Byte>* clientMac,
    /* [in] */ Boolean broadcast)
{
    mTransId = transId;
    mClientIp = clientIp;
    mYourIp = yourIp;
    mNextIp = nextIp;
    mRelayIp = relayIp;
    mClientMac = clientMac;
    mBroadcast = broadcast;
    return NOERROR;
}

ECode DhcpPacket::GetTransactionId(
    /* [out] */ Int32* result)
{
    *result = mTransId;
    return NOERROR;
}

ECode DhcpPacket::FillInPacket(
    /* [in] */ Int32 encap,
    /* [in] */ IInetAddress* destIp,
    /* [in] */ IInetAddress* srcIp,
    /* [in] */ Int16 destUdp,
    /* [in] */ Int16 srcUdp,
    /* [in] */ IByteBuffer* buf,
    /* [in] */ Byte requestCode,
    /* [in] */ Boolean broadcast)
{
    AutoPtr<ArrayOf<Byte> > destIpArray;
    destIp->GetAddress((ArrayOf<Byte>**)&destIpArray);
    AutoPtr<ArrayOf<Byte> > srcIpArray;
    srcIp->GetAddress((ArrayOf<Byte>**)&srcIpArray);
    Int32 ipLengthOffset = 0;
    Int32 ipChecksumOffset = 0;
    Int32 endIpHeader = 0;
    Int32 udpHeaderOffset = 0;
    Int32 udpLengthOffset = 0;
    Int32 udpChecksumOffset = 0;
    IBuffer::Probe(buf)->Clear();
    buf->SetOrder(ByteOrder_BIG_ENDIAN);
    // if a full IP packet needs to be generated, put the IP & UDP
    // headers in place, and pre-populate with artificial values
    // needed to seed the IP checksum.
    if (encap == ENCAP_L3) {
        // fake IP header, used in the IP-header checksum
        buf->Put(IP_VERSION_HEADER_LEN);
        buf->Put(IP_TOS_LOWDELAY);    // tos: IPTOS_LOWDELAY
        IBuffer::Probe(buf)->GetPosition(&ipLengthOffset);
        buf->PutInt16(0);  // length
        buf->PutInt16(0);  // id
        buf->PutInt16(IP_FLAGS_OFFSET); // ip offset: don't fragment
        buf->Put(IP_TTL);    // TTL: use default 64 from RFC1340
        buf->Put(IP_TYPE_UDP);
        IBuffer::Probe(buf)->GetPosition(&ipChecksumOffset);
        buf->PutInt16((Int16) 0); // checksum
        buf->Put(srcIpArray);
        buf->Put(destIpArray);
        IBuffer::Probe(buf)->GetPosition(&endIpHeader);
        // UDP header
        IBuffer::Probe(buf)->GetPosition(&udpHeaderOffset);
        buf->PutInt16(srcUdp);
        buf->PutInt16(destUdp);
        IBuffer::Probe(buf)->GetPosition(&udpLengthOffset);
        buf->PutInt16(0); // length
        IBuffer::Probe(buf)->GetPosition(&udpChecksumOffset);
        buf->PutInt16(0); // UDP checksum -- initially zero
    }
    // DHCP payload
    buf->Put(requestCode);
    buf->Put((Byte) 1); // Hardware Type: Ethernet
    buf->Put((Byte) mClientMac->GetLength()); // Hardware Address Length
    buf->Put((Byte) 0); // Hop Count
    buf->PutInt32(mTransId);  // Transaction ID
    buf->PutInt16((Int16) 0); // Elapsed Seconds
    if (broadcast) {
        buf->PutInt16((Int16) 0x8000); // Flags
    } else {
        buf->PutInt16((Int16) 0x0000); // Flags
    }
    AutoPtr< ArrayOf<Byte> > address;
    mClientIp->GetAddress((ArrayOf<Byte>**)&address);
    buf->Put(address);
    address = NULL;
    mYourIp->GetAddress((ArrayOf<Byte>**)&address);
    buf->Put(address);
    address = NULL;
    mNextIp->GetAddress((ArrayOf<Byte>**)&address);
    buf->Put(address);
    address = NULL;
    mRelayIp->GetAddress((ArrayOf<Byte>**)&address);
    buf->Put(address);
    buf->Put(mClientMac);
    Int32 position;
    IBuffer::Probe(buf)->GetPosition(&position);
    IBuffer::Probe(buf)->SetPosition(position +
                 (16 - mClientMac->GetLength()) // pad addr to 16 Bytes
                 + 64     // empty server host name (64 bytes)
                 + 128);  // empty boot file name (128 bytes)
    buf->PutInt32(0x63825363); // magic number
    FinishPacket(buf);
    // round up to an even number of octets
    IBuffer::Probe(buf)->GetPosition(&position);
    if ((position & 1) == 1) {
        buf->Put((Byte) 0);
    }
    // If an IP packet is being built, the IP & UDP checksums must be
    // computed.
    if (encap == ENCAP_L3) {
        // fix UDP header: insert length
        IBuffer::Probe(buf)->GetPosition(&position);
        Int16 udpLen = position - udpHeaderOffset;
        buf->PutInt16(udpLengthOffset, udpLen);
        // fix UDP header: checksum
        // checksum for UDP at udpChecksumOffset
        Int32 udpSeed = 0;
        // apply IPv4 pseudo-header.  Read IP address src and destination
        // values from the IP header and accumulate checksum.
        Int16 shortVal;
        buf->GetInt16(ipChecksumOffset + 2, &shortVal);
        udpSeed += IntAbs(shortVal);
        buf->GetInt16(ipChecksumOffset + 4, &shortVal);
        udpSeed += IntAbs(shortVal);
        buf->GetInt16(ipChecksumOffset + 6, &shortVal);
        udpSeed += IntAbs(shortVal);
        buf->GetInt16(ipChecksumOffset + 8, &shortVal);
        udpSeed += IntAbs(shortVal);
        // accumulate extra data for the pseudo-header
        udpSeed += IP_TYPE_UDP;
        udpSeed += udpLen;
        // and compute UDP checksum
        IBuffer::Probe(buf)->GetPosition(&position);
        buf->PutInt16(udpChecksumOffset, (Int16) Checksum(buf, udpSeed, udpHeaderOffset, position));
        // fix IP header: insert length
        buf->PutInt16(ipLengthOffset, (Int16)position);
        // fixup IP-header checksum
        buf->PutInt16(ipChecksumOffset,
                     (Int16) Checksum(buf, 0, 0, endIpHeader));
    }
    return NOERROR;
}

Int32 DhcpPacket::IntAbs(
    /* [in] */ Int16 v)
{
    if (v < 0) {
        Int32 r = v + 65536;
        return r;
    }
    return v;
}

Int32 DhcpPacket::Checksum(
    /* [in] */ IByteBuffer* buf,
    /* [in] */ Int32 seed,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    Int32 sum = seed;
    Int32 bufPosition;
    IBuffer::Probe(buf)->GetPosition(&bufPosition);
    // set position of original ByteBuffer, so that the ShortBuffer
    // will be correctly initialized
    IBuffer::Probe(buf)->GetPosition(&start);
    AutoPtr<IInt16Buffer> shortBuf;
    buf->AsInt16Buffer((IInt16Buffer**)&shortBuf);
    // re-set ByteBuffer position
    IBuffer::Probe(buf)->GetPosition(&bufPosition);
    AutoPtr<ArrayOf<Int16> > shortArray = ArrayOf<Int16>::Alloc((end - start) / 2);
    shortBuf->Get(shortArray);
    for (Int32 i = 0; i < shortArray->GetLength(); ++i) {
        sum += IntAbs((*shortArray)[i]);
    }
    start += shortArray->GetLength() * 2;
    // see if a singleton byte remains
    if (end != start) {
        Byte b;
        buf->Get(start, &b);
        // make it unsigned
        if (b < 0) {
            b += 256;
        }
        sum += b * 256;
    }
    sum = ((sum >> 16) & 0xFFFF) + (sum & 0xFFFF);
    sum = ((sum + ((sum >> 16) & 0xFFFF)) & 0xFFFF);
    Int32 negated = ~sum;
    return IntAbs((Int16) negated);
}

ECode DhcpPacket::AddTlv(
    /* [in] */ IByteBuffer* buf,
    /* [in] */ Byte type,
    /* [in] */ Byte value)
{
    buf->Put(type);
    buf->Put((Byte) 1);
    buf->Put(value);
    return NOERROR;
}

ECode DhcpPacket::AddTlv(
    /* [in] */ IByteBuffer* buf,
    /* [in] */ Byte type,
    /* [in] */ ArrayOf<Byte>* payload)
{
    if (payload != NULL) {
        buf->Put(type);
        buf->Put((Byte) payload->GetLength());
        buf->Put(payload);
    }
    return NOERROR;
}

ECode DhcpPacket::AddTlv(
    /* [in] */ IByteBuffer* buf,
    /* [in] */ Byte type,
    /* [in] */ IInetAddress* addr)
{
    if (addr != NULL) {
        AutoPtr< ArrayOf<Byte> > address;
        addr->GetAddress((ArrayOf<Byte>**)&address);
        AddTlv(buf, type, address);
    }
    return NOERROR;
}

ECode DhcpPacket::AddTlv(
    /* [in] */ IByteBuffer* buf,
    /* [in] */ Byte type,
    /* [in] */ IList* addrs)
{
    Int32 size;
    if (addrs != NULL && (addrs->GetSize(&size), size > 0)) {
        buf->Put(type);
        buf->Put((Byte)(4 * size));
        AutoPtr<IIterator> it;
        addrs->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr< ArrayOf<Byte> > address;
            IInetAddress::Probe(obj)->GetAddress((ArrayOf<Byte>**)&address);
            buf->Put(address);
        }
    }
    return NOERROR;
}

ECode DhcpPacket::AddTlv(
    /* [in] */ IByteBuffer* buf,
    /* [in] */ Byte type,
    /* [in] */ IInteger32* value)
{
    if (value != NULL) {
        buf->Put(type);
        buf->Put((Byte) 4);
        Int32 i32v;
        value->GetValue(&i32v);
        buf->PutInt32(i32v);
    }
    return NOERROR;
}

ECode DhcpPacket::AddTlv(
    /* [in] */ IByteBuffer* buf,
    /* [in] */ Byte type,
    /* [in] */ const String& str)
{
    if (str != NULL) {
        buf->Put(type);
        buf->Put((Byte) str.GetLength());
        for (Int32 i = 0; i < str.GetLength(); i++) {
            buf->Put((Byte) str.GetChar(i));
        }
    }
    return NOERROR;
}

ECode DhcpPacket::AddTlvEnd(
    /* [in] */ IByteBuffer* buf)
{
    return buf->Put((Byte) 0xFF);
}

ECode DhcpPacket::MacToString(
    /* [in] */ ArrayOf<Byte>* mac,
    /* [out] */ String* result)
{
    String macAddr("");
    for (Int32 i = 0; i < mac->GetLength(); i++) {
        String hexString = String("0") + StringUtils::ToHexString((*mac)[i]);
        // substring operation grabs the last 2 digits: this
        // allows signed bytes to be converted correctly.
        macAddr += hexString.Substring(hexString.GetLength() - 2);
        if (i != (mac->GetLength() - 1)) {
            macAddr += ":";
        }
    }
    *result = macAddr;
    return NOERROR;
}

ECode DhcpPacket::ToString(
    /* [out] */ String* result)
{
    return MacToString(mClientMac, result);
}

ECode DhcpPacket::ReadIpAddress(
    /* [in] */ IByteBuffer* packet,
    /* [out] */ IInetAddress** result)
{
    AutoPtr<ArrayOf<Byte> > ipAddr = ArrayOf<Byte>::Alloc(4);
    packet->Get(ipAddr);
        // try {
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    ECode ec = inetAddressHelper->GetByAddress(ipAddr, result);
        // } catch (UnknownHostException ex) {
            // ipAddr is numeric, so this should not be
            // triggered.  However, if it is, just nullify
    if (FAILED(ec)) {
        if (ec == (ECode)E_UNKNOWN_HOST_EXCEPTION) {
            result = NULL;
        }
        else
            return ec;
    }
        // }
    return NOERROR;
}

ECode DhcpPacket::ReadAsciiString(
    /* [in] */ IByteBuffer* buf,
    /* [in] */ Int32 byteCount,
    /* [out] */ String* result)
{
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(byteCount);
    buf->Get(bytes);
    *result = String(*bytes, 0, bytes->GetLength()/*, IStandardCharsets::US_ASCII*/);
    return NOERROR;
}

ECode DhcpPacket::DecodeFullPacket(
    /* [in] */ IByteBuffer* packet,
    /* [in] */ Int32 pktType,
    /* [out] */ DhcpPacket** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    // bootp parameters
    Int32 transactionId;
    AutoPtr<IInetAddress> clientIp;
    AutoPtr<IInetAddress> yourIp;
    AutoPtr<IInetAddress> nextIp;
    AutoPtr<IInetAddress> relayIp;
    AutoPtr<ArrayOf<Byte> > clientMac;
    AutoPtr<IList> dnsServers;
    CArrayList::New((IList**)&dnsServers);
    AutoPtr<IInetAddress> gateway; // aka router
    AutoPtr<IInteger32> leaseTime;
    AutoPtr<IInetAddress> serverIdentifier;
    AutoPtr<IInetAddress> netMask;
    String message = String(NULL);
    String vendorId = String(NULL);
    AutoPtr<ArrayOf<Byte> > expectedParams;
    String hostName = String(NULL);
    String domainName = String(NULL);
    AutoPtr<IInetAddress> ipSrc;
    AutoPtr<IInetAddress> ipDst;
    AutoPtr<IInetAddress> bcAddr;
    AutoPtr<IInetAddress> requestedIp;
    // dhcp options
    Byte dhcpType = (Byte) 0xFF;
    packet->SetOrder(ByteOrder_BIG_ENDIAN);
    // check to see if we need to parse L2, IP, and UDP encaps
    if (pktType == ENCAP_L2) {
        // System.out.println("buffer len " + packet.limit());
        AutoPtr<ArrayOf<Byte> > l2dst = ArrayOf<Byte>::Alloc(6);
        AutoPtr<ArrayOf<Byte> > l2src = ArrayOf<Byte>::Alloc(6);
        packet->Get(l2dst);
        packet->Get(l2src);
        Int16 l2type;
        packet->GetInt16(&l2type);
        if (l2type != 0x0800) {
            *result = NULL;
            return NOERROR;
        }
    }
    if ((pktType == ENCAP_L2) || (pktType == ENCAP_L3)) {
        // assume l2type is 0x0800, i.e. IP
        Byte ipType;
        packet->Get(&ipType);
        // System.out.println("ipType is " + ipType);
        Byte ipDiffServicesField;
        packet->Get(&ipDiffServicesField);
        Int16 ipTotalLength;
        packet->GetInt16(&ipTotalLength);
        Int16 ipIdentification;
        packet->GetInt16(&ipIdentification);
        Byte ipFlags;
        packet->Get(&ipFlags);
        Byte ipFragOffset;
        packet->Get(&ipFragOffset);
        Byte ipTTL;
        packet->Get(&ipTTL);
        Byte ipProto;
        packet->Get(&ipProto);
        Int16 ipChksm;
        packet->GetInt16(&ipChksm);
        ReadIpAddress(packet, (IInetAddress**)&ipSrc);
        ReadIpAddress(packet, (IInetAddress**)&ipDst);
        if (ipProto != IP_TYPE_UDP) { // UDP
            *result = NULL;
            return NOERROR;
        }
        // assume UDP
        Int16 udpSrcPort;
        packet->GetInt16(&udpSrcPort);
        Int16 udpDstPort;
        packet->GetInt16(&udpDstPort);
        Int16 udpLen;
        packet->GetInt16(&udpLen);
        Int16 udpChkSum;
        packet->GetInt16(&udpChkSum);
        if ((udpSrcPort != DHCP_SERVER) && (udpSrcPort != DHCP_CLIENT)) {
            *result = NULL;
            return NOERROR;
        }
    }
    // assume bootp
    Byte type;
    packet->Get(&type);
    Byte hwType;
    packet->Get(&hwType);
    Byte addrLen;
    packet->Get(&addrLen);
    Byte hops;
    packet->Get(&hops);
    packet->GetInt32(&transactionId);
    Int16 elapsed;
    packet->GetInt16(&elapsed);
    Int16 bootpFlags;
    packet->GetInt16(&bootpFlags);
    Boolean broadcast = (bootpFlags & 0x8000) != 0;
    AutoPtr<ArrayOf<Byte> > ipv4addr = ArrayOf<Byte>::Alloc(4);
    // try {
        AutoPtr<IInetAddressHelper> inetAddressHelper;
        CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
        packet->Get(ipv4addr);
        ECode ec = inetAddressHelper->GetByAddress(ipv4addr, (IInetAddress**)&clientIp);
        if (FAILED(ec)) {
            *result = NULL;
            return ec == (ECode)E_UNKNOWN_HOST_EXCEPTION ? NOERROR : ec;
        }
        packet->Get(ipv4addr);
        ec = inetAddressHelper->GetByAddress(ipv4addr, (IInetAddress**)&yourIp);
        if (FAILED(ec)) {
            *result = NULL;
            return ec == (ECode)E_UNKNOWN_HOST_EXCEPTION ? NOERROR : ec;
        }
        packet->Get(ipv4addr);
        ec = inetAddressHelper->GetByAddress(ipv4addr, (IInetAddress**)&nextIp);
        if (FAILED(ec)) {
            *result = NULL;
            return ec == (ECode)E_UNKNOWN_HOST_EXCEPTION ? NOERROR : ec;
        }
        packet->Get(ipv4addr);
        ec = inetAddressHelper->GetByAddress(ipv4addr, (IInetAddress**)&relayIp);
        if (FAILED(ec)) {
            *result = NULL;
            return ec == (ECode)E_UNKNOWN_HOST_EXCEPTION ? NOERROR : ec;
        }
    // } catch (UnknownHostException ex) {
    // }
    clientMac = ArrayOf<Byte>::Alloc(addrLen);
    packet->Get(clientMac);
    // skip over address padding (16 octets allocated)
    Int32 pos;
    IBuffer::Probe(packet)->GetPosition(&pos);
    IBuffer::Probe(packet)->SetPosition(pos + (16 - addrLen)
                    + 64    // skip server host name (64 chars)
                    + 128); // skip boot file name (128 chars)
    Int32 dhcpMagicCookie;
    packet->GetInt32(&dhcpMagicCookie);
    if (dhcpMagicCookie !=  0x63825363) {
        *result = NULL;
        return NOERROR;
    }
    // parse options
    Boolean notFinishedOptions = TRUE;
    Int32 limit;
    IBuffer::Probe(packet)->GetLimit(&limit);
    while ((IBuffer::Probe(packet)->GetPosition(&pos), pos < limit) && notFinishedOptions) {
        Byte optionType;
        packet->Get(&optionType);
        if (optionType == (Byte) 0xFF) {
            notFinishedOptions = FALSE;
        } else {
            Byte optionLen;
            packet->Get(&optionLen);
            Int32 expectedLen = 0;
            switch(optionType) {
                case DHCP_SUBNET_MASK:
                    ReadIpAddress(packet, (IInetAddress**)&netMask);
                    expectedLen = 4;
                    break;
                case DHCP_ROUTER:
                    ReadIpAddress(packet, (IInetAddress**)&gateway);
                    expectedLen = 4;
                    break;
                case DHCP_DNS_SERVER:
                    expectedLen = 0;
                    for (expectedLen = 0; expectedLen < optionLen;
                         expectedLen += 4) {
                        AutoPtr<IInetAddress> ipAddress;
                        ReadIpAddress(packet, (IInetAddress**)&ipAddress);
                        dnsServers->Add(ipAddress);
                    }
                    break;
                case DHCP_HOST_NAME:
                    expectedLen = optionLen;
                    ReadAsciiString(packet, optionLen, &hostName);
                    break;
                case DHCP_DOMAIN_NAME:
                    expectedLen = optionLen;
                    ReadAsciiString(packet, optionLen, &domainName);
                    break;
                case DHCP_BROADCAST_ADDRESS:
                    ReadIpAddress(packet, (IInetAddress**)&bcAddr);
                    expectedLen = 4;
                    break;
                case DHCP_REQUESTED_IP:
                    ReadIpAddress(packet, (IInetAddress**)&requestedIp);
                    expectedLen = 4;
                    break;
                case DHCP_LEASE_TIME: {
                    Int16 i16v;
                    packet->GetInt16(&i16v);
                    CInteger32::New(i16v, (IInteger32**)&leaseTime);
                    expectedLen = 4;
                    break;
                }
                case DHCP_MESSAGE_TYPE:
                    packet->Get(&dhcpType);
                    expectedLen = 1;
                    break;
                case DHCP_SERVER_IDENTIFIER:
                    ReadIpAddress(packet, (IInetAddress**)&serverIdentifier);
                    expectedLen = 4;
                    break;
                case DHCP_PARAMETER_LIST:
                    expectedParams = ArrayOf<Byte>::Alloc(optionLen);
                    packet->Get(expectedParams);
                    expectedLen = optionLen;
                    break;
                case DHCP_MESSAGE:
                    expectedLen = optionLen;
                    ReadAsciiString(packet, optionLen, &message);
                    break;
                case DHCP_VENDOR_CLASS_ID:
                    expectedLen = optionLen;
                    ReadAsciiString(packet, optionLen, &vendorId);
                    break;
                case DHCP_CLIENT_IDENTIFIER: { // Client identifier
                    AutoPtr<ArrayOf<Byte> > id = ArrayOf<Byte>::Alloc(optionLen);
                    packet->Get(id);
                    expectedLen = optionLen;
                } break;
                default:
                    // ignore any other parameters
                    for (Int32 i = 0; i < optionLen; i++) {
                        expectedLen++;
                        Byte throwaway;
                        packet->Get(&throwaway);
                    }
            }
            if (expectedLen != optionLen) {
                *result = NULL;
                return NOERROR;
            }
        }
    }
    AutoPtr<DhcpPacket> newPacket;
    switch(dhcpType) {
        case (Byte)-1: {
            *result = NULL;
            return NOERROR;
        }
        case DHCP_MESSAGE_TYPE_DISCOVER:
            newPacket = new DhcpDiscoverPacket();
            ((DhcpDiscoverPacket*)newPacket.Get())->constructor(transactionId, clientMac, broadcast);
            break;
        case DHCP_MESSAGE_TYPE_OFFER:
            newPacket = new DhcpOfferPacket();
            ((DhcpOfferPacket*)newPacket.Get())->constructor(transactionId, broadcast, ipSrc, yourIp, clientMac);
            break;
        case DHCP_MESSAGE_TYPE_REQUEST:
            newPacket = new DhcpRequestPacket();
            ((DhcpRequestPacket*)newPacket.Get())->constructor(transactionId, clientIp, clientMac, broadcast);
            break;
        case DHCP_MESSAGE_TYPE_DECLINE:
            newPacket = new DhcpDeclinePacket();
            ((DhcpDeclinePacket*)newPacket.Get())->constructor(transactionId, clientIp, yourIp, nextIp, relayIp, clientMac);
            break;
        case DHCP_MESSAGE_TYPE_ACK:
            newPacket = new DhcpAckPacket();
            ((DhcpAckPacket*)newPacket.Get())->constructor(transactionId, broadcast, ipSrc, yourIp, clientMac);
            break;
        case DHCP_MESSAGE_TYPE_NAK:
            newPacket = new DhcpNakPacket();
            ((DhcpNakPacket*)newPacket.Get())->constructor(transactionId, clientIp, yourIp, nextIp, relayIp, clientMac);
            break;
        case DHCP_MESSAGE_TYPE_INFORM:
            newPacket = new DhcpInformPacket();
            ((DhcpInformPacket*)newPacket.Get())->constructor(transactionId, clientIp, yourIp, nextIp, relayIp, clientMac);
            break;
        default:
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            AutoPtr<IPrintStream> printStream;
            system->GetOut((IPrintStream**)&printStream);
            String s("Unimplemented type: ");
            s.AppendFormat("%d", dhcpType);
            printStream->Println(s);
            *result = NULL;
            return NOERROR;
    }
    newPacket->mBroadcastAddress = bcAddr;
    newPacket->mDnsServers = dnsServers;
    newPacket->mDomainName = domainName;
    newPacket->mGateway = gateway;
    newPacket->mHostName = hostName;
    newPacket->mLeaseTime = leaseTime;
    newPacket->mMessage = message;
    newPacket->mRequestedIp = requestedIp;
    newPacket->mRequestedParams = expectedParams;
    newPacket->mServerIdentifier = serverIdentifier;
    newPacket->mSubnetMask = netMask;
    *result = newPacket;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode DhcpPacket::DecodeFullPacket(
    /* [in] */ ArrayOf<Byte>* packet,
    /* [in] */ Int32 pktType,
    /* [out] */ DhcpPacket** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IByteBufferHelper> helper;
    AutoPtr<IByteBuffer> buffer;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
    helper->Wrap(packet, (IByteBuffer**)&buffer);
    buffer->SetOrder(ByteOrder_BIG_ENDIAN);
    return DecodeFullPacket(buffer, pktType, result);
}

ECode DhcpPacket::BuildDiscoverPacket(
    /* [in] */ Int32 encap,
    /* [in] */ Int32 transactionId,
    /* [in] */ ArrayOf<Byte>* clientMac,
    /* [in] */ Boolean broadcast,
    /* [in] */ ArrayOf<Byte>* expectedParams,
    /* [out] */ IByteBuffer** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<DhcpPacket> pkt = new DhcpDiscoverPacket();
    ((DhcpDiscoverPacket*)pkt.Get())->constructor(transactionId, clientMac, broadcast);
    pkt->mRequestedParams = expectedParams;
    return pkt->BuildPacket(encap, DHCP_SERVER, DHCP_CLIENT, result);
}

ECode DhcpPacket::BuildOfferPacket(
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
    /* [out] */ IByteBuffer** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<DhcpPacket> pkt = new DhcpOfferPacket();
    ((DhcpOfferPacket*)pkt.Get())->constructor(transactionId, broadcast, serverIpAddr, clientIpAddr, mac);
    pkt->mGateway = gateway;
    pkt->mDnsServers = dnsServers;
    pkt->mLeaseTime = timeout;
    pkt->mDomainName = domainName;
    pkt->mServerIdentifier = dhcpServerIdentifier;
    pkt->mSubnetMask = netMask;
    pkt->mBroadcastAddress = bcAddr;
    return pkt->BuildPacket(encap, DHCP_CLIENT, DHCP_SERVER, result);
}

ECode DhcpPacket::BuildAckPacket(
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
    /* [out] */ IByteBuffer** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<DhcpPacket> pkt = new DhcpAckPacket();
    ((DhcpAckPacket*)pkt.Get())->constructor(transactionId, broadcast, serverIpAddr, clientIpAddr, mac);
    pkt->mGateway = gateway;
    pkt->mDnsServers = dnsServers;
    pkt->mLeaseTime = timeout;
    pkt->mDomainName = domainName;
    pkt->mSubnetMask = netMask;
    pkt->mServerIdentifier = dhcpServerIdentifier;
    pkt->mBroadcastAddress = bcAddr;
    return pkt->BuildPacket(encap, DHCP_CLIENT, DHCP_SERVER, result);
}

ECode DhcpPacket::BuildNakPacket(
    /* [in] */ Int32 encap,
    /* [in] */ Int32 transactionId,
    /* [in] */ IInetAddress* serverIpAddr,
    /* [in] */ IInetAddress* clientIpAddr,
    /* [in] */ ArrayOf<Byte>* mac,
    /* [out] */ IByteBuffer** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<DhcpPacket> pkt = new DhcpNakPacket();
    ((DhcpNakPacket*)pkt.Get())->constructor(transactionId, clientIpAddr, serverIpAddr, serverIpAddr, serverIpAddr, mac);
    pkt->mMessage = "requested address not available";
    pkt->mRequestedIp = clientIpAddr;
    return pkt->BuildPacket(encap, DHCP_CLIENT, DHCP_SERVER, result);
}

ECode DhcpPacket::BuildRequestPacket(
    /* [in] */ Int32 encap,
    /* [in] */ Int32 transactionId,
    /* [in] */ IInetAddress* clientIp,
    /* [in] */ Boolean broadcast,
    /* [in] */ ArrayOf<Byte>* clientMac,
    /* [in] */ IInetAddress* requestedIpAddress,
    /* [in] */ IInetAddress* serverIdentifier,
    /* [in] */ ArrayOf<Byte>* requestedParams,
    /* [in] */ const String& hostName,
    /* [out] */ IByteBuffer** result)
{
    VALIDATE_NOT_NULL(result)

        AutoPtr<DhcpPacket> pkt = new DhcpRequestPacket();
        ((DhcpRequestPacket*)pkt.Get())->constructor(transactionId, clientIp, clientMac, broadcast);
        pkt->mRequestedIp = requestedIpAddress;
        pkt->mServerIdentifier = serverIdentifier;
        pkt->mHostName = hostName;
        pkt->mRequestedParams = requestedParams;
        return pkt->BuildPacket(encap, DHCP_SERVER, DHCP_CLIENT, result);
}

} // namespace Dhcp
} // namespace Net
} // namespace Droid
} // namespace Elastos
