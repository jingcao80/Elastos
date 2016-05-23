#include "SocketChannelImpl.h"
#include "IoBridge.h"
#include "SocketUtils.h"
#include "AutoLock.h"
#include "CPlainSocketImpl.h"
#include "InetAddress.h"
#include "Buffer.h"
#include "Arrays.h"
#include "FileChannelImpl.h"
#include "CLibcore.h"
#include "IoUtils.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Libcore::IO::IoBridge;
using Libcore::IO::CLibcore;
using Libcore::IO::IoUtils;
using Elastos::Net::SocketUtils;
using Elastos::Net::ISocketImpl;
using Elastos::Net::EIID_ISocket;
using Elastos::Net::CPlainSocketImpl;
using Elastos::Net::InetAddress;
using Elastos::Utility::Arrays;

namespace Elastos{
namespace IO{

//==========================================================
//       SocketChannelImpl::SocketAdapter
//==========================================================
SocketChannelImpl::SocketAdapter::SocketAdapter(
    /* [in] */ IPlainSocketImpl* socketImpl,
    /* [in] */ SocketChannelImpl* channel)
{
    assert(0 && "TODO");
    // Super(socketImpl);
    mSocketImpl = socketImpl;
    mChannel = channel;
    SocketUtils::SetCreated(this);

    // Sync state socket state with the channel it is being created from
    if (mChannel->mIsBound) {
        OnBind(mChannel->mLocalAddress, mChannel->mLocalPort);
    }
    Boolean isflag = FALSE;
    if (mChannel->IsConnected(&isflag), isflag) {
        AutoPtr<IInetAddress> addres;
        mChannel->mConnectAddress->GetAddress((IInetAddress**)&addres);
        Int32 portnum = 0;
        mChannel->mConnectAddress->GetPort(&portnum);
        OnConnect(ISocketAddress::Probe(addres), portnum);
    }
    if (mChannel->IsOpen(&isflag), isflag) {
        OnClose();
    }
}

AutoPtr<SocketChannel> SocketChannelImpl::SocketAdapter::GetChannel()
{
    return mChannel;
}

ECode SocketChannelImpl::SocketAdapter::Connect(
    /* [in] */ ISocketAddress* remoteAddr,
    /* [in] */ Int32 timeout)
{
    Boolean isflag = FALSE;
    if (mChannel->IsBlocking(&isflag), isflag) {
        // throw new IllegalBlockingModeException();
        return E_ILLEGAL_BLOCKING_MODE_EXCEPTION;
    }
    if (IsConnected(&isflag), isflag) {
        // throw new AlreadyConnectedException();
        return E_ALREADY_CONNECTED_EXCEPTION;
    }
    Socket::Connect(remoteAddr, timeout);
    mChannel->OnBind(FALSE);
    if (Socket::IsConnected(&isflag), isflag) {
        AutoPtr<IInetSocketAddress> remoteInetAddress = IInetSocketAddress::Probe(remoteAddr);
        mChannel->OnConnectStatusChanged(
                remoteInetAddress, SOCKET_STATUS_CONNECTED, FALSE /* updateSocketState */);
    }
    return NOERROR;
}

ECode SocketChannelImpl::SocketAdapter::Bind(
    /* [in] */ ISocketAddress* localAddr)
{
    Boolean isflag = FALSE;
    if (mChannel->IsConnected(&isflag), isflag) {
        // throw new AlreadyConnectedException();
        return E_ALREADY_CONNECTED_EXCEPTION;
    }
    if (SocketChannelImpl::SOCKET_STATUS_PENDING == mChannel->mStatus) {
        // throw new ConnectionPendingException();
        return E_CONNECTION_PENDING_EXCEPTION;
    }
    Socket::Bind(ISocketAddress::Probe(mChannel->mLocalAddress));
    mChannel->OnBind(FALSE);
    return NOERROR;
}

ECode SocketChannelImpl::SocketAdapter::Close()
{
    {    AutoLock syncLock(mChannel);
        Socket::Close();
        Boolean isflag = FALSE;
        if (mChannel->IsOpen(&isflag), isflag) {
            // mChannel->close() recognizes the socket is closed and avoids recursion. There
            // is no mChannel->onClose() because the "closed" field is private.
            mChannel->Close();
        }
    }
    return NOERROR;
}

ECode SocketChannelImpl::SocketAdapter::GetInputStream(
    /* [out] */ IInputStream** inputStream)
{
    VALIDATE_NOT_NULL(inputStream)

    AutoPtr<IInputStream> outinput;
    Socket::GetInputStream((IInputStream**)&outinput);
    *inputStream = (IInputStream*) new BlockingCheckInputStream(outinput, mChannel);
    REFCOUNT_ADD(*inputStream)
    return NOERROR;
}

ECode SocketChannelImpl::SocketAdapter::GetOutputStream(
    /* [out] */ IOutputStream** outstream)
{
    // return new BlockingCheckOutputStream(super.getOutputStream(), mChannel);
    VALIDATE_NOT_NULL(outstream)

    AutoPtr<IOutputStream> outinput;
    Socket::GetOutputStream((IOutputStream**)&outinput);
    *outstream = (IOutputStream*) new BlockingCheckOutputStream(outinput, mChannel);
    REFCOUNT_ADD(*outstream)
    return NOERROR;
}

ECode SocketChannelImpl::SocketAdapter::GetFileDescriptor(
    /* [out] */ IFileDescriptor** outfd)
{
    return ISocketImpl::Probe(mSocketImpl)->GetFileDescriptor(outfd);
}

//==========================================================
//       SocketChannelImpl::BlockingCheckOutputStream
//==========================================================
SocketChannelImpl::BlockingCheckOutputStream::BlockingCheckOutputStream(
    /* [in] */ IOutputStream* out,
    /* [in] */ SocketChannel* channel)
{
    FilterOutputStream::constructor(out);
    mChannel = channel;
}

ECode SocketChannelImpl::BlockingCheckOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    FAIL_RETURN(CheckBlocking());
    return mOut->Write(buffer, offset, count);
}

ECode SocketChannelImpl::BlockingCheckOutputStream::Write(
    /* [in] */ Int32 oneByte)
{
    FAIL_RETURN(CheckBlocking());
    return mOut->Write(oneByte);
}

ECode SocketChannelImpl::BlockingCheckOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buffer)
{
    FAIL_RETURN(CheckBlocking());
    return mOut->Write(buffer);
}

