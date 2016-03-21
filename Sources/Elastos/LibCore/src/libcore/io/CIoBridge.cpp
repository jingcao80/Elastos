
#include "CIoBridge.h"
#include "CLibcore.h"
#include "droid/system/OsConstants.h"
#include "CAsynchronousCloseMonitor.h"
#include "net/CInetSocketAddress.h"
#include "droid/system/CStructLinger.h"
#include "droid/system/CStructPollfd.h"
#include "droid/system/CStructTimeval.h"
#include "core/Math.h"
#include "CInteger32.h"
#include "CBoolean.h"
#include "io/CIoUtils.h"
#include "io/CFileDescriptor.h"
#include "net/CInet6Address.h"
#include "net/NetworkInterface.h"
#include "CSystem.h"

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Libcore::IO::CIoUtils;
using Libcore::IO::IIoUtils;
using Elastos::IO::IBuffer;
using Elastos::IO::CFileDescriptor;
using Elastos::Net::CInetSocketAddress;
using Elastos::Net::IInet4Address;
using Elastos::Net::IInet6Address;
using Elastos::Net::CInet6Address;
using Elastos::Net::ISocketAddress;
using Elastos::Net::ISocketOptions;
using Elastos::Net::INetworkInterface;
using Elastos::Net::NetworkInterface;
using Elastos::Net::ISocketOptions;
using Elastos::Droid::System::IStructLinger;
using Elastos::Droid::System::IStructPollfd;
using Elastos::Droid::System::IStructTimeval;
using Elastos::Droid::System::CStructLinger;
using Elastos::Droid::System::CStructPollfd;
using Elastos::Droid::System::CStructTimeval;
using Elastos::Droid::System::OsConstants;
using Elastos::Droid::System::IStructGroupReq;
using Elastos::Droid::System::IStructGroupSourceReq;
using Elastos::Droid::System::IStructStat;

