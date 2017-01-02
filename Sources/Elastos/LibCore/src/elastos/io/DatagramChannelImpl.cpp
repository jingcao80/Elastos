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

#include "DatagramChannelImpl.h"
#include "IoBridge.h"
#include "CLibcore.h"
#include "CFileDescriptor.h"
#include "SelectorProvider.h"
#include "CInetSocketAddress.h"
#include "CPlainDatagramSocketImpl.h"
#include "CDatagramPacket.h"
#include "InetAddress.h"
#include "AutoLock.h"
#include "SocketChannelImpl.h"
#include "libcore/utility/EmptyArray.h"
#include "Arrays.h"
#include "FileChannelImpl.h"
#include "Math.h"
#include "IoUtils.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::AutoLock;
using Elastos::Net::CInetSocketAddress;
using Elastos::Net::EIID_IDatagramSocket;
using Elastos::Net::IPlainDatagramSocketImpl;
using Elastos::Net::CPlainDatagramSocketImpl;
using Elastos::Net::IDatagramPacket;
using Elastos::Net::CDatagramPacket;
using Elastos::Net::InetAddress;
using Elastos::Utility::Arrays;
using Elastos::IO::Channels::EIID_IDatagramChannel;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IOs;
using Libcore::IO::IoUtils;
using Libcore::IO::IoBridge;
using Libcore::Utility::EmptyArray;