ECode SocketChannelImpl::BlockingCheckOutputStream::Close()
{
    FilterOutputStream::Close();
    // mChannel->close() recognizes the socket is closed and avoids recursion. There is no
    // mChannel->onClose() because the "closed" field is private.
    return mChannel->Close();
}

ECode SocketChannelImpl::BlockingCheckOutputStream::CheckBlocking()
{
    Boolean isflag = FALSE;
    if (mChannel->IsBlocking(&isflag), !isflag) {
        // throw new IllegalBlockingModeException();
        return E_ILLEGAL_BLOCKING_MODE_EXCEPTION;
    }
    return NOERROR;
}

//==========================================================
//       SocketChannelImpl::BlockingCheckInputStream
//==========================================================

SocketChannelImpl::BlockingCheckInputStream::BlockingCheckInputStream(
    /* [in] */ IInputStream* in,
    /* [in] */ SocketChannel* channel)
{
    FilterInputStream::constructor(in);
    mChannel = channel;
}

ECode SocketChannelImpl::BlockingCheckInputStream::Read(
    /* [out] */ Int32* value)
{
    FAIL_RETURN(CheckBlocking());
    return mIn->Read(value);
}

ECode SocketChannelImpl::BlockingCheckInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* number)
{
    FAIL_RETURN(CheckBlocking());
    return mIn->Read(buffer, byteOffset, byteCount, number);
}

