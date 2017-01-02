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

#include "PlainSocketImpl.h"
#include "InetAddress.h"
#include "CFileDescriptor.h"
#include "CInetSocketAddress.h"
#include "CInteger32.h"
#include "CInetAddressHelper.h"
#include "CSocks4Message.h"
#include "Arrays.h"
#include "CCloseGuard.h"
#include "CLibcore.h"
#include "IoBridge.h"
#include "OsConstants.h"
#include "Memory.h"
#include "CPlainSocketInputStream.h"
#include "CPlainSocketOutputStream.h"

using Elastos::IO::CFileDescriptor;
using Elastos::IO::EIID_IInputStream;
using Elastos::IO::EIID_IOutputStream;
using Elastos::Net::CInetSocketAddress;
using Elastos::Net::ProxyType;
using Elastos::Net::CInetAddressHelper;
using Elastos::Net::Socks4Message;
using Elastos::Net::CSocks4Message;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::CCloseGuard;
using Elastos::Utility::Arrays;
using Libcore::IO::IoBridge;
using Libcore::IO::CLibcore;
using Libcore::IO::Memory;
using Droid::System::OsConstants;

namespace Elastos {
namespace Net {

AutoPtr<IInetAddress> PlainSocketImpl::sLastConnectedAddress;
Int32 PlainSocketImpl::sLastConnectedPort = 0;

CAR_INTERFACE_IMPL(PlainSocketImpl, SocketImpl, IPlainSocketImpl)

PlainSocketImpl::PlainSocketImpl()
    :SocketImpl()
    , mStreaming(TRUE)
    , mShutdownInput(FALSE)
{
    mGuard = CCloseGuard::Get();
}

PlainSocketImpl::~PlainSocketImpl()
{
    Finalize();
}

ECode PlainSocketImpl::constructor(
    /* [in] */ IFileDescriptor* fd)
{
    mFd = fd;
    Boolean bValid;
    fd->Valid(&bValid);
    if (bValid) {
       mGuard->Open(String("PlainSocketImpl::Close"));
    }
    return NOERROR;
}

ECode PlainSocketImpl::constructor(
    /* [in] */ IProxy* proxy)
{
    AutoPtr<IFileDescriptor> fd;
    CFileDescriptor::New((IFileDescriptor**)&fd);
    constructor(fd);
    mProxy = proxy;
    return NOERROR;
}

ECode PlainSocketImpl::constructor()
{
    AutoPtr<IFileDescriptor> fd;
    CFileDescriptor::New((IFileDescriptor**)&fd);
    constructor(fd);
    return NOERROR;
}

ECode PlainSocketImpl::constructor(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 localport,
    /* [in] */ IInetAddress* addr,
    /* [in] */ Int32 port)
{
    mFd = fd;
    mLocalport = localport;
    mAddress = addr;
    mPort = port;
    Boolean bValid;
    fd->Valid(&bValid);
    if (bValid) {
       mGuard->Open(String("PlainSocketImpl::Close"));
    }
    return NOERROR;
}

ECode PlainSocketImpl::Accept(
    /* [in] */ ISocketImpl* newImpl)
{
    if (UsingSocks()) {
        PlainSocketImpl* p = (PlainSocketImpl*)IPlainSocketImpl::Probe(newImpl);
        if (p != NULL) {
            p->SocksBind();
            p->SocksAccept();
            return NOERROR;
        }
    }

    PlainSocketImpl* p;
    AutoPtr<IInteger32> val;

    AutoPtr<IInetSocketAddress> peerAddress;
    CInetSocketAddress::New((IInetSocketAddress**)&peerAddress);
    AutoPtr<IFileDescriptor> clientFd;
    ECode ec = CLibcore::sOs->Accept(mFd, peerAddress, (IFileDescriptor**)&clientFd);
    FAIL_GOTO(ec, _Exit_)

    // TODO: we can't just set newImpl.fd to clientFd because a nio SocketChannel may
    // be sharing the FileDescriptor. http://b//4452981.
    p = (PlainSocketImpl*)IPlainSocketImpl::Probe(newImpl);
    if (p == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    p->mFd = clientFd;

    p->mAddress = NULL;
    peerAddress->GetAddress((IInetAddress**)&(p->mAddress));
    peerAddress->GetPort(&(p->mPort));

    // Reset the client's inherited read timeout to the Java-specified default of 0.
    CInteger32::New(0, (IInteger32**)&val);
    ISocketOptions::Probe(newImpl)->SetOption(ISocketOptions::_SO_TIMEOUT, val);
    IoBridge::GetSocketLocalPort(p->mFd, &(p->mLocalport));
    return NOERROR;

_Exit_:
    return E_SOCKET_EXCEPTION;
}

Boolean PlainSocketImpl::UsingSocks()
{
    if (mProxy != NULL) {
        ProxyType type;
        mProxy->GetType(&type);
        if (type == ProxyType_SOCKS) {
            return TRUE;
        }
    }
    return FALSE;
}

ECode PlainSocketImpl::CheckNotClosed()
{
    Boolean value  = FALSE;
    mFd->Valid(&value);
    if (!value) {
        return E_SOCKET_CLOSE_EXCEPTION;
    }
    return NOERROR;
}

ECode PlainSocketImpl::Available(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    FAIL_RETURN(CheckNotClosed());
    // we need to check if the input has been shutdown. If so
    // we should return that there is no data to be read
    if (mShutdownInput) {
        *value = 0;
        return NOERROR;
    }

    return IoBridge::Available(mFd, value);
}

ECode PlainSocketImpl::Bind(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port)
{
    FAIL_RETURN(IoBridge::Bind(mFd, address, port));

    if (port != 0) {
        mLocalport = port;
        return NOERROR;
    }
    else {
       return IoBridge::GetSocketLocalPort(mFd, &mLocalport);
    }
}

ECode PlainSocketImpl::OnBind(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port)
{
    mLocalport = port;
    return NOERROR;
}

ECode PlainSocketImpl::Close()
{
    mGuard->Close();
    return IoBridge::CloseAndSignalBlockedThreads(mFd);
}

ECode PlainSocketImpl::OnClose()
{
    return mGuard->Close();
}

ECode PlainSocketImpl::Connect(
    /* [in] */ const String& aHost,
    /* [in] */ Int32 aPort)
{
    AutoPtr<IInetAddressHelper> helper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper);
    AutoPtr<IInetAddress> addr;
    helper->GetByName(aHost, (IInetAddress**)&addr);
    return Connect(addr, aPort);
}

ECode PlainSocketImpl::Connect(
    /* [in] */ IInetAddress* anAddr,
    /* [in] */ Int32 aPort)
{
    return Connect(anAddr, aPort, 0);
}

ECode PlainSocketImpl::Connect(
    /* [in] */ IInetAddress* anAddr,
    /* [in] */ Int32 aPort,
    /* [in] */ Int32 timeout)
{
    AutoPtr<IInetAddress> normalAddr;
    Boolean isAny;
    anAddr->IsAnyLocalAddress(&isAny);
    if (isAny) {
        AutoPtr<IInetAddressHelper> helper;
        CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper);
        helper->GetLocalHost((IInetAddress**)&normalAddr);
    }
    else {
        normalAddr = anAddr;
    }