namespace Elastos{
namespace IO {

//==========================================================
//       DatagramChannelImpl::DatagramSocketAdapter
//==========================================================

DatagramChannelImpl::DatagramSocketAdapter::DatagramSocketAdapter()
{}

ECode DatagramChannelImpl::DatagramSocketAdapter::constructor(
    /* [in] */ IDatagramSocketImpl* socketimpl,
    /* [in] */ DatagramChannelImpl* channelimpl)
{
    FAIL_RETURN(DatagramSocket::constructor(socketimpl))

    mChannelImpl = channelimpl;
    // Sync state socket state with the channel it is being created from
    if (mChannelImpl->mIsBound) {
        OnBind(mChannelImpl->mLocalAddress, mChannelImpl->mLocalPort);
    }
    if (mChannelImpl->mConnected) {
        AutoPtr<IInetAddress> addr;
        Int32 numport;
        mChannelImpl->mConnectAddress->GetAddress((IInetAddress**)&addr);
        mChannelImpl->mConnectAddress->GetPort(&numport);
        OnConnect(addr, numport);
    }
    else {
        OnDisconnect();
    }

    Boolean isflag = FALSE;
    if (mChannelImpl->IsOpen(&isflag), !isflag) {
        OnClose();
    }
    return NOERROR;
}

ECode DatagramChannelImpl::DatagramSocketAdapter::GetChannel(
    /* [out] */ IDatagramChannel** channel)
{
    VALIDATE_NOT_NULL(channel)

    *channel = IDatagramChannel::Probe(mChannelImpl);
    REFCOUNT_ADD(*channel)
    return NOERROR;
}

ECode DatagramChannelImpl::DatagramSocketAdapter::Bind(
    /* [in] */ ISocketAddress* localAddr)
{
    Boolean isflag = FALSE;
    if (mChannelImpl->IsConnected(&isflag), isflag) {
        // throw new AlreadyConnectedException();
        return E_ALREADY_CONNECTED_EXCEPTION;
    }

    FAIL_RETURN(DatagramSocket::Bind(localAddr))
    mChannelImpl->OnBind(FALSE /* updateSocketState */);
    return NOERROR;
}

ECode DatagramChannelImpl::DatagramSocketAdapter::Connect(
    /* [in] */ ISocketAddress* peer)
{
    Boolean bval;
    if (IsConnected(&bval), bval) {
        // RI compatibility: If the socket is already connected this fails.
        // throw new IllegalStateException("Socket is already connected.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    FAIL_RETURN(DatagramSocket::Connect(peer))
    // Connect may have performed an implicit bind(). Sync up here.
    mChannelImpl->OnBind(FALSE /* updateSocketState */);

    IInetSocketAddress* inetSocketAddress = IInetSocketAddress::Probe(peer);
    AutoPtr<IInetAddress> address;
    inetSocketAddress->GetAddress((IInetAddress**)&address);
    Int32 port;
    inetSocketAddress->GetPort(&port);
    return mChannelImpl->OnConnect(address, port, FALSE /* updateSocketState */);
}

ECode DatagramChannelImpl::DatagramSocketAdapter::Connect(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port)
{
    // To avoid implementing connect() twice call this.connect(SocketAddress) in preference
    // to super.connect().
    // try {
    AutoPtr<ISocketAddress> isa;
    CInetSocketAddress::New(address, port, (ISocketAddress**)&isa);
    Connect(isa);
    // } catch (SocketException e) {
    //     // Ignored - there is nothing we can report here.
    // }
    return NOERROR;
}

ECode DatagramChannelImpl::DatagramSocketAdapter::Receive(
    /* [in] */ IDatagramPacket* pack)
{
    Boolean isflag = FALSE;
    if (mChannelImpl->IsBlocking(&isflag), !isflag) {
        // throw new IllegalBlockingModeException();
        return E_ILLEGAL_BLOCKING_MODE_EXCEPTION;
    }

    Boolean wasBound;
    IsBound(&wasBound);
    FAIL_RETURN(DatagramSocket::Receive(pack))
    if (!wasBound) {
        // DatagramSocket.receive() will implicitly bind if it hasn't been done explicitly.
        // Sync the channel state with the socket.
        mChannelImpl->OnBind(FALSE /* updateSocketState */);
    }
    return NOERROR;
}

ECode DatagramChannelImpl::DatagramSocketAdapter::Send(
    /* [in] */ IDatagramPacket* pack)
{
    Boolean isflag = FALSE;
    if (mChannelImpl->IsBlocking(&isflag), !isflag) {
        // throw new IllegalBlockingModeException();
        return E_ILLEGAL_BLOCKING_MODE_EXCEPTION;
    }
    return DatagramSocket::Send(pack);

    // DatagramSocket.send() will implicitly bind if it hasn't been done explicitly. Force
    // bind() here so that the channel state stays in sync with the socket.
    Boolean wasBound;
    IsBound(&wasBound);
    FAIL_RETURN(DatagramSocket::Send(pack))
    if (!wasBound) {
        // DatagramSocket.send() will implicitly bind if it hasn't been done explicitly.
        // Sync the channel state with the socket.
        mChannelImpl->OnBind(FALSE /* updateSocketState */);
    }
    return NOERROR;
}

ECode DatagramChannelImpl::DatagramSocketAdapter::Close()
{
    {    AutoLock syncLock(mChannelImpl);
        FAIL_RETURN(DatagramSocket::Close())

        Boolean isflag = FALSE;
        if (mChannelImpl->IsOpen(&isflag), isflag) {
            // try {
            return mChannelImpl->Close();
            // } catch (IOException e) {
                // Ignore
            // }
        }
    }

    return NOERROR;
}

ECode DatagramChannelImpl::DatagramSocketAdapter::Disconnect()
{
    FAIL_RETURN(DatagramSocket::Disconnect())
    return mChannelImpl->OnDisconnect(FALSE /* updateSocketState */);
}

//==========================================================
//       DatagramChannelImpl
//==========================================================
CAR_INTERFACE_IMPL(DatagramChannelImpl, DatagramChannel, IFileDescriptorChannel)

DatagramChannelImpl::DatagramChannelImpl()
    : mConnected(FALSE)
    , mIsBound(FALSE)
    , mLocalPort(0)
{
    CFileDescriptor::New((IFileDescriptor**)&mFd);
    CInetSocketAddress::New(0, (IInetSocketAddress**)&mConnectAddress);
}

ECode DatagramChannelImpl::constructor()
{
    SelectorProvider::GetProvider((ISelectorProvider**)&mSelprovider);
    return DatagramChannel::constructor(mSelprovider);
}

ECode DatagramChannelImpl::constructor(
    /* [in] */ ISelectorProvider* provider)
{
    FAIL_RETURN(DatagramChannel::constructor(provider))
    return IoBridge::Socket(FALSE, (IFileDescriptor**)&mFd);
}

ECode DatagramChannelImpl::GetSocket(
    /* [out] */ IDatagramSocket** socket)
{
    VALIDATE_NOT_NULL(socket)
    *socket = NULL;

    if (NULL == mSocket) {
        AutoPtr<IPlainDatagramSocketImpl> res;
        FAIL_RETURN(CPlainDatagramSocketImpl::New(mFd, mLocalPort, (IPlainDatagramSocketImpl**)&res));
        AutoPtr<DatagramSocketAdapter> dsa = new DatagramSocketAdapter();
        FAIL_RETURN(dsa->constructor(IDatagramSocketImpl::Probe(res), this))
        mSocket = IDatagramSocket::Probe(dsa);
    }

    *socket = mSocket;
    REFCOUNT_ADD(*socket);
    return NOERROR;
}

ECode DatagramChannelImpl::OnBind(
    /* [in] */ Boolean updateSocketState)
{
    AutoPtr<ISocketAddress> sa;
    ECode ec;
    do {
        if (FAILED(ec = CLibcore::sOs->Getsockname(mFd, (ISocketAddress**)&sa))) break;
    } while(FALSE);
    if (FAILED(ec)) {
        if ((ECode)E_ERRNO_EXCEPTION == ec)
            return E_ASSERTION_ERROR;
        return ec;
    }
    mIsBound = true;
    AutoPtr<IInetSocketAddress> localSocketAddress = IInetSocketAddress::Probe(sa);
    mLocalAddress = NULL;
    localSocketAddress->GetAddress((IInetAddress**)&mLocalAddress);
    localSocketAddress->GetPort(&mLocalPort);
    if (updateSocketState && mSocket != NULL) {
        mSocket->OnBind(mLocalAddress, mLocalPort);
    }
    return NOERROR;
}

ECode DatagramChannelImpl::IsConnected(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mConnected;
    return NOERROR;
}

ECode DatagramChannelImpl::Connect(
    /* [in] */ ISocketAddress* address,
    /* [out] */ IDatagramChannel** channel)
{
    VALIDATE_NOT_NULL(channel)

    // must be open
    FAIL_RETURN(CheckOpen())
    // status must be un-connected.
    if (mConnected) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // check the address
    AutoPtr<IInetSocketAddress> inetSocketAddress;
    SocketChannelImpl::ValidateAddress(address, (IInetSocketAddress**)&inetSocketAddress);
    AutoPtr<IInetAddress> remoteAddress;
    inetSocketAddress->GetAddress((IInetAddress**)&remoteAddress);
    Int32 remotePort;
    inetSocketAddress->GetPort(&remotePort);
    //try {
        Begin();
        IoBridge::Connect(mFd, remoteAddress, remotePort);
    //} catch (ConnectException e) {
        // ConnectException means connect fail, not exception
    //} finally {
        End(TRUE);
    //}

    // connect() performs a bind() if an explicit bind() was not performed. Keep the local
    // address state held by the channel and the socket up to date.
    if (!mIsBound) {
        OnBind(TRUE /* updateSocketState */);
    }

    // Keep the connected state held by the channel and the socket up to date.
    return OnConnect(remoteAddress, remotePort, TRUE /* updateSocketState */);
}

ECode DatagramChannelImpl::OnConnect(
    /* [in] */ IInetAddress* remoteAddress,
    /* [in] */ Int32 remotePort,
    /* [in] */ Boolean updateSocketState)
{
    mConnected = true;
    mConnectAddress = NULL;
    CInetSocketAddress::New(remoteAddress, remotePort, (IInetSocketAddress**)&mConnectAddress);
    if (updateSocketState && mSocket != NULL) {
        mSocket->OnConnect(remoteAddress, remotePort);
    }

    return NOERROR;
}

ECode DatagramChannelImpl::Disconnect()
{
    Boolean bConnected, bOpen;
    IsConnected(&bConnected);
    IsOpen(&bOpen);

    if (!bOpen || !bConnected) {
        return NOERROR;
    }

    // Keep the disconnected state held by the channel and the socket up to date.
    OnDisconnect(TRUE /* updateSocketState */);

    // try {
    FAIL_RETURN(CLibcore::sOs->Connect(mFd, InetAddress::UNSPECIFIED, 0));
    // } catch (ErrnoException errnoException) {
        // throw errnoException.rethrowAsIOException();
    // }

    return NOERROR;
}

ECode DatagramChannelImpl::OnDisconnect(
    /* [in] */ Boolean updateSocketState)
{
    mConnected = false;
    mConnectAddress = NULL;
    Boolean bval;
    if (updateSocketState && mSocket != NULL && (mSocket->IsConnected(&bval), bval)) {
        mSocket->OnDisconnect();
    }
    return NOERROR;
}

ECode DatagramChannelImpl::Receive(
    /* [in] */ IByteBuffer* target,
    /* [out] */ ISocketAddress** address)
{
    VALIDATE_NOT_NULL(address)
    *address = NULL;

    FAIL_RETURN(((Buffer*)target)->CheckWritable())
    FAIL_RETURN(CheckOpen())

    if (!mIsBound) {
        return NOERROR;
    }

    AutoPtr<ISocketAddress> retAddr;
    // try {
    Boolean bval, loop;
    Begin();

    // receive real data packet, (not peek)
    {    AutoLock syncLock(mReadLock);
        IsBlocking(&loop);
        if (IBuffer::Probe(target)->IsDirect(&bval), !bval) {
            ReceiveImpl(target, loop, (ISocketAddress**)&retAddr);
        }
        else {
            ReceiveDirectImpl(target, loop, (ISocketAddress**)&retAddr);
        }
    }
    // } catch (InterruptedIOException e) {
        // this line used in Linux
        // return NULL;
    // } finally {
        End(retAddr != NULL);
    // }
    *address = retAddr;
    REFCOUNT_ADD(*address)
    return NOERROR;
}

ECode DatagramChannelImpl::ReceiveImpl(
    /* [in] */ IByteBuffer* target,
    /* [in] */ Boolean loop,
    /* [out] */ ISocketAddress** addr)
{
    VALIDATE_NOT_NULL(addr)
    *addr = NULL;

    AutoPtr<ISocketAddress> retAddr;
    AutoPtr<IDatagramPacket> receivePacket;
    Int32 oldPosition;
    IBuffer::Probe(target)->GetPosition(&oldPosition);
    Int32 received;
    Boolean bval;
    // TODO: disallow mapped buffers and lose this conditional?
    if (IBuffer::Probe(target)->HasArray(&bval), bval) {
        AutoPtr< ArrayOf<Byte> > bytearr;
        target->GetArray((ArrayOf<Byte>**)&bytearr);
        Int32 offset = 0;
        Int32 remain = 0;
        IBuffer::Probe(target)->GetArrayOffset(&offset);
        IBuffer::Probe(target)->GetRemaining(&remain);
        FAIL_RETURN(CDatagramPacket::New(bytearr, oldPosition + offset, remain,
            (IDatagramPacket**)&receivePacket));
    }
    else {
        Int32 remain = 0;
        IBuffer::Probe(target)->GetRemaining(&remain);
        AutoPtr< ArrayOf<Byte> > bytearr = ArrayOf<Byte>::Alloc(remain);
        FAIL_RETURN(CDatagramPacket::New(bytearr, remain,
            (IDatagramPacket**)&receivePacket));
    }

    do {
        AutoPtr< ArrayOf<Byte> > bytearr;
        receivePacket->GetData((ArrayOf<Byte>**)&bytearr);
        Int32 offset, length;
        receivePacket->GetOffset(&offset);
        receivePacket->GetLength(&length);
        Boolean isflag = FALSE;
        IsConnected(&isflag);
        FAIL_RETURN(IoBridge::Recvfrom(FALSE, mFd, bytearr, offset, length, 0, receivePacket, isflag, &received));
        AutoPtr<IInetAddress> outnet;
        receivePacket->GetAddress((IInetAddress**)&outnet);
        if (outnet != NULL) {
            if (received > 0) {
                if (IBuffer::Probe(target)->HasArray(&bval), bval) {
                    IBuffer::Probe(target)->SetPosition(oldPosition + received);
                }
                else {
                    // copy the data of received packet
                    target->Put(bytearr, 0, received);
                }
            }
            receivePacket->GetSocketAddress((ISocketAddress**)&retAddr);
            break;
        }
    } while (loop);

    *addr = retAddr;
    REFCOUNT_ADD(*addr)
    return NOERROR;
}

ECode DatagramChannelImpl::ReceiveDirectImpl(
    /* [in] */ IByteBuffer* target,
    /* [in] */ Boolean loop,
    /* [out] */ ISocketAddress** addr)
{
    VALIDATE_NOT_NULL(addr)
    *addr = NULL;

    AutoPtr<ISocketAddress> retAddr;
    AutoPtr<IDatagramPacket> receivePacket;
    CDatagramPacket::New(EmptyArray::BYTE, 0, (IDatagramPacket**)&receivePacket);
    Int32 oldposition;
    IBuffer::Probe(target)->GetPosition(&oldposition);
    Int32 received;
    do {
        Boolean isflag = FALSE;
        IsConnected(&isflag);
        IoBridge::Recvfrom(FALSE, mFd, target, 0, receivePacket, isflag, &received);
        AutoPtr<IInetAddress> address;
        receivePacket->GetAddress((IInetAddress**)&address);
        if (address != NULL) {
            // copy the data of received packet
            if (received > 0) {
                IBuffer::Probe(target)->SetPosition(oldposition + received);
            }
            retAddr = NULL;
            receivePacket->GetSocketAddress((ISocketAddress**)&retAddr);
            break;
        }
    } while (loop);

    *addr = retAddr;
    REFCOUNT_ADD(*addr)
    return NOERROR;
}

ECode DatagramChannelImpl::Send(
    /* [in] */ IByteBuffer* source,
    /* [in] */ ISocketAddress* socketAddress,
    /* [out] */ Int32 * count)
{
    VALIDATE_NOT_NULL(count)
    *count = 0;

    FAIL_RETURN(CheckNotNull(source))
    FAIL_RETURN(CheckOpen())

    AutoPtr<IInetSocketAddress> isa = IInetSocketAddress::Probe(socketAddress);
    AutoPtr<IInetAddress> ia;
    isa->GetAddress((IInetAddress**)&ia);
    if (ia == NULL) {
        // throw new IOException();
        return E_IO_EXCEPTION;
    }

    Boolean isflag = FALSE;
    IsConnected(&isflag);
    if (isflag && !Object::Equals(mConnectAddress, isa)) {
        // throw new IllegalArgumentException("Connected to " + connectAddress +
        //                                    ", not " + socketAddress);

        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    {    AutoLock syncLock(mWriteLock);
        Int32 sendCount = 0;
        // try {
            Begin();
            Int32 oldPosition;
            IBuffer::Probe(source)->GetPosition(&oldPosition);
            Int32 port;
            isa->GetPort(&port);
            IoBridge::Sendto(mFd, source, 0, ia, port, &sendCount);
            if (sendCount > 0) {
                IBuffer::Probe(source)->SetPosition(oldPosition + sendCount);
            }
            if (!mIsBound) {
                OnBind(TRUE /* updateSocketState */);
            }
        // } finally {
            End(sendCount >= 0);
        // }
        *count = sendCount;
    }
    return NOERROR;
}

ECode DatagramChannelImpl::Read(
    /* [in] */ IByteBuffer* target,
    /* [out] */ Int32 * count)
{
    VALIDATE_NOT_NULL(count)
    *count = 0;

    FAIL_RETURN(((Buffer*)target)->CheckWritable())
    FAIL_RETURN(CheckOpenConnected())

    Boolean bval;
    IBuffer::Probe(target)->HasRemaining(&bval);
    if (!bval) {
        return NOERROR;
    }

    Int32 readCount;
    IBuffer::Probe(target)->IsDirect(&bval);
    Boolean hasArray;
    IBuffer::Probe(target)->HasArray(&hasArray);
    if (bval || hasArray) {
        FAIL_RETURN(ReadImpl(target, &readCount))
        if (readCount > 0) {
            Int32 position;
            IBuffer::Probe(target)->GetPosition(&position);
            IBuffer::Probe(target)->SetPosition(position + readCount);
        }
    }
    else {
        Int32 remaining;
        IBuffer::Probe(target)->GetRemaining(&remaining);
        AutoPtr<ArrayOf<Byte> > readArray = ArrayOf<Byte>::Alloc(remaining);
        AutoPtr<IByteBuffer> readBuffer;
        ByteBuffer::Wrap(readArray, (IByteBuffer**)&readBuffer);
        FAIL_RETURN(ReadImpl(readBuffer, &readCount))
        if (readCount > 0) {
            target->Put(readArray, 0, readCount);
        }
    }
    *count = readCount;
    return NOERROR;
}

ECode DatagramChannelImpl::Read(
    /* [in] */ ArrayOf<IByteBuffer*> * targets,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int64* count)
{
    VALIDATE_NOT_NULL(count)
    *count = 0;

    FAIL_RETURN(Arrays::CheckOffsetAndCount(targets->GetLength(), offset, length))

    // status must be open and connected
    FAIL_RETURN(CheckOpenConnected())
    Int32 totalCount;
    FAIL_RETURN(FileChannelImpl::CalculateTotalRemaining(
        targets, offset, length, TRUE, &totalCount))
    if (totalCount == 0) {
        return NOERROR;
    }

    // read data to readBuffer, and then transfer data from readBuffer to
    // targets.
    AutoPtr<IByteBuffer> readBuffer;
    ByteBuffer::Allocate(totalCount, (IByteBuffer**)&readBuffer);
    Int32 readCount;
    FAIL_RETURN(ReadImpl(readBuffer, &readCount))
    Int32 left = readCount;
    Int32 index = offset;
    // transfer data from readBuffer to targets

    AutoPtr<ArrayOf<Byte> > readArray;
    readBuffer->GetArray((ArrayOf<Byte>**)&readArray);
    while (left > 0) {
        IByteBuffer* bb = (*targets)[index];
        Int32 remaining;
        IBuffer::Probe(bb)->GetRemaining(&remaining);
        Int32 putLength = Elastos::Core::Math::Min(remaining, left);
        bb->Put(readArray, readCount - left, putLength);
        index++;
        left -= putLength;
    }
    *count = readCount;
    return NOERROR;
}

ECode DatagramChannelImpl::ReadImpl(
    /* [in] */ IByteBuffer* dst,
    /* [out] */ Int32 * count)
{
    VALIDATE_NOT_NULL(count)
    *count = 0;

    {    AutoLock syncLock(mReadLock);
        Int32 readCount = 0;
        // try {
            Begin();
            Boolean isflag = FALSE;
            IsConnected(&isflag);
            IoBridge::Recvfrom(FALSE, mFd, dst, 0, NULL, isflag, &readCount);
        // } catch (InterruptedIOException e) {
        //     // InterruptedIOException will be thrown when timeout.
        //     return 0;
        // } finally {
            End(readCount > 0);
        // }
        *count = readCount;
    }
    return NOERROR;
}

ECode DatagramChannelImpl::Write(
    /* [in] */ IByteBuffer* src,
    /* [out] */ Int32 * count)
{
    VALIDATE_NOT_NULL(count)
    *count = 0;

    FAIL_RETURN(CheckNotNull(src))
    FAIL_RETURN(CheckOpenConnected())

    Boolean bval;
    IBuffer::Probe(src)->HasRemaining(&bval);
    if (!bval) {
        return NOERROR;
    }

    Int32 writeCount;
    FAIL_RETURN(WriteImpl(src, &writeCount))
    if (writeCount > 0) {
        Int32 position;
        IBuffer::Probe(src)->GetPosition(&position);
        IBuffer::Probe(src)->SetPosition(position + writeCount);
    }
    *count = writeCount;
    return NOERROR;
}

ECode DatagramChannelImpl::Write(
    /* [in] */ ArrayOf<IByteBuffer*> * sources,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int64* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = 0;

    FAIL_RETURN(Arrays::CheckOffsetAndCount(sources->GetLength(), offset, length))

    // status must be open and connected
    FAIL_RETURN(CheckOpenConnected())
    Int32 count;
    FileChannelImpl::CalculateTotalRemaining(sources, offset, length, FALSE, &count);
    if (count == 0) {
        return NOERROR;
    }

    AutoPtr<IByteBuffer> writeBuf;
    ByteBuffer::Allocate(count, (IByteBuffer**)&writeBuf);
    for (Int32 val = offset; val < length + offset; val++) {
        IByteBuffer* source = (*sources)[val];
        Int32 oldPosition;
        IBuffer::Probe(source)->GetPosition(&oldPosition);
        writeBuf->Put(source);
        IBuffer::Probe(source)->SetPosition(oldPosition);
    }
    IBuffer::Probe(writeBuf)->Flip();
    Int32 result;
    FAIL_RETURN(WriteImpl(writeBuf, &result))
    Int32 val = offset;
    Int32 written = result;
    while (result > 0) {
        IByteBuffer* source = (*sources)[val];
        Int32 remaining;
        IBuffer::Probe(source)->GetRemaining(&remaining);
        Int32 gap = Elastos::Core::Math::Min(result, remaining);
        Int32 oldPosition;
        IBuffer::Probe(source)->GetPosition(&oldPosition);
        IBuffer::Probe(source)->SetPosition(oldPosition + gap);
        val++;
        result -= gap;
    }
    *ret = written;
    return NOERROR;
}

ECode DatagramChannelImpl::WriteImpl(
    /* [in] */ IByteBuffer* buf,
    /* [out] */ Int32 * count)
{
    VALIDATE_NOT_NULL(count)
    *count = 0;

    {    AutoLock syncLock(mWriteLock);
        Int32 result = 0;
        // try {
            Begin();
            IoBridge::Sendto(mFd, buf, 0, NULL, 0, &result);
        // } finally {
            End(result > 0);
        // }
        return result;
    }
    return NOERROR;
}

ECode DatagramChannelImpl::ImplCloseSelectableChannel()
{
    // A closed channel is not connected.
    OnDisconnect(TRUE /* updateSocketState */);
    IoBridge::CloseAndSignalBlockedThreads(mFd);

    Boolean bval;
    if (mSocket != NULL && (mSocket->IsClosed(&bval), !bval)) {
        mSocket->OnClose();
    }
    return NOERROR;
}

ECode DatagramChannelImpl::ImplConfigureBlocking(
    /* [in] */ Boolean blocking)
{
    return IoUtils::SetBlocking(mFd, blocking);
}

ECode DatagramChannelImpl::GetFD(
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd)
    *fd = mFd;
    REFCOUNT_ADD(*fd)
    return NOERROR;
}

ECode DatagramChannelImpl::CheckOpen()
{
    Boolean bval;
    if (IsOpen(&bval), !bval) {
        // throw new ClosedChannelException();
        return E_CLOSED_CHANNEL_EXCEPTION;
    }
    return NOERROR;
}

ECode DatagramChannelImpl::CheckOpenConnected()
{
    FAIL_RETURN(CheckOpen())
    Boolean isflag = FALSE;
    IsConnected(&isflag);
    if (!isflag) {
        // throw new NotYetConnectedException();
        return E_NOT_YET_CONNECTED_EXCEPTION;
    }
    return NOERROR;
}

ECode DatagramChannelImpl::CheckNotNull(
    /* [in] */ IByteBuffer* source)
{
    if (source == NULL) {
        // throw new NullPointerException("source == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