ECode SocketChannelImpl::BlockingCheckInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [out] */ Int32* number)
{
    FAIL_RETURN(CheckBlocking());
    return mIn->Read(buffer, number);
}

ECode SocketChannelImpl::BlockingCheckInputStream::Close()
{
    FilterInputStream::Close();
    // mChannel->close() recognizes the socket is closed and avoids recursion. There is no
    // mChannel->onClose() because the "closed" field is private.
    return mChannel->Close();
}

ECode SocketChannelImpl::BlockingCheckInputStream::CheckBlocking()
{
    Boolean isflag = FALSE;
    if (mChannel->IsBlocking(&isflag), !isflag) {
        // throw new IllegalBlockingModeException();
        return E_ILLEGAL_BLOCKING_MODE_EXCEPTION;
    }
    return NOERROR;
}

const Int32 SocketChannelImpl::SOCKET_STATUS_UNINITITLIZED = -1;

const Int32 SocketChannelImpl::SOCKET_STATUS_UNCONNECTED = 0;

const Int32 SocketChannelImpl::SOCKET_STATUS_PENDING = 1;

const Int32 SocketChannelImpl::SOCKET_STATUS_CONNECTED = 2;

const Int32 SocketChannelImpl::SOCKET_STATUS_CLOSED = 3;

SocketChannelImpl::SocketChannelImpl(
    /* [in] */ ISelectorProvider* provider)
{
    SocketChannel::constructor(provider);
    mStatus = SOCKET_STATUS_UNCONNECTED;
    Boolean connect = TRUE;
    if (connect) {
        IoBridge::Socket(TRUE, (IFileDescriptor**)&mFd);
    }
    else {
        CFileDescriptor::New((IFileDescriptor**)&mFd);
    }
}

SocketChannelImpl::SocketChannelImpl(
    /* [in] */ ISelectorProvider* provider,
    /* [in] */ Boolean connect)
{
    SocketChannel::constructor(provider);
    mStatus = SOCKET_STATUS_UNCONNECTED;
    if (connect) {
        IoBridge::Socket(TRUE, (IFileDescriptor**)&mFd);
    }
    else {
        CFileDescriptor::New((IFileDescriptor**)&mFd);
    }
}

SocketChannelImpl::SocketChannelImpl(
    /* [in] */ ISelectorProvider* provider,
    /* [in] */ IFileDescriptor* existingFd)
{
    SocketChannel::constructor(provider);
    mStatus = SOCKET_STATUS_CONNECTED;
    mFd = existingFd;
}

CAR_INTERFACE_IMPL(SocketChannelImpl, Object, IFileDescriptorChannel)

ECode SocketChannelImpl::GetSocket(
    /* [out] */ ISocket** socket)
{
    VALIDATE_NOT_NULL(socket)
    *socket = NULL;

    if (mSocket == NULL) {
        // try {
            AutoPtr<IInetAddress> addr;
            Int32 port = 0;
            if (mConnectAddress != NULL) {
                mConnectAddress->GetAddress((IInetAddress**)&addr);
                mConnectAddress->GetPort(&port);
            }
            AutoPtr<IPlainSocketImpl> psi;
            CPlainSocketImpl::New(mFd, mLocalPort, addr, port, (IPlainSocketImpl**)&psi);
            mSocket = new SocketAdapter(psi, this);
        // } catch (SocketException e) {
            // return NULL;
        // }
    }
    *socket = mSocket;
    REFCOUNT_ADD(*socket)
    return NOERROR;
}

ECode SocketChannelImpl::IsConnected(
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = FALSE;

    *ret = mStatus == SOCKET_STATUS_CONNECTED;
    return NOERROR;
}

ECode SocketChannelImpl::IsConnectionPending(
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = FALSE;

    *ret = mStatus == SOCKET_STATUS_PENDING;
    return NOERROR;
}

