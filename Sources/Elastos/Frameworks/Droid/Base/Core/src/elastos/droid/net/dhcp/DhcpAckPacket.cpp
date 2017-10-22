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
#include "elastos/droid/net/dhcp/DhcpAckPacket.h"
#include "elastos/droid/net/DhcpStateMachine.h"
#include "elastos/droid/net/dhcp/DhcpAckPacket.h"
#include "elastos/droid/net/dhcp/DhcpPacket.h"
#include "elastos/droid/os/Build.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/core/StringUtils.h"

using Elastos::Droid::Os::Build;

using Elastos::Core::IInteger32;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::Net::CInet4AddressHelper;
using Elastos::Net::IInet4Address;
using Elastos::Net::IInet4AddressHelper;
using Elastos::Net::IInetAddress;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Dhcp {

DhcpAckPacket::DhcpAckPacket()
{}

ECode DhcpAckPacket::constructor(
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
    DhcpPacket::constructor(transId, any, clientIp, serverAddress, any, clientMac, broadcast);
    mBroadcast = broadcast;
    mSrcIp = serverAddress;
    return NOERROR;
}

ECode DhcpAckPacket::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder sb;
    String s;
    DhcpPacket::ToString(&s);
    String sYourIp;
    IObject::Probe(mYourIp)->ToString(&sYourIp);
    String sSubnetMask;
    IObject::Probe(mSubnetMask)->ToString(&sSubnetMask);
    String sGateway;
    IObject::Probe(mGateway)->ToString(&sGateway);
    String sLeaseTime;
    IObject::Probe(mLeaseTime)->ToString(&sLeaseTime);
    sb += s;
    sb += " ACK: your new IP ";
    sb += sYourIp;
    sb += ", netmask ";
    sb += sSubnetMask;
    sb += ", gateway ";
    sb += sGateway;
    sb += " DNS servers: ";
    AutoPtr<IIterator> it;
    mDnsServers->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        String sDnsServer;
        IObject::Probe(obj)->ToString(&sDnsServer);
        sb += sDnsServer;
        sb += " ";
    }
    sb += ", lease time ";
    sb += sLeaseTime;
    return sb.ToString(result);
}

ECode DhcpAckPacket::BuildPacket(
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

    AutoPtr<IInetAddress> destIp;
    AutoPtr<IInetAddress> srcIp;
    if (mBroadcast) {
        AutoPtr<IInet4AddressHelper> helper;
        CInet4AddressHelper::AcquireSingleton((IInet4AddressHelper**)&helper);
        helper->GetALL((IInetAddress**)&destIp);
        helper->GetANY((IInetAddress**)&srcIp);
    }
    else {
        destIp = mYourIp;
        srcIp = mSrcIp;
    }
    FillInPacket(encap, destIp, srcIp, destUdp, srcUdp, rev,
        DHCP_BOOTREPLY, mBroadcast);
    IBuffer::Probe(rev)->Flip();
    *result = rev;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode DhcpAckPacket::FinishPacket(
    /* [in] */ IByteBuffer* buffer)
{
    AddTlv(buffer, DHCP_MESSAGE_TYPE, DHCP_MESSAGE_TYPE_ACK);
    AddTlv(buffer, DHCP_SERVER_IDENTIFIER, mServerIdentifier);
    AddTlv(buffer, DHCP_LEASE_TIME, mLeaseTime);
    // the client should renew at 1/2 the lease-expiry interval
    if (mLeaseTime != NULL) {
        Int32 time;
        mLeaseTime->GetValue(&time);
        AddTlv(buffer, DHCP_RENEWAL_TIME, StringUtils::ToString(time / 2));
    }
    AddTlv(buffer, DHCP_SUBNET_MASK, mSubnetMask);
    AddTlv(buffer, DHCP_ROUTER, mGateway);
    AddTlv(buffer, DHCP_DOMAIN_NAME, mDomainName);
    AddTlv(buffer, DHCP_BROADCAST_ADDRESS, mBroadcastAddress);
    AddTlv(buffer, DHCP_DNS_SERVER, mDnsServers);
    AddTlvEnd(buffer);
    return NOERROR;
}

ECode DhcpAckPacket::GetInt(
    /* [in] */ IInteger32* v,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (v == NULL) {
        *result = 0;
        return NOERROR;
    } else {
        return v->GetValue(result);
    }
    return NOERROR;
}

ECode DhcpAckPacket::DoNextOp(
    /* [in] */ DhcpStateMachine* machine)
{
    Int32 time;
    mLeaseTime->GetValue(&time);
    return machine->OnAckReceived(mYourIp, mSubnetMask, mGateway, mDnsServers,
            mServerIdentifier, time);
}

} // namespace Dhcp
} // namespace Net
} // namespace Droid
} // namespace Elastos
