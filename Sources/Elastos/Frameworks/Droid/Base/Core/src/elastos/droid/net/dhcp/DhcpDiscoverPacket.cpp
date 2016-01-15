
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/net/dhcp/DhcpDiscoverPacket.h"
#include "elastos/droid/net/ReturnOutValue.h"
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
    FillInPacket(encap, Ptr(inet4AddressHelper)->Func(inet4AddressHelper->GetALL),
            Ptr(inet4AddressHelper)->Func(inet4AddressHelper->GetANY),
            destUdp, srcUdp,
            rev, DHCP_BOOTREQUEST, TRUE);
    IBuffer::Probe(rev)->Flip();
    FUNC_RETURN(rev)
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