ECode SocketChannelImpl::Connect(
    /* [in] */ ISocketAddress* socketAddress,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = FALSE;
    // mStatus must be open and unconnected
    FAIL_RETURN(CheckUnconnected());

    // check the address
    AutoPtr<IInetSocketAddress> inetSocketAddress;
    ValidateAddress(socketAddress, (IInetSocketAddress**)&inetSocketAddress);
    AutoPtr<IInetAddress> normalAddr;
    inetSocketAddress->GetAddress((IInetAddress**)&normalAddr);
    Int32 port = 0;
    inetSocketAddress->GetPort(&port);

    // When connecting, map ANY address to localhost
    Boolean isflag = FALSE;
    if (normalAddr->IsAnyLocalAddress(&isflag), isflag) {
        InetAddress::GetLocalHost((IInetAddress**)&normalAddr);
    }

    Boolean isBlocking = FALSE;
    IsBlocking(&isBlocking);
    Boolean finished = FALSE;
    Int32 newStatus = 0;
    // try {
        if (isBlocking) {
            Begin();
        }
        // When in blocking mode, IoBridge.connect() will return without an exception when the
        // mSocket is connected. When in non-blocking mode it will return without an exception
        // without knowing the result of the connection attempt, which could still be going on.
        ECode ec = IoBridge::Connect(mFd, normalAddr, port);
        if (ec == NOERROR) {
            newStatus = isBlocking ? SOCKET_STATUS_CONNECTED : SOCKET_STATUS_PENDING;
            finished = TRUE;
        }
        else {
            if (IsEINPROGRESS(ec)) {
                newStatus = SOCKET_STATUS_PENDING;
            }
            else {
                if (IsOpen()) {
                    Close();
                    finished = TRUE;
                }
                return ec;
            }
        }

    // } catch (IOException e) {

    // } finally {
        if (isBlocking) {
            End(finished);
        }
    // }

    // If the channel was not bound, a connection attempt will have caused an implicit bind() to
    // take place. Keep the local address state held by the channel and the socket up to date.
    if (!mIsBound) {
        OnBind(TRUE /* updateSocketState */);
    }

    // Keep the connected state held by the channel and the socket up to date.
    OnConnectStatusChanged(inetSocketAddress, newStatus, TRUE /* updateSocketState */);

    *ret = mStatus == SOCKET_STATUS_CONNECTED;
    return NOERROR;
}

ECode SocketChannelImpl::FinishConnect(
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = FALSE;

    {    AutoLock syncLock(this);
        if (!IsOpen()) {
            // throw new ClosedChannelException();
            return E_CLOSED_CHANNEL_EXCEPTION;
        }
        if (mStatus == SOCKET_STATUS_CONNECTED) {
            *ret = TRUE;
            return NOERROR;
        }
        if (mStatus != SOCKET_STATUS_PENDING) {
            // throw new NoConnectionPendingException();
            return E_NO_CONNECTION_PENDING_EXCEPTION;
        }
    }

    Boolean finished = FALSE;
    // try {
        Begin();
        AutoPtr<IInetAddress> inetAddress;
        mConnectAddress->GetAddress((IInetAddress**)&inetAddress);
        Int32 port = 0;
        mConnectAddress->GetPort(&port);
        ECode ec = IoBridge::IsConnected(mFd, inetAddress, port, 0, 0, &finished); // Return immediately.
    // } catch (ConnectException e) {
        if (ec != NOERROR) {
            if (IsOpen()) {
                Close();
                finished = TRUE;
            }
            return ec;
        }

    // } finally {
        End(finished);
    // }

    {    AutoLock syncLock(this);
        mStatus = (finished ? SOCKET_STATUS_CONNECTED : mStatus);
        if (finished && mSocket != NULL) {
            AutoPtr<IInetAddress> addr;
            mConnectAddress->GetAddress((IInetAddress**)&addr);
            Int32 portnum = 0;
            mConnectAddress->GetPort(&portnum);
            mSocket->OnConnect(ISocketAddress::Probe(addr), portnum);
        }
    }
    *ret = finished;
    return NOERROR;
}