namespace Libcore {
namespace IO {

CAR_SINGLETON_IMPL(CIoBridge)

CAR_INTERFACE_IMPL(CIoBridge, Singleton, IIoBridge)

ECode CIoBridge::_Available(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Int32* avail)
{
    VALIDATE_NOT_NULL(avail);

//    try {
    Int32 available, result;
    FAIL_RETURN(CLibcore::sOs->IoctlInt(fd, OsConstants::_FIONREAD, &available, &result));
    if (available < 0) {
        // If the fd refers to a regular file, the result is the difference between
        // the file size and the file position. This may be negative if the position
        // is past the end of the file. If the fd refers to a special file masquerading
        // as a regular file, the result may be negative because the special file
        // may appear to have zero size and yet a previous read call may have
        // read some amount of data and caused the file position to be advanced.
        available = 0;
    }
    *avail = available;
    return NOERROR;
//    } catch (ErrnoException errnoException) {
//        if (errnoException.errno == ENOTTY) {
//            // The fd is unwilling to opine about its read buffer.
//            return 0;
//        }
//        throw errnoException.rethrowAsIOException();
//    }
}

ECode CIoBridge::_Bind(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port)
{
    AutoPtr<IInet6Address> inet6 = IInet6Address::Probe(inetAddress);
    AutoPtr<IInet6Address> v6Addr;
    AutoPtr<IInetAddress> arg = inetAddress;
    Int32 scopeid = 0;
    Boolean isLinkLocalAddress;

    if (inet6 != NULL && ((inet6->GetScopeId(&scopeid), scopeid) == 0) &&
            (inetAddress->IsLinkLocalAddress(&isLinkLocalAddress), isLinkLocalAddress)) {
       // Linux won't let you bind a link-local address without a scope id. Find one.
        AutoPtr<INetworkInterface> nif;
        NetworkInterface::GetByInetAddress(inetAddress, (INetworkInterface**)&nif);
        if (nif == NULL) {
            // throw new SocketException("Can't bind to a link-local address without a scope id: " + inetAddress);
            return E_SOCKET_EXCEPTION;
        }
        // try {
        String hostname;
        inetAddress->GetHostName(&hostname);
        AutoPtr< ArrayOf<Byte> > outbyte;
        inetAddress->GetAddress((ArrayOf<Byte>**)&outbyte);
        Int32 index = 0;
        nif->GetIndex(&index);
        CInet6Address::GetByAddress(hostname, outbyte, index, (IInet6Address**)&v6Addr);
        arg = IInetAddress::Probe(v6Addr);
        // } catch (UnknownHostException ex) {
        //     // throw new AssertionError(ex); // Can't happen.
        // }
    }
    // try {
    return CLibcore::sOs->Bind(fd, arg, port);
    // } catch (ErrnoException errnoException) {
    //    throw new BindException(errnoException.getMessage(), errnoException);
    // }
}

ECode CIoBridge::_Connect(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port)
{
//    try {
    return _Connect(fd, inetAddress, port, 0);
//    } catch (SocketTimeoutException ex) {
//        throw new AssertionError(ex); // Can't happen for a connect without a timeout.
//    }
}

ECode CIoBridge::_Connect(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [in] */ Int32 timeoutMs)
{
    // try {
    return ConnectErrno(fd, inetAddress, port, timeoutMs);
    // } catch (ErrnoException errnoException) {
    //     throw new ConnectException(connectDetail(inetAddress, port, timeoutMs, errnoException), errnoException);
    // } catch (SocketException ex) {
    //     throw ex; // We don't want to doubly wrap these.
    // } catch (SocketTimeoutException ex) {
    //     throw ex; // We don't want to doubly wrap these.
    // } catch (IOException ex) {
    //     throw new SocketException(ex);
    // }
}

ECode CIoBridge::ConnectErrno(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [in] */ Int32 timeoutMs)
{
    // With no timeout, just call connect(2) directly.
    if (timeoutMs == 0) {
        FAIL_RETURN(CLibcore::sOs->Connect(fd, inetAddress, port));
        return NOERROR;
    }

    // For connect with a timeout, we:
    //   1. set the socket to non-blocking,
    //   2. connect(2),
    //   3. loop using poll(2) to decide whether we're connected, whether we should keep
    //      waiting, or whether we've seen a permanent failure and should give up,
    //   4. set the socket back to blocking.

    AutoPtr<CSystem> system;
    Elastos::Core::CSystem::AcquireSingletonByFriend((CSystem**)&system);

    // 1. set the socket to non-blocking.
    AutoPtr<CIoUtils> utils;
    CIoUtils::AcquireSingletonByFriend((CIoUtils**)&utils);
    utils->SetBlocking(fd, FALSE); // TODO::

    // 2. call connect(2) non-blocking.
    Int64 finishTimeMs;
    system->GetCurrentTimeMillis(&finishTimeMs);
    finishTimeMs += timeoutMs;
    // try {
    ECode ec = CLibcore::sOs->Connect(fd, inetAddress, port);
    if (!FAILED(ec)) {
        utils->SetBlocking(fd, TRUE); // 4. set the socket back to blocking. // TODO::
        return NOERROR;
    }

    // } catch (ErrnoException errnoException) {
    //    if (errnoException.errno != EINPROGRESS) {
    //        throw errnoException;
    //    }
    //    // EINPROGRESS means we should keep trying...
    // }

    // 3. loop using poll(2).
    Int32 remainingTimeoutMs = 0;
    Int64 millis;

    Boolean isflag = FALSE;
    do {
        system->GetCurrentTimeMillis(&millis);
        remainingTimeoutMs = (Int32) (finishTimeMs - millis);
        if (remainingTimeoutMs <= 0) {
            // throw new SocketTimeoutException(connectDetail(inetAddress, port, timeoutMs, null));
            return E_SOCKET_TIMEOUT_EXCEPTION;
        }
    } while (_IsConnected(fd, inetAddress, port, timeoutMs, remainingTimeoutMs, &isflag), !isflag);
    utils->SetBlocking(fd, TRUE); // 4. set the socket back to blocking. // TODO::
    return NOERROR;
}

ECode CIoBridge::_CloseAndSignalBlockedThreads(
    /* [in] */ IFileDescriptor* fd)
{
    Boolean valid;
    if ((fd == NULL) || (fd->Valid(&valid), !valid)) {
        // Socket.close doesn't throw if you try to close an already-closed socket.
        return NOERROR;
    }
    AutoPtr<CAsynchronousCloseMonitor> monitor;
    CAsynchronousCloseMonitor::AcquireSingletonByFriend((CAsynchronousCloseMonitor**)&monitor);
    monitor->SignalBlockedThreads(fd);
    // try {
    return CLibcore::sOs->Close(fd);
    // } catch (ErrnoException errnoException) {
    //     // TODO: are there any cases in which we should throw?
    // }
}

ECode CIoBridge::_IsConnected(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [in] */ Int32 timeoutMs,
    /* [in] */ Int32 remainingTimeoutMs,
    /* [out] */ Boolean* isConnected)
{
    VALIDATE_NOT_NULL(isConnected);

    // ErrnoException cause;
    // try {
    AutoPtr<ArrayOf<IStructPollfd*> > pollFds = ArrayOf<IStructPollfd*>::Alloc(1);
    AutoPtr<IStructPollfd> pollFd;
    CStructPollfd::New((IStructPollfd**)&pollFd);
    pollFds->Set(0, pollFd);
    pollFd->SetFd(fd);
    pollFd->SetEvents((Int16)OsConstants::_POLLOUT);
    Int32 rc;
    FAIL_RETURN(CLibcore::sOs->Poll(pollFds, remainingTimeoutMs, &rc));
    if (rc == 0) {
        *isConnected = FALSE; // Timeout.
        return NOERROR;
    }
    Int32 connectError;
    FAIL_RETURN(CLibcore::sOs->GetsockoptInt32(fd,
            OsConstants::_SOL_SOCKET, OsConstants::_SO_ERROR, &connectError));
    if (connectError == 0) {
        *isConnected = TRUE; // Success!
        return NOERROR;
    }
    // throw new ErrnoException("isConnected", connectError); // The connect(2) failed.
    return E_ERRNO_EXCEPTION;
    // } catch (ErrnoException errnoException) {
    //     if (!fd.valid()) {
    //         throw new SocketException("Socket closed");
    //     }
    //     if (errnoException.errno == EINTR) {
    //         return false; // Punt and ask the caller to try again.
    //     } else {
    //         cause = errnoException;
    //     }
    // }
    // TODO: is it really helpful/necessary to throw so many different exceptions?
    // String detail = connectDetail(inetAddress, port, timeoutMs, cause);
    // if (cause.errno == ECONNRESET || cause.errno == ECONNREFUSED ||
    //         cause.errno == EADDRNOTAVAIL || cause.errno == EADDRINUSE ||
    //         cause.errno == ENETUNREACH) {
    //     throw new ConnectException(detail, cause);
    // } else if (cause.errno == EACCES) {
    //     throw new SecurityException(detail, cause);
    // } else if (cause.errno == ETIMEDOUT) {
    //     throw new SocketTimeoutException(detail, cause);
    // }
    // throw new SocketException(detail, cause);
}

ECode CIoBridge::_GetSocketOption(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 option,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);

