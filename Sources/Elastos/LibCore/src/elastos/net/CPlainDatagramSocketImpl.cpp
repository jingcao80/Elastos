#include "CPlainDatagramSocketImpl.h"
#include "CFileDescriptor.h"
#include "CBoolean.h"
#include "CDatagramPacket.h"
#include "CInteger32.h"
#include "CInetAddressHelper.h"
#include "CCloseGuardHelper.h"
#include "AutoLock.h"
#include "OsConstants.h"
#include "CStructGroupReq.h"
#include "CLibcore.h"
#include "IoBridge.h"

using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::ICloseGuardHelper;
using Elastos::Core::CCloseGuardHelper;
using Elastos::IO::CFileDescriptor;
using Libcore::IO::IoBridge;
using Libcore::IO::IIoBridge;
using Libcore::IO::CLibcore;
using Elastos::Droid::System::OsConstants;
using Elastos::Droid::System::CStructGroupReq;

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CPlainDatagramSocketImpl, DatagramSocketImpl, IPlainDatagramSocketImpl)

CAR_OBJECT_IMPL(CPlainDatagramSocketImpl)

CPlainDatagramSocketImpl::CPlainDatagramSocketImpl()
    : mIsNativeConnected(FALSE)
    , mConnectedPort(-1)
{
    AutoPtr<ICloseGuardHelper> closeGuardHelper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&closeGuardHelper);
    closeGuardHelper->Get((ICloseGuard**)&mGuard);
}

CPlainDatagramSocketImpl::~CPlainDatagramSocketImpl()
{
    // try {
    if (mGuard != NULL) {
        mGuard->WarnIfOpen();
    }
    Close();
    // } finally {
    //     super.finalize()
    // }
}

ECode CPlainDatagramSocketImpl::constructor(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 localPort)
{
    VALIDATE_NOT_NULL(fd);
    mFd = fd;
    mLocalPort = localPort;
    Boolean flag = FALSE;
    if ((fd->Valid(&flag), flag)) {
        return mGuard->Open(String("CPlainDatagramSocketImpl::Close"));
    }
    return NOERROR;
}

ECode CPlainDatagramSocketImpl::constructor()
{
    return CFileDescriptor::New((IFileDescriptor**)&mFd);
}

ECode CPlainDatagramSocketImpl::Bind(
    /* [in] */ Int32 port,
    /* [in] */ IInetAddress* address)
{
    FAIL_RETURN(IoBridge::Bind(mFd, address, port));
    if (port != 0) {
        mLocalPort = port;
    }
    else {
        FAIL_RETURN(IoBridge::GetSocketLocalPort(mFd, &mLocalPort));
    }
    AutoPtr<IBoolean> ib;
    CBoolean::New(TRUE, (IBoolean**)&ib);
    return SetOption(ISocketOptions::_SO_BROADCAST, ib);
}

ECode CPlainDatagramSocketImpl::OnBind(
    /* [in] */ IInetAddress* inetAddr,
    /* [in] */ Int32 port)
{
    mLocalPort = port;
    return NOERROR;
}

ECode CPlainDatagramSocketImpl::Close()
{
    synchronized(this) {
        mGuard->Close();
        return IoBridge::CloseAndSignalBlockedThreads(mFd);
    }
    return NOERROR;
}

ECode CPlainDatagramSocketImpl::OnClose()
{
    mGuard->Close();
    return NOERROR;
}

ECode CPlainDatagramSocketImpl::Create()
{
    AutoPtr<IFileDescriptor> fd;
    FAIL_RETURN(IoBridge::Socket(FALSE, (IFileDescriptor**)&fd));
    mFd = fd;
    return NOERROR;
}