ECode SocketChannelImpl::Read(
    /* [in] */ IByteBuffer* dst,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = 0;

    FAIL_RETURN(((Buffer*)IBuffer::Probe(dst))->CheckWritable());
    FAIL_RETURN(CheckOpenConnected());
    Boolean isflag = FALSE;
    if (IBuffer::Probe(dst)->HasRemaining(&isflag), !isflag) {
        *ret = 0;
        return NOERROR;
    }
    return ReadImpl(dst, ret);
}

ECode SocketChannelImpl::Read(
    /* [in] */ ArrayOf<IByteBuffer*>* targets,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int64* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = 0;

    FAIL_RETURN(Arrays::CheckOffsetAndCount(targets->GetLength(), offset, length));
    FAIL_RETURN(CheckOpenConnected());
    Int32 totalCount = 0;
    FileChannelImpl::CalculateTotalRemaining(targets, offset, length, TRUE, &totalCount);
    if (totalCount == 0) {
        *ret = 0;
        return NOERROR;
    }
    AutoPtr< ArrayOf<Byte> > readArray = ArrayOf<Byte>::Alloc(totalCount);
    AutoPtr<IByteBuffer> readBuffer;
    ByteBuffer::Wrap(readArray, (IByteBuffer**)&readBuffer);
    Int32 readCount = 0;
    // read data to readBuffer, and then transfer data from readBuffer to targets.
    ReadImpl(readBuffer, &readCount);
    IBuffer::Probe(readBuffer)->Flip();
    if (readCount > 0) {
        Int32 left = readCount;
        Int32 index = offset;
        // transfer data from readArray to targets
        while (left > 0) {
            Int32 remainvalue = 0;
            IBuffer::Probe((*targets)[index])->GetRemaining(&remainvalue);
            Int32 putLength = Elastos::Core::Math::Min(remainvalue, left);
            (*targets)[index]->Put(readArray, readCount - left, putLength);
            index++;
            left -= putLength;
        }
    }
    *ret = readCount;
    return NOERROR;
}

