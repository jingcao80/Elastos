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
#include "elastos/droid/net/dhcp/DhcpOfferPacket.h"
#include "elastos/droid/net/dhcp/DhcpPacket.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Os::Build;

using Elastos::Core::StringUtils;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::Net::CInet4AddressHelper;
using Elastos::Net::IInet4Address;
using Elastos::Net::IInet4AddressHelper;
using Elastos::Net::IInetAddress;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Dhcp {

DhcpOfferPacket::DhcpOfferPacket()
{}

ECode DhcpOfferPacket::constructor(
    /* [in] */ Int32 transId,
    /* [in] */ Boolean broadcast,
    /* [in] */ IInetAddress* serverAddress,
    /* [in] */ IInetAddress* clientIp,
    /* [in] */ ArrayOf<Byte>* clientMac)
{
    AutoPtr<IInetAddress> any;
    AutoPtr<IInet4AddressHelper> inet4AddressHelper;
    CInet4AddressHelper::AcquireSingleton((IInet4AddressHelper**)&inet4AddressHelper);
    inet4AddressHelper->GetANY((IInetAddress**)&any);
    DhcpPacket::constructor(transId, any, clientIp, any, any, clientMac, broadcast);
    mSrcIp = serverAddress;
    return NOERROR;
}

ECode DhcpOfferPacket::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String s;
    DhcpPacket::ToString(&s);
    String dnsServers(", DNS servers: ");
    if (mDnsServers != NULL) {
        FOR_EACH(iter, mDnsServers) {
            AutoPtr<IInetAddress> dnsServer = IInetAddress::Probe(Ptr(iter)->Func(iter->GetNext));
            String sDnsServer;
            IObject::Probe(dnsServer)->ToString(&sDnsServer);
            dnsServers += sDnsServer + " ";
        }
    }
    *result = s + " OFFER, ip " + Object::ToString(mYourIp) + ", mask " + Object::ToString(mSubnetMask) +
            dnsServers + ", gateway " + Object::ToString(mGateway) +
            " lease time " + Object::ToString(mLeaseTime) + ", domain " + mDomainName;
    return NOERROR;
}

ECode DhcpOfferPacket::BuildPacket(
    /* [in] */ Int32 encap,
    /* [in] */ Int16 destUdp,
    /* [in] */ Int16 srcUdp,
    /* [out] */ IByteBuffer** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IByteBuffer> rev;
    AutoPtr<IByteBufferHelper> byteBufferHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&byteBufferHelper);
    byteBufferHelper->Allocate(MAX_LENGTH, (IByteBuffer**)&rev);
    AutoPtr<IInet4AddressHelper> inet4AddressHelper;
    CInet4AddressHelper::AcquireSingleton((IInet4AddressHelper**)&inet4AddressHelper);
    AutoPtr<IInetAddress> destIp = mBroadcast ? Ptr(inet4AddressHelper)->Func(inet4AddressHelper->GetALL) : mYourIp;
    AutoPtr<IInetAddress> srcIp = mBroadcast ? Ptr(inet4AddressHelper)->Func(inet4AddressHelper->GetANY) : mSrcIp;
    FillInPacket(encap, destIp, srcIp, destUdp, srcUdp, rev,
        DHCP_BOOTREPLY, mBroadcast);
    IBuffer::Probe(rev)->Flip();
    FUNC_RETURN(rev)
}

ECode DhcpOfferPacket::FinishPacket(
    /* [in] */ IByteBuffer* buffer)
{
    AddTlv(buffer, DHCP_MESSAGE_TYPE, DHCP_MESSAGE_TYPE_OFFER);
    AddTlv(buffer, DHCP_SERVER_IDENTIFIER, mServerIdentifier);
    AddTlv(buffer, DHCP_LEASE_TIME, mLeaseTime);
    // the client should renew at 1/2 the lease-expiry interval
    if (mLeaseTime != NULL) {
        AddTlv(buffer, DHCP_RENEWAL_TIME,
            StringUtils::ToString(Ptr(mLeaseTime)->Func(mLeaseTime->GetValue) / 2));
    }
    AddTlv(buffer, DHCP_SUBNET_MASK, mSubnetMask);
    AddTlv(buffer, DHCP_ROUTER, mGateway);
    AddTlv(buffer, DHCP_DOMAIN_NAME, mDomainName);
    AddTlv(buffer, DHCP_BROADCAST_ADDRESS, mBroadcastAddress);
    AddTlv(buffer, DHCP_DNS_SERVER, mDnsServers);
    AddTlvEnd(buffer);
    return NOERROR;
}

ECode DhcpOfferPacket::DoNextOp(
    /* [in] */ DhcpStateMachine* machine)
{
    return machine->OnOfferReceived(mBroadcast, mTransId, mClientMac, mYourIp,
            mServerIdentifier);
}

} // namespace Dhcp
} // namespace Net
} // namespace Droid
} // namespace Elastos