ECode CPlainDatagramSocketImpl::GetOption(
    /* [in] */ Int32 option,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IInterface> socketOption;
    FAIL_RETURN(IoBridge::GetSocketOption(mFd, option, (IInterface**)&socketOption));
    *result = socketOption;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CPlainDatagramSocketImpl::GetTimeToLive(
    /* [out] */ Int32* timeToLive)
{
    VALIDATE_NOT_NULL(timeToLive);
    AutoPtr<IInterface> result;
    FAIL_RETURN(GetOption(IIoBridge::ELASTOS_IP_MULTICAST_TTL, (IInterface**)&result));
    AutoPtr<IInteger32> i32Obj = (IInteger32*)IInteger32::Probe(result);
    return i32Obj->GetValue(timeToLive);
}

ECode CPlainDatagramSocketImpl::GetTTL(
    /* [out] */ Byte* byte)
{
    VALIDATE_NOT_NULL(byte);
    Int32 ttl;
    FAIL_RETURN(GetTimeToLive(&ttl));
    *byte = (Byte)ttl;
    return NOERROR;
}

AutoPtr<IStructGroupReq> CPlainDatagramSocketImpl::MakeGroupReq(
    /* [in] */ IInetAddress* gr_group,
    /* [in] */ INetworkInterface* networkInterface)
{
    Int32 gr_interface;
    if (networkInterface != NULL) {
        networkInterface->GetIndex(&gr_interface);
    } else {
        gr_interface = 0;
    }

    AutoPtr<IStructGroupReq> sGroupReq;
    CStructGroupReq::New(gr_interface, gr_group, (IStructGroupReq**)&sGroupReq);
    return sGroupReq;
}

ECode CPlainDatagramSocketImpl::Join(
    /* [in] */ IInetAddress* addr)
{
    AutoPtr<IStructGroupReq> structGroupReq = MakeGroupReq(addr, NULL);
    return SetOption(IIoBridge::ELASTOS_MCAST_JOIN_GROUP, structGroupReq);
}

ECode CPlainDatagramSocketImpl::JoinGroup(
    /* [in] */ ISocketAddress* addr,
    /* [in] */ INetworkInterface* netInterface)
{
    if (IInetSocketAddress::Probe(addr)) {
        AutoPtr<IInetAddress> groupAddr;
        IInetSocketAddress::Probe(addr)->GetAddress((IInetAddress**)&groupAddr);
        AutoPtr<IStructGroupReq> structGroupReq = MakeGroupReq(groupAddr, netInterface);
        return SetOption(IIoBridge::ELASTOS_MCAST_JOIN_GROUP, structGroupReq);
    }
    return NOERROR;
}

ECode CPlainDatagramSocketImpl::Leave(
    /* [in] */ IInetAddress* addr)
{
    AutoPtr<IStructGroupReq> structGroupReq = MakeGroupReq(addr, NULL);
    return SetOption(IIoBridge::ELASTOS_MCAST_LEAVE_GROUP, structGroupReq);
}

ECode CPlainDatagramSocketImpl::LeaveGroup(
    /* [in] */ ISocketAddress* addr,
    /* [in] */ INetworkInterface* netInterface)
{
    if (IInetSocketAddress::Probe(addr)) {
        AutoPtr<IInetAddress> groupAddr;
        IInetSocketAddress::Probe(addr)->GetAddress((IInetAddress**)&groupAddr);
        AutoPtr<IStructGroupReq> structGroupReq = MakeGroupReq(groupAddr, netInterface);
        return SetOption(IIoBridge::ELASTOS_MCAST_LEAVE_GROUP, structGroupReq);
    }
    return NOERROR;
}

ECode CPlainDatagramSocketImpl::Peek(
    /* [in] */ IInetAddress* sender,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(0); // EmptyArray.BYTE;
    // We don't actually want the data: we just want the DatagramPacket's filled-in address.
    AutoPtr<IDatagramPacket> packet;
    CDatagramPacket::New(data, 0, (IDatagramPacket**)&packet);
    PeekData(packet, result);
    // Note: evil side-effect on InetAddress! This method should have returned InetSocketAddress!
    AutoPtr<IInetAddress> netAddress;
    packet->GetAddress((IInetAddress**)&netAddress);
    AutoPtr< ArrayOf<Byte> > outarr1;
    netAddress->GetAddress((ArrayOf<Byte>**)&outarr1);
    AutoPtr< ArrayOf<Byte> > outarr2;
    netAddress->GetAddress((ArrayOf<Byte>**)&outarr2);
    outarr2 = outarr1;
    return NOERROR;
}

ECode CPlainDatagramSocketImpl::DoRecv(
    /* [in] */ IDatagramPacket* pack,
    /* [in] */ Int32 flags)
{
    AutoPtr<ArrayOf<Byte> > bytes;
    pack->GetData((ArrayOf<Byte>**)&bytes);
    Int32 byteOffset;
    pack->GetOffset(&byteOffset);
    Int32 length;
    pack->GetLength(&length);
    Int32 result;
    FAIL_RETURN(IoBridge::Recvfrom(FALSE, mFd, bytes, byteOffset, length, flags, pack, mIsNativeConnected, &result));
    if (mIsNativeConnected) {
        UpdatePacketRecvAddress(pack);
    }
    return NOERROR;
}

ECode CPlainDatagramSocketImpl::Receive(
    /* [in] */ IDatagramPacket* pack)
{
    return DoRecv(pack, 0);
}

ECode CPlainDatagramSocketImpl::PeekData(
    /* [in] */ IDatagramPacket* pack,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(DoRecv(pack, OsConstants::_MSG_PEEK));
    return pack->GetPort(result);
}

ECode CPlainDatagramSocketImpl::Send(
    /* [in] */ IDatagramPacket* packet)
{
    VALIDATE_NOT_NULL(packet);
    Int32 packetPort;
    Int32 port = mIsNativeConnected ? 0 : (packet->GetPort(&packetPort),packetPort);
    AutoPtr<IInetAddress> packetAddress;
    packet->GetAddress((IInetAddress**)&packetAddress);
    AutoPtr<IInetAddress> address = mIsNativeConnected ? NULL : packetAddress;
    AutoPtr<ArrayOf<Byte> > bytes;
    packet->GetData((ArrayOf<Byte>**)&bytes);
    Int32 byteOffset;
    packet->GetOffset(&byteOffset);
    Int32 length;
    packet->GetLength(&length);
    Int32 result;
    return IoBridge::Sendto(mFd, bytes, byteOffset, length, 0, address, port, &result);
}

ECode CPlainDatagramSocketImpl::SetOption(
    /* [in] */ Int32 option,
    /* [in] */ IInterface* value)
{
    return IoBridge::SetSocketOption(mFd, option, value);
}

ECode CPlainDatagramSocketImpl::SetTimeToLive(
    /* [in] */ Int32 ttl)
{
    AutoPtr<IInteger32> value;
    CInteger32::New(ttl, (IInteger32**)&value);
    return SetOption(IIoBridge::ELASTOS_IP_MULTICAST_TTL, value);
}

ECode CPlainDatagramSocketImpl::SetTTL(
    /* [in] */ Byte ttl)
{
    return SetTimeToLive((Int32)ttl & 0xff); // Avoid sign extension.
}

ECode CPlainDatagramSocketImpl::Connect(
    /* [in] */ IInetAddress* inetAddr,
    /* [in] */ Int32 port)
{
    FAIL_RETURN(IoBridge::Connect(mFd, inetAddr, port));
    AutoPtr<ArrayOf<Byte> > address;
    inetAddr->GetAddress((ArrayOf<Byte>**)&address);
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    ECode ec = inetAddressHelper->GetByAddress(address, (IInetAddress**)&mConnectedAddress);
    if (FAILED(ec)) {
        // this is never expected to happen as we should not have gotten
        // here if the address is not resolvable
        // throw new SocketException("Host is unresolved: " + inetAddr.getHostName());
        return E_SOCKET_EXCEPTION;
    }
    mConnectedPort = port;
    mIsNativeConnected = TRUE;
    return NOERROR;
}

ECode CPlainDatagramSocketImpl::OnConnect(
    /* [in] */ IInetAddress* remoteAddress,
    /* [in] */ Int32 remotePort)
{
    mIsNativeConnected = TRUE;
    mConnectedAddress = remoteAddress;
    mConnectedPort = remotePort;
    return NOERROR;
}

ECode CPlainDatagramSocketImpl::Disconnect()
{
    // try {
    FAIL_RETURN(CLibcore::sOs->Connect(mFd, InetAddress::UNSPECIFIED, 0));
    // } catch (ErrnoException errnoException) {
        // throw new AssertionError(errnoException);
    // } catch (SocketException ignored) {
            // Thrown if the socket has already been closed, but this method can't throw anything.
    // }
    mConnectedPort = -1;
    mConnectedAddress = NULL;
    mIsNativeConnected = FALSE;
    return NOERROR;
}

ECode CPlainDatagramSocketImpl::OnDisconnect()
{
    mIsNativeConnected = FALSE;
    mConnectedAddress = NULL;
    mConnectedPort = -1;
    return NOERROR;
}

void CPlainDatagramSocketImpl::UpdatePacketRecvAddress(
    /* [in] */ IDatagramPacket* packet)
{
    if (packet) {
        packet->SetAddress(mConnectedAddress);
        packet->SetPort(mConnectedPort);
    }
}


} // namespace Net
} // namespace Elastos