    // try {
    return GetSocketOptionErrno(fd, option, value);
    // } catch (ErrnoException errnoException) {
    //     throw errnoException.rethrowAsSocketException();
    // }
}

ECode CIoBridge::GetSocketOptionErrno(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 option,
    /* [out] */ IInterface** value)
{
    switch (option) {
    case ISocketOptions::_IP_MULTICAST_IF:
    {
        // This is IPv4-only.
        AutoPtr<IInetAddress> iObj;
        CLibcore::sOs->GetsockoptInAddr(fd,
                OsConstants::_IPPROTO_IP, OsConstants::_IP_MULTICAST_IF, (IInetAddress**)&iObj);
        *value = iObj;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    case ISocketOptions::_IP_MULTICAST_IF2:
    {
        // This is IPv6-only.
        Int32 socketopt;
        CLibcore::sOs->GetsockoptInt32(fd,
                OsConstants::_IPPROTO_IPV6, OsConstants::_IPV6_MULTICAST_IF, &socketopt);
        AutoPtr<IInteger32> iObj;
        CInteger32::New(socketopt, (IInteger32**)&iObj);
        *value = iObj;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    case ISocketOptions::_IP_MULTICAST_LOOP:
    {
        // Since setting this from java.net always sets IPv4 and IPv6 to the same value,
        // it doesn't matter which we return.
        Int32 socketopt;
        CLibcore::sOs->GetsockoptInt32(fd,
                OsConstants::_IPPROTO_IPV6, OsConstants::_IPV6_MULTICAST_LOOP, &socketopt);
        Boolean b = BooleanFromInt(socketopt);
        AutoPtr<IBoolean> bObj;
        CBoolean::New(b, (IBoolean**)&bObj);
        *value = bObj;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    case JAVA_IP_MULTICAST_TTL:
    {
        // Since setting this from java.net always sets IPv4 and IPv6 to the same value,
        // it doesn't matter which we return.
        Int32 socketopt;
        CLibcore::sOs->GetsockoptInt32(fd,
                OsConstants::_IPPROTO_IPV6, OsConstants::_IPV6_MULTICAST_HOPS, &socketopt);
        AutoPtr<IInteger32> iObj;
        CInteger32::New(socketopt, (IInteger32**)&iObj);
        *value = iObj;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    case ISocketOptions::_IP_TOS:
    {
        // Since setting this from java.net always sets IPv4 and IPv6 to the same value,
        // it doesn't matter which we return.
        Int32 socketopt;
        CLibcore::sOs->GetsockoptInt32(fd,
                OsConstants::_IPPROTO_IPV6, OsConstants::_IPV6_TCLASS, &socketopt);
        AutoPtr<IInteger32> iObj;
        CInteger32::New(socketopt, (IInteger32**)&iObj);
        *value = iObj;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    case ISocketOptions::_SO_BROADCAST:
    {
        Int32 socketopt;
        CLibcore::sOs->GetsockoptInt32(fd,
                OsConstants::_SOL_SOCKET, OsConstants::_SO_BROADCAST, &socketopt);
        Boolean b = BooleanFromInt(socketopt);
        AutoPtr<IBoolean> bObj;
        CBoolean::New(b, (IBoolean**)&bObj);
        *value = bObj;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    case ISocketOptions::_SO_KEEPALIVE:
    {
        Int32 socketopt;
        CLibcore::sOs->GetsockoptInt32(fd,
                OsConstants::_SOL_SOCKET, OsConstants::_SO_KEEPALIVE, &socketopt);
        Boolean b = BooleanFromInt(socketopt);
        AutoPtr<IBoolean> bObj;
        CBoolean::New(b, (IBoolean**)&bObj);
        *value = bObj;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    case ISocketOptions::_SO_LINGER:
    {
        AutoPtr<IStructLinger> linger;
        CLibcore::sOs->GetsockoptLinger(fd,
                OsConstants::_SOL_SOCKET, OsConstants::_SO_LINGER, (IStructLinger**)&linger);
        Boolean isOn;
        if (linger->IsOn(&isOn), !isOn) {
            AutoPtr<IBoolean> bObj;
            CBoolean::New(FALSE, (IBoolean**)&bObj);
            *value = bObj;
            REFCOUNT_ADD(*value);
            return NOERROR;
        }
        Int32 l;
        linger->GetLinger(&l);
        AutoPtr<IInteger32> iObj;
        CInteger32::New(l, (IInteger32**)&iObj);
        *value = iObj;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    case ISocketOptions::_SO_OOBINLINE:
    {
        Int32 socketopt;
        CLibcore::sOs->GetsockoptInt32(fd,
                OsConstants::_SOL_SOCKET, OsConstants::_SO_OOBINLINE, &socketopt);
        Boolean b = BooleanFromInt(socketopt);
        AutoPtr<IBoolean> bObj;
        CBoolean::New(b, (IBoolean**)&bObj);
        *value = bObj;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    case ISocketOptions::_SO_RCVBUF:
    {
        Int32 socketopt;
        CLibcore::sOs->GetsockoptInt32(fd,
                OsConstants::_SOL_SOCKET, OsConstants::_SO_RCVBUF, &socketopt);
        AutoPtr<IInteger32> iObj;
        CInteger32::New(socketopt, (IInteger32**)&iObj);
        *value = iObj;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    case ISocketOptions::_SO_REUSEADDR:
    {
        Int32 socketopt;
        CLibcore::sOs->GetsockoptInt32(fd,
                OsConstants::_SOL_SOCKET, OsConstants::_SO_REUSEADDR, &socketopt);
        Boolean b = BooleanFromInt(socketopt);
        AutoPtr<IBoolean> bObj;
        CBoolean::New(b, (IBoolean**)&bObj);
        *value = bObj;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    case ISocketOptions::_SO_SNDBUF:
    {
        Int32 socketopt;
        CLibcore::sOs->GetsockoptInt32(fd,
                OsConstants::_SOL_SOCKET, OsConstants::_SO_SNDBUF, &socketopt);
        AutoPtr<IInteger32> iObj;
        CInteger32::New(socketopt, (IInteger32**)&iObj);
        *value = iObj;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    case ISocketOptions::_SO_TIMEOUT:
    {
        AutoPtr<IStructTimeval> tv;
        CLibcore::sOs->GetsockoptTimeval(fd,
                OsConstants::_SOL_SOCKET, OsConstants::_SO_RCVTIMEO, (IStructTimeval**)&tv);
        Int64 millis;
        tv->ToMillis(&millis);
        AutoPtr<IInteger32> iObj;
        CInteger32::New((Int32)millis, (IInteger32**)&iObj);
        *value = iObj;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    case ISocketOptions::_TCP_NODELAY:
    {
        Int32 socketopt;
        CLibcore::sOs->GetsockoptInt32(fd,
                OsConstants::_IPPROTO_TCP, OsConstants::_TCP_NODELAY, &socketopt);
        Boolean b = BooleanFromInt(socketopt);
        AutoPtr<IBoolean> bObj;
        CBoolean::New(b, (IBoolean**)&bObj);
        *value = bObj;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    default:
        // throw new SocketException("Unknown socket option: " + option);
        return E_SOCKET_EXCEPTION;
    }
}

Boolean CIoBridge::BooleanFromInt(
    /* [in] */ Int32 i)
{
    return (i != 0);
}

Int32 CIoBridge::BooleanToInt(
    /* [in] */ Boolean b)
{
    return b ? 1 : 0;
}

ECode CIoBridge::_SetSocketOption(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 option,
    /* [in] */ IInterface* value)
{
    // try {
    return SetSocketOptionErrno(fd, option, value);
    // } catch (ErrnoException errnoException) {
    //     throw errnoException.rethrowAsSocketException();
    // }
}

ECode CIoBridge::SetSocketOptionErrno(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 option,
    /* [in] */ IInterface* value)
{
    switch (option) {
    case ISocketOptions::_IP_MULTICAST_IF:
        // throw new UnsupportedOperationException("Use IP_MULTICAST_IF2 on Android");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    case ISocketOptions::_IP_MULTICAST_IF2:
    {
        // Although IPv6 was cleaned up to use int, IPv4 uses an ip_mreqn containing an int.
        assert(IInteger32::Probe(value) != NULL);
        Int32 i;
        IInteger32::Probe(value)->GetValue(&i);
        FAIL_RETURN(CLibcore::sOs->SetsockoptIpMreqn(fd,
                OsConstants::_IPPROTO_IP, OsConstants::_IP_MULTICAST_IF, i));
        FAIL_RETURN(CLibcore::sOs->SetsockoptInt32(fd,
                OsConstants::_IPPROTO_IPV6, OsConstants::_IPV6_MULTICAST_IF, i));
        return NOERROR;
    }
    case ISocketOptions::_IP_MULTICAST_LOOP:
    {
        // Although IPv6 was cleaned up to use int, IPv4 multicast loopback uses a byte.
        assert(IBoolean::Probe(value) != NULL);
        Boolean b = FALSE;
        IBoolean::Probe(value)->GetValue(&b);
        FAIL_RETURN(CLibcore::sOs->SetsockoptByte(fd,
                OsConstants::_IPPROTO_IP, OsConstants::_IP_MULTICAST_LOOP, BooleanToInt(b)));
        FAIL_RETURN(CLibcore::sOs->SetsockoptInt32(fd,
                OsConstants::_IPPROTO_IPV6, OsConstants::_IPV6_MULTICAST_LOOP, BooleanToInt(b)));
        return NOERROR;
    }
    case CIoBridge::JAVA_IP_MULTICAST_TTL:
    {
        // Although IPv6 was cleaned up to use int, and IPv4 non-multicast TTL uses int,
        // IPv4 multicast TTL uses a byte.
        assert(IInteger32::Probe(value) != NULL);
        Int32 i;
        IInteger32::Probe(value)->GetValue(&i);
        FAIL_RETURN(CLibcore::sOs->SetsockoptByte(fd,
                OsConstants::_IPPROTO_IP, OsConstants::_IP_MULTICAST_TTL, i));
        FAIL_RETURN(CLibcore::sOs->SetsockoptInt32(fd,
                OsConstants::_IPPROTO_IPV6, OsConstants::_IPV6_MULTICAST_HOPS, i));
        return NOERROR;
    }
    case ISocketOptions::_IP_TOS:
    {
        assert(IInteger32::Probe(value) != NULL);
        Int32 i;
        IInteger32::Probe(value)->GetValue(&i);
        FAIL_RETURN(CLibcore::sOs->SetsockoptInt32(fd,
                OsConstants::_IPPROTO_IP, OsConstants::_IP_TOS, i));
        FAIL_RETURN(CLibcore::sOs->SetsockoptInt32(fd,
                OsConstants::_IPPROTO_IPV6, OsConstants::_IPV6_TCLASS, i));
        return NOERROR;
    }
    case ISocketOptions::_SO_BROADCAST:
    {
        assert(IBoolean::Probe(value) != NULL);
        Boolean b = FALSE;
        IBoolean::Probe(value)->GetValue(&b);
        FAIL_RETURN(CLibcore::sOs->SetsockoptInt32(fd,
                OsConstants::_SOL_SOCKET, OsConstants::_SO_BROADCAST, BooleanToInt(b)));
        return NOERROR;
    }
    case ISocketOptions::_SO_KEEPALIVE:
    {
        assert(IBoolean::Probe(value) != NULL);
        Boolean b = FALSE;
        IBoolean::Probe(value)->GetValue(&b);
        FAIL_RETURN(CLibcore::sOs->SetsockoptInt32(fd,
                OsConstants::_SOL_SOCKET, OsConstants::_SO_KEEPALIVE, BooleanToInt(b)));
        return NOERROR;
    }
    case ISocketOptions::_SO_LINGER:
    {
        Boolean on = FALSE;
        Int32 seconds = 0;
        if (IInteger32::Probe(value) != NULL) {
            on = TRUE;
            Int32 i;
            IInteger32::Probe(value)->GetValue(&i);
            seconds = Elastos::Core::Math::Min(i, 65535);
        }
        AutoPtr<IStructLinger> linger;
        CStructLinger::New(BooleanToInt(on), seconds, (IStructLinger**)&linger);
        FAIL_RETURN(CLibcore::sOs->SetsockoptLinger(fd,
                OsConstants::_SOL_SOCKET, OsConstants::_SO_LINGER, linger));
        return NOERROR;
    }
    case ISocketOptions::_SO_OOBINLINE:
    {
        assert(IBoolean::Probe(value) != NULL);
        Boolean b = FALSE;
        IBoolean::Probe(value)->GetValue(&b);
        FAIL_RETURN(CLibcore::sOs->SetsockoptInt32(fd,
                OsConstants::_SOL_SOCKET, OsConstants::_SO_OOBINLINE, BooleanToInt(b)));
        return NOERROR;
    }
    case ISocketOptions::_SO_RCVBUF:
    {
        assert(IInteger32::Probe(value) != NULL);
        Int32 i;
        IInteger32::Probe(value)->GetValue(&i);
        FAIL_RETURN(CLibcore::sOs->SetsockoptInt32(fd,
                OsConstants::_SOL_SOCKET, OsConstants::_SO_RCVBUF, i));
        return NOERROR;
    }
    case ISocketOptions::_SO_REUSEADDR:
    {
        assert(IBoolean::Probe(value) != NULL);
        Boolean b = FALSE;
        IBoolean::Probe(value)->GetValue(&b);
        FAIL_RETURN(CLibcore::sOs->SetsockoptInt32(fd,
                OsConstants::_SOL_SOCKET, OsConstants::_SO_REUSEADDR, BooleanToInt(b)));
        return NOERROR;
    }
    case ISocketOptions::_SO_SNDBUF:
    {
        assert(IInteger32::Probe(value) != NULL);
        Int32 i;
        IInteger32::Probe(value)->GetValue(&i);
        FAIL_RETURN(CLibcore::sOs->SetsockoptInt32(fd,
                OsConstants::_SOL_SOCKET, OsConstants::_SO_SNDBUF, i));
        return NOERROR;
    }
    case ISocketOptions::_SO_TIMEOUT:
    {
        assert(IInteger32::Probe(value) != NULL);
        Int32 millis;
        IInteger32::Probe(value)->GetValue(&millis);
        AutoPtr<IStructTimeval> tv;
        CStructTimeval::FromMillis(millis, (IStructTimeval**)&tv);
        FAIL_RETURN(CLibcore::sOs->SetsockoptTimeval(fd,
                OsConstants::_SOL_SOCKET, OsConstants::_SO_RCVTIMEO, tv));
        return NOERROR;
    }
    case ISocketOptions::_TCP_NODELAY:
    {
        assert(IBoolean::Probe(value) != NULL);
        Boolean b = FALSE;
        IBoolean::Probe(value)->GetValue(&b);
        FAIL_RETURN(CLibcore::sOs->SetsockoptInt32(fd,
                OsConstants::_IPPROTO_TCP, OsConstants::_TCP_NODELAY, BooleanToInt(b)));
        return NOERROR;
    }
    case JAVA_MCAST_JOIN_GROUP:
    case JAVA_MCAST_LEAVE_GROUP:
    {
        assert(IStructGroupReq::Probe(value) != NULL);
        AutoPtr<IStructGroupReq> groupReq = IStructGroupReq::Probe(value);
        AutoPtr<IInetAddress> group;
        groupReq->GetGrGroup((IInetAddress**)&group);
        Int32 level = (IInet4Address::Probe(group) != NULL) ? OsConstants::_IPPROTO_IP : OsConstants::_IPPROTO_IPV6;
        Int32 op = (option == JAVA_MCAST_JOIN_GROUP) ? JAVA_MCAST_JOIN_GROUP : JAVA_MCAST_LEAVE_GROUP;
        FAIL_RETURN(CLibcore::sOs->SetsockoptGroupReq(fd, level, op, groupReq));
        return NOERROR;
    }
    case JAVA_MCAST_JOIN_SOURCE_GROUP:
    case JAVA_MCAST_LEAVE_SOURCE_GROUP:
    case JAVA_MCAST_BLOCK_SOURCE:
    case JAVA_MCAST_UNBLOCK_SOURCE:
    {
        assert(IStructGroupSourceReq::Probe(value) != NULL);
        AutoPtr<IStructGroupSourceReq> groupSourceReq = IStructGroupSourceReq::Probe(value);
        AutoPtr<IInetAddress> group;
        groupSourceReq->GetGsrGroup((IInetAddress**)&group);
        Int32 level = (IInet4Address::Probe(group) != NULL) ? OsConstants::_IPPROTO_IP : OsConstants::_IPPROTO_IPV6;
        Int32 op = GetGroupSourceReqOp(option);
        assert(op != -1);
        CLibcore::sOs->SetsockoptGroupSourceReq(fd, level, op, groupSourceReq);
        return NOERROR;
    }
    default:
        // throw new SocketException("Unknown socket option: " + option);
        return E_SOCKET_EXCEPTION;
    }
}

Int32 CIoBridge::GetGroupSourceReqOp(
    /* [in] */ Int32 value)
{
    switch (value) {
        case IIoBridge::JAVA_MCAST_JOIN_SOURCE_GROUP:
            return OsConstants::_MCAST_JOIN_SOURCE_GROUP;
        case IIoBridge::JAVA_MCAST_LEAVE_SOURCE_GROUP:
            return OsConstants::_MCAST_LEAVE_SOURCE_GROUP;
        case IIoBridge::JAVA_MCAST_BLOCK_SOURCE:
            return OsConstants::_MCAST_BLOCK_SOURCE;
        case IIoBridge::JAVA_MCAST_UNBLOCK_SOURCE:
            return OsConstants::_MCAST_UNBLOCK_SOURCE;
        default:
            return -1;
    }
}

ECode CIoBridge::_Open(
    /* [in] */ const String& path,
    /* [in] */ Int32 flags,
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd);

    *fd = NULL;
    AutoPtr<IStructStat> stat;
    // try {
    // On Android, we don't want default permissions to allow global access.
    Int32 mode = ((flags & OsConstants::_O_ACCMODE) == OsConstants::_O_RDONLY) ? 0 : 0600;
    FAIL_GOTO(CLibcore::sOs->Open(path, flags, mode, fd), FILE_NOT_FOUND);
    if (*fd != NULL) {
        // Posix open(2) fails with EISDIR only if you ask for write permission.
        // Java disallows reading directories too.
        FAIL_RETURN(CLibcore::sOs->Fstat(*fd, (IStructStat**)&stat));
        stat->GetMode(&mode);
        if (OsConstants::S_IsDIR(mode)) {
            // throw new ErrnoException("open", EISDIR);
            goto FILE_NOT_FOUND;
        }
    }
    return NOERROR;

FILE_NOT_FOUND:
    if (*fd != NULL) {
        AutoPtr<CIoUtils> utils;
        CIoUtils::AcquireSingletonByFriend((CIoUtils**)&utils);
        utils->Close(*fd); //TODO::
    }
    return E_FILE_NOT_FOUND_EXCEPTION;
}

ECode CIoBridge::_Read(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(bytes);
    VALIDATE_NOT_NULL(count);

    if ((byteOffset | byteCount) < 0 || byteOffset > bytes->GetLength() ||
        bytes->GetLength() - byteOffset < byteCount) {
        // throw new ArrayIndexOutOfBoundsException(arrayLength, offset, count);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (byteCount == 0) {
        *count = 0;
        return NOERROR;
    }
    // try {
    FAIL_RETURN(CLibcore::sOs->Read(fd, bytes, byteOffset, byteCount, count));
    if (*count == 0) {
        *count = -1;
        return NOERROR;
    }
    return NOERROR;
    // } catch (ErrnoException errnoException) {
    //     if (errnoException.errno == EAGAIN) {
    //         // We return 0 rather than throw if we try to read from an empty non-blocking pipe.
    //         return 0;
    //     }
    //     throw errnoException.rethrowAsIOException();
    // }
}

ECode CIoBridge::_Write(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount)
{
    if ((byteOffset | byteCount) < 0 || byteOffset > bytes->GetLength() ||
        bytes->GetLength() - byteOffset < byteCount) {
        // throw new ArrayIndexOutOfBoundsException(arrayLength, offset, count);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (byteCount == 0) {
        return NOERROR;
    }
    // try {
    while (byteCount > 0) {
        Int32 bytesWritten;
        FAIL_RETURN(CLibcore::sOs->Write(fd, bytes, byteOffset, byteCount, &bytesWritten));
        byteCount -= bytesWritten;
        byteOffset += bytesWritten;
    }
    return NOERROR;
    // } catch (ErrnoException errnoException) {
    //     throw errnoException.rethrowAsIOException();
    // }
}

ECode CIoBridge::_Sendto(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int32 flags,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean isDatagram = (inetAddress != NULL);
    if (!isDatagram && byteCount <= 0) {
        *result = 0;
        return NOERROR;
    }
    // try {
    return CLibcore::sOs->Sendto(fd, bytes, byteOffset, byteCount, flags, inetAddress, port, result);
    // } catch (ErrnoException errnoException) {
    //     result = maybeThrowAfterSendto(isDatagram, errnoException);
    // }
}

ECode CIoBridge::_Sendto(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 flags,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isDatagram = (inetAddress != NULL);
    Int32 remainvalue = 0;
    AutoPtr<IBuffer> buf = IBuffer::Probe(buffer);
    if (buf == NULL) {
        *result = 0;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    buf->GetRemaining(&remainvalue);
    if (!isDatagram && remainvalue == 0) {
        *result = 0;
        return NOERROR;
    }
    // try {
    return CLibcore::sOs->Sendto(fd, buffer, flags, inetAddress, port, result);
    // } catch (ErrnoException errnoException) {
    //     result = maybeThrowAfterSendto(isDatagram, errnoException);
    // }
}

ECode CIoBridge::_Recvfrom(
    /* [in] */ Boolean isRead,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int32 flags,
    /* [in] */ IDatagramPacket* packet,
    /* [in] */ Boolean isConnected,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    // try {
    AutoPtr<IInetSocketAddress> srcAddress;
    if (packet != NULL && !isConnected) {
        CInetSocketAddress::New((IInetSocketAddress**)&srcAddress);
    }
    FAIL_RETURN(CLibcore::sOs->Recvfrom(fd, bytes, byteOffset, byteCount, flags, srcAddress, result));
    *result = PostRecvfrom(isRead, packet, isConnected, srcAddress, *result);
    return NOERROR;
    // } catch (ErrnoException errnoException) {
    //     result = maybeThrowAfterRecvfrom(isRead, isConnected, errnoException);
    // }
    // return result;
}

ECode CIoBridge::_Recvfrom(
    /* [in] */ Boolean isRead,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 flags,
    /* [in] */ IDatagramPacket* packet,
    /* [in] */ Boolean isConnected,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    // try {
    AutoPtr<IInetSocketAddress> srcAddress;
    if (packet != NULL && !isConnected) {
        CInetSocketAddress::New((IInetSocketAddress**)&srcAddress); //TODO::
    }

    FAIL_RETURN(CLibcore::sOs->Recvfrom(fd, buffer, flags, srcAddress, result));
    *result = PostRecvfrom(isRead, packet, isConnected, srcAddress, *result);
    return NOERROR;
    // } catch (ErrnoException errnoException) {
    //     result = maybeThrowAfterRecvfrom(isRead, isConnected, errnoException);
    // }
}


Int32 CIoBridge::PostRecvfrom(
    /* [in] */ Boolean isRead,
    /* [in] */ IDatagramPacket* packet,
    /* [in] */ Boolean isConnected,
    /* [in] */ IInetSocketAddress* srcAddress,
    /* [in] */ Int32 byteCount)
{
    if (isRead && byteCount == 0) {
        return -1;
    }
    if (packet != NULL) {
        packet->SetLength(byteCount);
        if (!isConnected) {
            AutoPtr<IInetAddress> addr;
            srcAddress->GetAddress((IInetAddress**)&addr);
            Int32 port;
            srcAddress->GetPort(&port);
            packet->SetAddress(addr);
            packet->SetPort(port);
        }
    }
    return byteCount;
}

ECode CIoBridge::_Socket(
    /* [in] */ Boolean stream,
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd);

    // try {
    FAIL_RETURN(CLibcore::sOs->Socket(OsConstants::_AF_INET6, stream ?
            OsConstants::_SOCK_STREAM : OsConstants::_SOCK_DGRAM, 0, fd));

    // The RFC (http://www.ietf.org/rfc/rfc3493.txt) says that IPV6_MULTICAST_HOPS defaults
    // to 1. The Linux kernel (at least up to 2.6.38) accidentally defaults to 64 (which
    // would be correct for the *unicast* hop limit).
    // See http://www.spinics.net/lists/netdev/msg129022.html, though no patch appears to
    // have been applied as a result of that discussion. If that bug is ever fixed, we can
    // remove this code. Until then, we manually set the hop limit on IPv6 datagram sockets.
    // (IPv4 is already correct.)
    if (!stream) {
        CLibcore::sOs->SetsockoptInt32(*fd,
                OsConstants::_IPPROTO_IPV6, OsConstants::_IPV6_MULTICAST_HOPS, 1);
    }

    return NOERROR;
    // } catch (ErrnoException errnoException) {
    //     throw errnoException.rethrowAsSocketException();
    // }
}

ECode CIoBridge::_GetSocketLocalAddress(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IInetAddress** address)
{
    VALIDATE_NOT_NULL(address);

    // try {
    AutoPtr<ISocketAddress> sa;
    FAIL_RETURN(CLibcore::sOs->Getsockname(fd, (ISocketAddress**)&sa));
    assert(IInetSocketAddress::Probe(sa) != NULL);
    return IInetSocketAddress::Probe(sa)->GetAddress(address);
    // } catch (ErrnoException errnoException) {
    //     throw new AssertionError(errnoException);
    // }
}

ECode CIoBridge::_GetSocketLocalPort(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);

    // try {
    AutoPtr<ISocketAddress> sa;
    FAIL_RETURN(CLibcore::sOs->Getsockname(fd, (ISocketAddress**)&sa));
    assert(IInetSocketAddress::Probe(sa) != NULL);
    return IInetSocketAddress::Probe(sa)->GetPort(port);
    // } catch (ErrnoException errnoException) {
    //     throw new AssertionError(errnoException);
    // }
}

ECode CIoBridge::Available(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Int32* avail)
{
    return _Available(fd, avail);
}

ECode CIoBridge::Bind(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port)
{
    return _Bind(fd, inetAddress, port);
}

ECode CIoBridge::Connect(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port)
{
//    try {
    return Connect(fd, inetAddress, port, 0);
//    } catch (SocketTimeoutException ex) {
//        throw new AssertionError(ex); // Can't happen for a connect without a timeout.
//    }
}

ECode CIoBridge::Connect(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [in] */ Int32 timeoutMs)
{
    return _Connect(fd, inetAddress, port, timeoutMs);
}

ECode CIoBridge::CloseAndSignalBlockedThreads(
    /* [in] */ IFileDescriptor* fd)
{
    return _CloseAndSignalBlockedThreads(fd);
}

ECode CIoBridge::IsConnected(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [in] */ Int32 timeoutMs,
    /* [in] */ Int32 remainingTimeoutMs,
    /* [out] */ Boolean* isConnected)
{
    return _IsConnected(fd, inetAddress, port, timeoutMs, remainingTimeoutMs, isConnected);
}

ECode CIoBridge::GetSocketOption(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 option,
    /* [out] */ IInterface** value)
{
    return _GetSocketOption(fd, option, value);
}

ECode CIoBridge::SetSocketOption(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 option,
    /* [in] */ IInterface* value)
{
    return _SetSocketOption(fd, option, value);
}

ECode CIoBridge::Open(
    /* [in] */ const String& path,
    /* [in] */ Int32 flags,
    /* [out] */ IFileDescriptor** fd)
{
    return _Open(path, flags, fd);
}

ECode CIoBridge::Read(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* count)
{
    return _Read(fd, bytes, byteOffset, byteCount, count);
}

ECode CIoBridge::Write(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount)
{
    return _Write(fd, bytes, byteOffset, byteCount);
}

ECode CIoBridge::Sendto(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int32 flags,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [out] */ Int32* result)
{
    return _Sendto(fd, bytes, byteOffset, byteCount, flags, inetAddress, port, result);
}

ECode CIoBridge::Sendto(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Int32 flags,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Int32 port,
        /* [out] */ Int32* result)
{
    return _Sendto(fd, buffer, flags, inetAddress, port, result);
}

ECode CIoBridge::Recvfrom(
    /* [in] */ Boolean isRead,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int32 flags,
    /* [in] */ IDatagramPacket* packet,
    /* [in] */ Boolean isConnected,
    /* [out] */ Int32* result)
{
    return _Recvfrom(isRead, fd, bytes, byteOffset, byteCount, flags, packet, isConnected, result);
}

ECode CIoBridge::Recvfrom(
    /* [in] */ Boolean isRead,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 flags,
    /* [in] */ IDatagramPacket* packet,
    /* [in] */ Boolean isConnected,
    /* [out] */ Int32* result)
{
    return _Recvfrom(isRead, fd, buffer, flags, packet, isConnected, result);
}

ECode CIoBridge::Socket(
    /* [in] */ Boolean stream,
    /* [out] */ IFileDescriptor** fd)
{
    return _Socket(stream, fd);
}

ECode CIoBridge::GetSocketLocalAddress(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IInetAddress** address)
{
    return _GetSocketLocalAddress(fd, address);
}

ECode CIoBridge::GetSocketLocalPort(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Int32* port)
{
    return _GetSocketLocalPort(fd, port);
}

} // namespace IO
} // namespace Libcore