ECode SocketChannelImpl::Write(
    /* [in] */ IByteBuffer* src,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;

    if (src == NULL) {
        // throw new NullPointerException("src == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    FAIL_RETURN(CheckOpenConnected());
    Boolean isflag = FALSE;
    if (IBuffer::Probe(src)->HasRemaining(&isflag), !isflag) {
        *number = 0;
        return NOERROR;
    }
    return WriteImpl(src, number);
}

ECode SocketChannelImpl::Write(
    /* [in] */ ArrayOf<IByteBuffer*>* sources,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int64* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = 0;

    FAIL_RETURN(Arrays::CheckOffsetAndCount(sources->GetLength(), offset, length));
    FAIL_RETURN(CheckOpenConnected());
    Int32 count = 0;
    FileChannelImpl::CalculateTotalRemaining(sources, offset, length, FALSE, &count);
    if (count == 0) {
        *ret = 0;
        return NOERROR;
    }
    AutoPtr<IByteBuffer> writeBuf;
    ByteBuffer::Allocate(count, (IByteBuffer**)&writeBuf);
    for (Int32 val = offset; val < length + offset; val++) {
        AutoPtr<IByteBuffer> source = (*sources)[val];
        Int32 oldPosition = 0;
        IBuffer::Probe(source)->GetPosition(&oldPosition);
        writeBuf->Put(source);
        IBuffer::Probe(source)->SetPosition(oldPosition);
    }
    IBuffer::Probe(writeBuf)->Flip();
    Int32 result = 0;
    WriteImpl(writeBuf, &result);
    Int32 val = offset;
    Int32 written = result;
    while (result > 0) {
        AutoPtr<IByteBuffer> source = (*sources)[val];
        Int32 remainvalue = 0;
        IBuffer::Probe(source)->GetRemaining(&remainvalue);
        Int32 gap = Elastos::Core::Math::Min(result, remainvalue);
        Int32 oldPosition = 0;
        IBuffer::Probe(source)->GetPosition(&oldPosition);
        IBuffer::Probe(source)->SetPosition(oldPosition + gap);
        val++;
        result -= gap;
    }
    *ret = written;
    return NOERROR;
}

ECode SocketChannelImpl::GetFD(
    /* [out] */ IFileDescriptor** dstcriptor)
{
    VALIDATE_NOT_NULL(dstcriptor)

    *dstcriptor = mFd;
    REFCOUNT_ADD(*dstcriptor)
    return NOERROR;
}

ECode SocketChannelImpl::OnAccept(
    /* [in] */ IInetSocketAddress* remoteAddress,
    /* [in] */ Boolean updateSocketState)
{
    OnBind(updateSocketState);
    OnConnectStatusChanged(remoteAddress, SOCKET_STATUS_CONNECTED, updateSocketState);
    return NOERROR;
}

void SocketChannelImpl::OnBind(
    /* [in] */ Boolean updateSocketState)
{
    AutoPtr<ISocketAddress> sa;
    // try {
    CLibcore::sOs->Getsockname(mFd, (ISocketAddress**)&sa);
    // } catch (ErrnoException errnoException) {
    //     throw new AssertionError(errnoException);
    // }
    mIsBound = TRUE;
    AutoPtr<IInetSocketAddress> localSocketAddress = IInetSocketAddress::Probe(sa);
    localSocketAddress->GetAddress((IInetAddress**)&mLocalAddress);
    localSocketAddress->GetPort(&mLocalPort);
    if (updateSocketState && mSocket != NULL) {
        mSocket->OnBind(mLocalAddress, mLocalPort);
    }
}

void SocketChannelImpl::OnConnectStatusChanged(
    /* [in] */ IInetSocketAddress* address,
    /* [in] */ Int32 mStatus,
    /* [in] */ Boolean updateSocketState)
{
    // this.mStatus = mStatus;
    // mConnectAddress = address;
    // if (mStatus == SOCKET_STATUS_CONNECTED && updateSocketState && mSocket != NULL) {
    //     mSocket.onConnect(mConnectAddress->getAddress(), mConnectAddress->getPort());
    // }
}

ECode SocketChannelImpl::ValidateAddress(
    /* [in] */ ISocketAddress* socketAddress,
    /* [out] */ IInetSocketAddress** sockAddr)
{
    VALIDATE_NOT_NULL(sockAddr)

    if (socketAddress == NULL) {
        // throw new IllegalArgumentException("socketAddress == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (!IInetSocketAddress::Probe(socketAddress)) {
        // throw new UnsupportedAddressTypeException();
        return E_UNSUPPORTED_ADDRESS_TYPE_EXCEPTION;
    }
    AutoPtr<IInetSocketAddress> inetSocketAddress = IInetSocketAddress::Probe(socketAddress);
    Boolean isflag = FALSE;
    if (inetSocketAddress->IsUnresolved(&isflag), isflag) {
        // throw new UnresolvedAddressException();
        return E_UNRESOLVED_ADDRESS_EXCEPTION;
    }
    *sockAddr = inetSocketAddress;
    REFCOUNT_ADD(*sockAddr)
    return NOERROR;
}

ECode SocketChannelImpl::ImplCloseSelectableChannel()
{
    if (mStatus != SOCKET_STATUS_CLOSED) {
        mStatus = SOCKET_STATUS_CLOSED;
        // IoBridge.closeAndSignalBlockedThreads(mFd) is idempotent: It is safe to call on an
        // already-closed file descriptor.
        IoBridge::CloseAndSignalBlockedThreads(mFd);
        Boolean isflag = FALSE;
        if (mSocket != NULL && (mSocket->IsClosed(&isflag), !isflag)) {
            mSocket->OnClose();
        }
    }
    return NOERROR;
}

ECode SocketChannelImpl::ImplConfigureBlocking(
    /* [in] */ Boolean blocking)
{
    return IoUtils::SetBlocking(mFd, blocking);
}

ECode SocketChannelImpl::ReadImpl(
    /* [in] */ IByteBuffer* dst,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = 0;

    {    AutoLock syncLock(mReadLock);
        Int32 readCount = 0;
        // try {
            Boolean isflag = FALSE;
            if (IsBlocking(&isflag), isflag) {
                Begin();
            }
            IoBridge::Recvfrom(TRUE, mFd, dst, 0, NULL, FALSE, &readCount);
            if (readCount > 0) {
                Int32 pos = 0;
                IBuffer::Probe(dst)->GetPosition(&pos);
                IBuffer::Probe(dst)->SetPosition(pos + readCount);
            }
        // } finally {
            if (IsBlocking(&isflag), isflag) {
                End(readCount > 0);
            }
        // }
        *ret = readCount;
    }
    return NOERROR;
}

ECode SocketChannelImpl::WriteImpl(
    /* [in] */ IByteBuffer* src,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = 0;

    {    AutoLock syncLock(mWriteLock);
        Boolean isflag = FALSE;
        if (IBuffer::Probe(src)->HasRemaining(&isflag), !isflag) {
            *ret = 0;
            return NOERROR;
        }
        Int32 writeCount = 0;
        // try {
            if (IsBlocking(&isflag), isflag) {
                Begin();
            }
            IoBridge::Sendto(mFd, src, 0, NULL, 0, &writeCount);
            if (writeCount > 0) {
                Int32 pos;
                IBuffer::Probe(src)->GetPosition(&pos);
                IBuffer::Probe(src)->SetPosition(pos + writeCount);
            }
        // } finally {
            if (IsBlocking(&isflag), isflag) {
                End(writeCount >= 0);
            }
        // }
        *ret = writeCount;
    }
    return NOERROR;
}

ECode SocketChannelImpl::CheckOpenConnected()
{
    if (!IsOpen()) {
        // throw new ClosedChannelException();
        return E_CLOSED_CHANNEL_EXCEPTION;
    }
    Boolean isflag = FALSE;
    if (IsConnected(&isflag), !isflag) {
        // throw new NotYetConnectedException();
        return E_NOT_YET_CONNECTED_EXCEPTION;
    }
    return NOERROR;
}

ECode SocketChannelImpl::CheckUnconnected()
{
    if (!IsOpen()) {
        // throw new ClosedChannelException();
        return E_CLOSED_CHANNEL_EXCEPTION;
    }
    if (mStatus == SOCKET_STATUS_CONNECTED) {
        // throw new AlreadyConnectedException();
        return E_ALREADY_CONNECTED_EXCEPTION;
    }
    if (mStatus == SOCKET_STATUS_PENDING) {
        // throw new ConnectionPendingException();
        return E_CONNECTION_PENDING_EXCEPTION;
    }
    return NOERROR;
}

Boolean SocketChannelImpl::IsEINPROGRESS(
    /* [in] */ ECode e)
{
    Boolean isflag = FALSE;
    if (IsBlocking(&isflag), isflag) {
        return FALSE;
    }
    assert(0 && "TODO");
    // if (e instanceof ConnectException) {
    //     Throwable cause = e.getCause();
    //     if (cause instanceof ErrnoException) {
    //         return ((ErrnoException) cause).errno == EINPROGRESS;
    //     }
    // }
    return FALSE;
}

} // namespace IO
} // namespace Elastos
