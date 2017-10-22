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
#include "elastos/droid/net/dhcp/DhcpNakPacket.h"
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

DhcpNakPacket::DhcpNakPacket()
{}

ECode DhcpNakPacket::constructor(
    /* [in] */ Int32 transId,
    /* [in] */ IInetAddress* clientIp,
    /* [in] */ IInetAddress* yourIp,
    /* [in] */ IInetAddress* nextIp,
    /* [in] */ IInetAddress* relayIp,
    /* [in] */ ArrayOf<Byte>* clientMac)
{
    AutoPtr<IInetAddress> any;
    AutoPtr<IInet4AddressHelper> inet4AddressHelper;
    CInet4AddressHelper::AcquireSingleton((IInet4AddressHelper**)&inet4AddressHelper);
    inet4AddressHelper->GetANY((IInetAddress**)&any);
    return DhcpPacket::constructor(transId, any, any, nextIp, relayIp,
            clientMac, FALSE);
}

ECode DhcpNakPacket::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String s;
    DhcpPacket::ToString(&s);
    *result = s + " NAK, reason " + (mMessage == NULL ? "(none)" : mMessage);
    return NOERROR;
}

ECode DhcpNakPacket::BuildPacket(
    /* [in] */ Int32 encap,
    /* [in] */ Int16 destUdp,
    /* [in] */ Int16 srcUdp,
    /* [out] */ IByteBuffer** result)
{
    AutoPtr<IByteBuffer> rev;
    AutoPtr<IByteBufferHelper> byteBufferHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&byteBufferHelper);
    byteBufferHelper->Allocate(MAX_LENGTH, (IByteBuffer**)&rev);
    AutoPtr<IInetAddress> destIp = mClientIp;
    AutoPtr<IInetAddress> srcIp = mYourIp;
    FillInPacket(encap, destIp, srcIp, destUdp, srcUdp, rev,
        DHCP_BOOTREPLY, mBroadcast);
    IBuffer::Probe(rev)->Flip();
    *result = rev;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode DhcpNakPacket::FinishPacket(
    /* [in] */ IByteBuffer* buffer)
{
    AddTlv(buffer, DHCP_MESSAGE_TYPE, DHCP_MESSAGE_TYPE_NAK);
    AddTlv(buffer, DHCP_SERVER_IDENTIFIER, mServerIdentifier);
    AddTlv(buffer, DHCP_MESSAGE, mMessage);
    AddTlvEnd(buffer);
    return NOERROR;
}

ECode DhcpNakPacket::DoNextOp(
    /* [in] */ DhcpStateMachine* machine)
{
    return machine->OnNakReceived();
}

} // namespace Dhcp
} // namespace Net
} // namespace Droid
} // namespace Elastos
