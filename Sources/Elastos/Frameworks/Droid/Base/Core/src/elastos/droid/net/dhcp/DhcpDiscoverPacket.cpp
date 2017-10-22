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
#include "elastos/droid/net/dhcp/DhcpDiscoverPacket.h"
#include "elastos/droid/net/dhcp/DhcpDiscoverPacket.h"
#include "elastos/droid/net/dhcp/DhcpPacket.h"
#include "elastos/droid/os/Build.h"

using Elastos::Droid::Os::Build;
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

DhcpDiscoverPacket::DhcpDiscoverPacket()
{}

ECode DhcpDiscoverPacket::constructor(
    /* [in] */ Int32 transId,
    /* [in] */ ArrayOf<Byte>* clientMac,
    /* [in] */ Boolean broadcast)
{
    AutoPtr<IInet4AddressHelper> inet4AddressHelper;
    CInet4AddressHelper::AcquireSingleton((IInet4AddressHelper**)&inet4AddressHelper);
    AutoPtr<IInetAddress> any;
    inet4AddressHelper->GetANY((IInetAddress**)&any);
    return DhcpPacket::constructor(transId, any, any, any, any, clientMac, broadcast);
}

ECode DhcpDiscoverPacket::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String s;
    DhcpPacket::ToString(&s);
    *result = s + " DISCOVER " +
            (mBroadcast ? "broadcast " : "unicast ");
    return NOERROR;
}

ECode DhcpDiscoverPacket::BuildPacket(
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
    AutoPtr<IInetAddress> any;
    inet4AddressHelper->GetANY((IInetAddress**)&any);
    AutoPtr<IInetAddress> destIp;
    inet4AddressHelper->GetALL((IInetAddress**)&destIp);
    FillInPacket(encap, destIp, any, destUdp, srcUdp,
            rev, DHCP_BOOTREQUEST, TRUE);
    IBuffer::Probe(rev)->Flip();
    *result = rev;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode DhcpDiscoverPacket::FinishPacket(
    /* [in] */ IByteBuffer* buffer)
{
    AddTlv(buffer, DHCP_MESSAGE_TYPE, DHCP_MESSAGE_TYPE_DISCOVER);
    AddTlv(buffer, DHCP_PARAMETER_LIST, mRequestedParams);
    AddTlvEnd(buffer);
    return NOERROR;
}

ECode DhcpDiscoverPacket::DoNextOp(
    /* [in] */ DhcpStateMachine* machine)
{
    // currently omitted: host name
   return machine->OnDiscoverReceived(mBroadcast, mTransId, mClientMac,
        mRequestedParams);
}

} // namespace Dhcp
} // namespace Net
} // namespace Droid
} // namespace Elastos
