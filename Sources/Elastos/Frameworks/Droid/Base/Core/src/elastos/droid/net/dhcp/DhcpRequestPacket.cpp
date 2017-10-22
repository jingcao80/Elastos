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
#include "elastos/droid/net/dhcp/DhcpRequestPacket.h"
#include "elastos/droid/net/dhcp/DhcpPacket.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::Build;

using Elastos::Core::StringUtils;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::Net::CInet4AddressHelper;
using Elastos::Net::IInet4Address;
using Elastos::Net::IInet4AddressHelper;
using Elastos::Net::IInetAddress;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Dhcp {

ECode DhcpRequestPacket::constructor(
    /* [in] */ Int32 transId,
    /* [in] */ IInetAddress* clientIp,
    /* [in] */ ArrayOf<Byte>* clientMac,
    /* [in] */ Boolean broadcast)
{
    AutoPtr<IInetAddress> any;
    AutoPtr<IInet4AddressHelper> inet4AddressHelper;
    CInet4AddressHelper::AcquireSingleton((IInet4AddressHelper**)&inet4AddressHelper);
    inet4AddressHelper->GetANY((IInetAddress**)&any);
    return DhcpPacket::constructor(transId, clientIp, any, any, any, clientMac, broadcast);
}

ECode DhcpRequestPacket::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String s;
    DhcpPacket::ToString(&s);
    *result = s + " REQUEST, desired IP " + Object::ToString(mRequestedIp) + " from host '"
            + mHostName + "', param list length "
            + StringUtils::ToString(mRequestedParams == NULL ? 0 : mRequestedParams->GetLength());
    return NOERROR;
}

ECode DhcpRequestPacket::BuildPacket(
    /* [in] */ Int32 encap,
    /* [in] */ Int16 destUdp,
    /* [in] */ Int16 srcUdp,
    /* [out] */ IByteBuffer** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IByteBufferHelper> byteBufferHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&byteBufferHelper);
    byteBufferHelper->Allocate(MAX_LENGTH, result);
    AutoPtr<IInet4AddressHelper> inet4AddressHelper;
    CInet4AddressHelper::AcquireSingleton((IInet4AddressHelper**)&inet4AddressHelper);
    AutoPtr<IInetAddress> all;
    inet4AddressHelper->GetALL((IInetAddress**)&all);
    AutoPtr<IInetAddress> any;
    inet4AddressHelper->GetANY((IInetAddress**)&any);
    FillInPacket(encap,
        all, any, destUdp, srcUdp, *result, DHCP_BOOTREQUEST, mBroadcast);
    IBuffer::Probe(*result)->Flip();
    return NOERROR;
}

ECode DhcpRequestPacket::FinishPacket(
    /* [in] */ IByteBuffer* buffer)
{
    AutoPtr<ArrayOf<Byte> > clientId = ArrayOf<Byte>::Alloc(7);
    // assemble client identifier
    (*clientId)[0] = CLIENT_ID_ETHER;
    clientId->Copy(1, mClientMac, 0, 6);
    AddTlv(buffer, DHCP_MESSAGE_TYPE, DHCP_MESSAGE_TYPE_REQUEST);
    AddTlv(buffer, DHCP_PARAMETER_LIST, mRequestedParams);
    AddTlv(buffer, DHCP_REQUESTED_IP, mRequestedIp);
    AddTlv(buffer, DHCP_SERVER_IDENTIFIER, mServerIdentifier);
    AddTlv(buffer, DHCP_CLIENT_IDENTIFIER, clientId);
    AddTlvEnd(buffer);
    return NOERROR;
}

ECode DhcpRequestPacket::DoNextOp(
    /* [in] */ DhcpStateMachine* machine)
{
    AutoPtr<IInetAddress> clientRequest =
            mRequestedIp == NULL ? mClientIp : mRequestedIp;
    Logger::V(TAG, "requested IP is %s and client IP is ", TO_CSTR(mRequestedIp),
            TO_CSTR(mClientIp));
    return machine->OnRequestReceived(mBroadcast, mTransId, mClientMac,
            clientRequest, mRequestedParams, mHostName);
}

} // namespace Dhcp
} // namespace Net
} // namespace Droid
} // namespace Elastos