    ECode ec;
    if (mStreaming && UsingSocks()) {
        ec = SocksConnect(anAddr, aPort, 0);
    }
    else {
        ec = IoBridge::Connect(mFd, normalAddr, aPort, timeout);
    }

    mAddress = normalAddr;
    mPort = aPort;
    return ec;
}

ECode PlainSocketImpl::OnConnect(
    /* [in] */ IInetAddress* remoteAddress,
    /* [in] */ Int32 remotePort)
{
    mAddress = remoteAddress;
    mPort = remotePort;
    return NOERROR;
}

ECode PlainSocketImpl::Create(
    /* [in] */ Boolean streaming)
{
    mStreaming = streaming;
    AutoPtr<IFileDescriptor> outfd;
    FAIL_RETURN(IoBridge::Socket(streaming, (IFileDescriptor**)&outfd));
    mFd = outfd;
    return NOERROR;
}

ECode PlainSocketImpl::Finalize()
{
    if (mGuard != NULL) {
        mGuard->WarnIfOpen();
    }
    ECode ec = Close();
    // SocketImpl::Finalize();
    return ec;
}

ECode PlainSocketImpl::GetInputStream(
    /* [out] */ IInputStream** in)
{
    VALIDATE_NOT_NULL(in)

    FAIL_RETURN(CheckNotClosed());

    return CPlainSocketInputStream::New(this, in);
}

ECode PlainSocketImpl::GetOption(
    /* [in] */ Int32 option,
    /* [out] */ IInterface** res)
{
    VALIDATE_NOT_NULL(res)

    AutoPtr<IInterface> out;
    IoBridge::GetSocketOption(mFd, option, (IInterface**)&out);
    *res = out;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode PlainSocketImpl::GetOutputStream(
    /* [out] */ IOutputStream** out)
{
    VALIDATE_NOT_NULL(out)

    FAIL_RETURN(CheckNotClosed());
    return CPlainSocketOutputStream::New(this, out);
}

ECode PlainSocketImpl::Listen(
    /* [in] */ Int32 backlog)
{
    if (UsingSocks()) {
        // Do nothing for a SOCKS connection. The listen occurs on the
        // server during the bind.
        return E_SOCKET_EXCEPTION;
    }
    return CLibcore::sOs->Listen(mFd, backlog);
}

ECode PlainSocketImpl::SetOption(
    /* [in] */ Int32 option,
    /* [in] */ IInterface* value)
{
    return IoBridge::SetSocketOption(mFd, option, value);
}

Int32 PlainSocketImpl::SocksGetServerPort()
{
    AutoPtr<ISocketAddress> addr;
    mProxy->GetAddress((ISocketAddress**)&addr);
    Int32 port;
    IInetSocketAddress::Probe(addr)->GetPort(&port);
    return port;
}

AutoPtr<IInetAddress> PlainSocketImpl::SocksGetServerAddress()
{
    String proxyName;
    // get socks server address from proxy. It is unnecessary to check
    // "socksProxyHost" property, since all proxy setting should be
    // determined by ProxySelector.
    AutoPtr<ISocketAddress> saddr;
    mProxy->GetAddress((ISocketAddress**)&saddr);
    AutoPtr<IInetSocketAddress> addr = IInetSocketAddress::Probe(saddr);
    addr->GetHostName(&proxyName);
    if (proxyName.IsNull()) {
        AutoPtr<IInetAddress> iaddr;
        addr->GetAddress((IInetAddress**)&iaddr);
        iaddr->GetHostAddress(&proxyName);
    }

    AutoPtr<IInetAddressHelper> helper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper);
    AutoPtr<IInetAddress> retAddr;
    helper->GetByName(proxyName, (IInetAddress**)&retAddr);
    return retAddr;
}

ECode PlainSocketImpl::SocksConnect(
    /* [in] */ IInetAddress* applicationServerAddress,
    /* [in] */ Int32 applicationServerPort,
    /* [in] */ Int32 timeout)
{
    AutoPtr<IInetAddress> outadd = SocksGetServerAddress();
    IoBridge::Connect(mFd, outadd, SocksGetServerPort(), timeout);
    SocksRequestConnection(applicationServerAddress, applicationServerPort);
    sLastConnectedAddress = applicationServerAddress;
    sLastConnectedPort = applicationServerPort;
    return NOERROR;
}

ECode PlainSocketImpl::SocksRequestConnection(
    /* [in] */ IInetAddress* applicationServerAddress,
    /* [in] */ Int32 applicationServerPort)
{
    SocksSendRequest(ISocks4Message::COMMAND_CONNECT,
            applicationServerAddress, applicationServerPort);

    AutoPtr<ISocks4Message> reply;
    SocksReadReply((ISocks4Message**)&reply);
    if (reply != NULL) {
        Int32 res;
        reply->GetCommandOrResult(&res);
        if (res != ISocks4Message::RETURN_SUCCESS) {
            return E_SOCKET_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode PlainSocketImpl::SocksAccept()
{
    AutoPtr<ISocks4Message> reply;
    SocksReadReply((ISocks4Message**)&reply);
    if (reply != NULL) {
        Int32 res;
        reply->GetCommandOrResult(&res);
        if (res != ISocks4Message::RETURN_SUCCESS) {
            return E_SOCKET_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode PlainSocketImpl::ShutdownInput()
{
    mShutdownInput = TRUE;
    return CLibcore::sOs->Shutdown(mFd, OsConstants::_SHUT_RD);
}

ECode PlainSocketImpl::ShutdownOutput()
{
    return CLibcore::sOs->Shutdown(mFd, OsConstants::_SHUT_WR);
}

ECode PlainSocketImpl::SocksBind()
{
    AutoPtr<IInetAddress> addr = SocksGetServerAddress();
    AutoPtr<ArrayOf<Byte> > array;
    addr->GetAddress((ArrayOf<Byte>**)&array);

    FAIL_RETURN(IoBridge::Connect(mFd, addr, SocksGetServerPort()));

    // There must be a connection to an application host for the bind to
    // work.
    if (sLastConnectedAddress == NULL) {
        return E_INVALID_SOCKET_EXCEPTION;
    }

    // Use the last connected address and port in the bind request.
    SocksSendRequest(ISocks4Message::COMMAND_BIND, sLastConnectedAddress,
            sLastConnectedPort);

    AutoPtr<ISocks4Message> reply;
    SocksReadReply((ISocks4Message**)&reply);

    if (reply == NULL) {
        return E_SOCKET_EXCEPTION;
    }
    Int32 res;
    reply->GetCommandOrResult(&res);
    if (res != ISocks4Message::RETURN_SUCCESS) {
        return E_SOCKET_EXCEPTION;
    }

    mAddress = NULL;
    // A peculiarity of socks 4 - if the address returned is 0, use the
    // original socks server address.
    Int32 ip;
    reply->GetIP(&ip);
    if (ip == 0) {
        mAddress = SocksGetServerAddress();
    }
    else {
        // IPv6 support not yet required as
        // currently the Socks4Message.getIP() only returns int,
        // so only works with IPv4 4byte addresses
        AutoPtr<ArrayOf<Byte> > replyBytes = ArrayOf<Byte>::Alloc(4);
        Int32 replyip = 0;
        reply->GetIP(&replyip);
        Memory::PokeInt32(replyBytes, 0, replyip, Elastos::IO::ByteOrder_BIG_ENDIAN);
        AutoPtr<IInetAddressHelper> helper;
        CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper);
        helper->GetByAddress(replyBytes, (IInetAddress**)&mAddress);
    }

    reply->GetPort(&mLocalport);
    return NOERROR;
}

ECode PlainSocketImpl::SocksSendRequest(
    /* [in] */ Int32 command,
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port)
{
    Socks4Message request;
    request.SetCommandOrResult(command);
    request.SetPort(port);
    AutoPtr<ArrayOf<Byte> > array;
    mAddress->GetAddress((ArrayOf<Byte>**)&array);
    request.SetIP(array);
    request.SetUserId(String("default"));

    AutoPtr<IOutputStream> out;
    GetOutputStream((IOutputStream**)&out);
    AutoPtr<ArrayOf<Byte> > bytes;
    request.GetBytes((ArrayOf<Byte>**)&bytes);
    Int32 length;
    request.GetLength(&length);
    return out->Write(bytes, 0, length);
}

ECode PlainSocketImpl::SocksReadReply(
    /* [out] */ ISocks4Message** ppMessage)
{
    AutoPtr<ISocks4Message> reply;
    CSocks4Message::New((ISocks4Message**)&reply);
    Int32 bytesRead = 0;
    while (bytesRead < ISocks4Message::REPLY_LENGTH) {
        Int32 count;
        AutoPtr<IInputStream> in;
        GetInputStream((IInputStream**)&in);
        AutoPtr<ArrayOf<Byte> > bytes;
        reply->GetBytes((ArrayOf<Byte>**)&bytes);
        in->Read(bytes, bytesRead, Socks4Message::REPLY_LENGTH - bytesRead, &count);
        if (-1 == count) {
            break;
        }
        bytesRead += count;
    }
    if (ISocks4Message::REPLY_LENGTH != bytesRead) {
        return E_MALFORMED_REPLY_EXCEPTION;
    }

    *ppMessage = reply;
    REFCOUNT_ADD(*ppMessage)
    return NOERROR;
}

ECode PlainSocketImpl::Connect(
    /* [in] */ ISocketAddress* remoteAddr,
    /* [in] */ Int32 timeout)
{
    AutoPtr<IInetSocketAddress> inetAddr = IInetSocketAddress::Probe(remoteAddr);
    AutoPtr<IInetAddress> addr;
    inetAddr->GetAddress((IInetAddress**)&addr);
    Int32 port;
    inetAddr->GetPort(&port);
    return Connect(addr, port, timeout);
}

ECode PlainSocketImpl::SupportsUrgentData(
    /* [out] */ Boolean* value)
{
    *value = TRUE;
    return NOERROR;
}

ECode PlainSocketImpl::SendUrgentData(
    /* [in] */ Int32 value)
{
    AutoPtr< ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(4);
    memcpy(buffer->GetPayload(), &value, 4);
    Int32 outvalue = 0;
    return CLibcore::sOs->Sendto(mFd, buffer, 0, 1, OsConstants::_MSG_OOB, NULL, 0, &outvalue);
}

ECode PlainSocketImpl::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = -1;

    if (byteCount == 0) {
        *value = 0;
        return NOERROR;
    }
    Arrays::CheckOffsetAndCount(buffer->GetLength(), offset, byteCount);
    if (mShutdownInput) {
        *value = -1;
        return NOERROR;
    }

    Int32 readCount;
    IoBridge::Recvfrom(TRUE, mFd, buffer, offset, byteCount, 0, NULL, FALSE, &readCount);
    // Return of zero bytes for a blocking socket means a timeout occurred
    if (readCount == 0) {
        // throw new SocketTimeoutException();
        return E_SOCKET_TIMEOUT_EXCEPTION;
    }
    // Return of -1 indicates the peer was closed
    if (readCount == -1) {
        mShutdownInput = TRUE;
    }
    *value = readCount;
    return NOERROR;
}

ECode PlainSocketImpl::Write(
    /* [in] */ ArrayOf<Byte> * buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;
    VALIDATE_NOT_NULL(buffer)

    Arrays::CheckOffsetAndCount(buffer->GetLength(), offset, byteCount);
    if (mStreaming) {
        while (byteCount > 0) {
            Int32 bytesWritten;
            FAIL_RETURN(IoBridge::Sendto(mFd, buffer, offset, byteCount, 0, NULL, 0, &bytesWritten));
            byteCount -= bytesWritten;
            offset += bytesWritten;
        }
    }
    else {
        // Unlike writes to a streaming socket, writes to a datagram
        // socket are all-or-nothing, so we don't need a loop here.
        // http://code.google.com/p/android/issues/detail?id=15304
        FAIL_RETURN(IoBridge::Sendto(mFd, buffer, offset, byteCount, 0, mAddress, mPort, value));
    }
    *value = byteCount;
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
