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

#include <Elastos.CoreLibrary.Net.h>
#include "elastos/droid/net/LocalSocketImpl.h"
#include "elastos/droid/net/CCredentials.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/system/Os.h"
#include "elastos/droid/system/OsConstants.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>
#include <cutils/sockets.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

using Elastos::Droid::Os::Process;
using Elastos::Droid::System::Os;
using Elastos::Droid::System::OsConstants;

using Elastos::Core::AutoLock;
using Elastos::Core::CInteger32;
using Elastos::Core::IBoolean;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::Thread;
using Elastos::IO::CFileDescriptor;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Net::ISocket;
using Elastos::Net::ISocketOptions;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {

//====================================================================================
//              LocalSocketImpl::SocketInputStream
//====================================================================================
LocalSocketImpl::SocketInputStream::SocketInputStream(
    /* [in] */ LocalSocketImpl* host)
    : mHost(host)
{}

ECode LocalSocketImpl::SocketInputStream::Available(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IFileDescriptor> myFd = mHost->mFd;
    if (myFd == NULL) {
        Logger::E("LocalSocketImpl", "socket closed");
        return E_IO_EXCEPTION;
    }
    return mHost->NativeAvailable(myFd, result);
}

ECode LocalSocketImpl::SocketInputStream::Close()
{
    return mHost->Close();
}

ECode LocalSocketImpl::SocketInputStream::Read(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    {    AutoLock syncLock(mHost->mReadMonitor);
        AutoPtr<IFileDescriptor> myFd = mHost->mFd;
        if (myFd == NULL) {
            Logger::E("LocalSocketImpl", "socket closed");
            return E_IO_EXCEPTION;
        }
        return mHost->NativeRead(myFd, result);
    }
    return NOERROR;
}

ECode LocalSocketImpl::SocketInputStream::Read(
    /* [in] */ ArrayOf<Byte>* b,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    assert(b != NULL);
    return Read(b, 0, b->GetLength(), result);
}

ECode LocalSocketImpl::SocketInputStream::Read(
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    {    AutoLock syncLock(mHost->mReadMonitor);
        AutoPtr<IFileDescriptor> myFd = mHost->mFd;
        if (myFd == NULL) {
            Logger::E("LocalSocketImpl", "socket closed");
            return E_IO_EXCEPTION;
        }

        if (off < 0 || len < 0 || (off + len) > b->GetLength()) {
            return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }
        return mHost->NativeReadba(b, off, len, myFd, result);
    }
    return NOERROR;
}

//====================================================================================
//              LocalSocketImpl::SocketOutputStream
//====================================================================================
LocalSocketImpl::SocketOutputStream::SocketOutputStream(
    /* [in] */ LocalSocketImpl* host)
    : mHost(host)
{}

ECode LocalSocketImpl::SocketOutputStream::Close()
{
    return mHost->Close();
}

ECode LocalSocketImpl::SocketOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* b)
{
    return Write(b, 0, b->GetLength());
}

ECode LocalSocketImpl::SocketOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    {    AutoLock syncLock(mHost->mWriteMonitor);
        AutoPtr<IFileDescriptor> myFd = mHost->mFd;
        if (myFd == NULL) {
            Logger::E("LocalSocketImpl", "socket closed");
            return E_IO_EXCEPTION;
        }

        if (off < 0 || len < 0 || (off + len) > b->GetLength()) {
            return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }
        return mHost->NativeWriteba(b, off, len, myFd);
    }
    return NOERROR;
}

ECode LocalSocketImpl::SocketOutputStream::Write(
    /* [in] */ Int32 b)
{
    {    AutoLock syncLock(mHost->mWriteMonitor);
        AutoPtr<IFileDescriptor> myFd = mHost->mFd;
        if (myFd == NULL) {
            Logger::E("LocalSocketImpl", "socket closed");
            return E_IO_EXCEPTION;
        }
        return mHost->NativeWrite(b, myFd);
    }
    return NOERROR;
}

ECode LocalSocketImpl::SocketOutputStream::Flush()
{
    AutoPtr<IFileDescriptor> myFd = mHost->mFd;
    if (myFd == NULL) {
        Logger::E("LocalSocketImpl", "socket closed");
        return E_IO_EXCEPTION;
    }
    Int32 pendingCount;
    while(mHost->NativePending(myFd, &pendingCount), pendingCount > 0) {
        // try {
        ECode ec = Thread::Sleep(10);
        // } catch (InterruptedException ie) {
        if (FAILED(ec)) {
            if (ec == (ECode)E_INTERRUPTED_EXCEPTION) return NOERROR;
            return ec;
        }
        // }
    }
    return NOERROR;
}

//====================================================================================
// LocalSocketImpl
//====================================================================================
CAR_INTERFACE_IMPL(LocalSocketImpl, Object, ILocalSocketImpl)

static Boolean droid_opt_to_real(Int32 optID, int* opt, int* level)
{
    switch (optID)
    {
        case 4098:
            *opt = SO_RCVBUF;
            *level = SOL_SOCKET;
            return TRUE;
        case 4097:
            *opt = SO_SNDBUF;
            *level = SOL_SOCKET;
            return TRUE;
        case 4102:
            *opt = SO_SNDTIMEO;
            *level = SOL_SOCKET;
            return TRUE;
        case 128:
            *opt = SO_LINGER;
            *level = SOL_SOCKET;
            return TRUE;
        case 1:
            *opt = TCP_NODELAY;
            *level = IPPROTO_TCP;
            return TRUE;
        case 4:
            *opt = SO_REUSEADDR;
            *level = SOL_SOCKET;
            return TRUE;

    }
    return FALSE;
}

/**
 * Processes ancillary data, handling only
 * SCM_RIGHTS. Creates appropriate objects and sets appropriate
 * fields in the LocalSocketImpl object. Returns 0 on success
 * or -1 if an exception was thrown.
 */
static int socket_process_cmsg(
    /* [in] */ LocalSocketImpl* thisObj,
    /* [in] */ struct msghdr * pMsg)
{
    struct cmsghdr *cmsgptr;

    for (cmsgptr = CMSG_FIRSTHDR(pMsg);
            cmsgptr != NULL; cmsgptr = CMSG_NXTHDR(pMsg, cmsgptr)) {

        if (cmsgptr->cmsg_level != SOL_SOCKET) {
            continue;
        }

        if (cmsgptr->cmsg_type == SCM_RIGHTS) {
            Int32 *pDescriptors = (Int32 *)CMSG_DATA(cmsgptr);
            Int32 count
                = ((cmsgptr->cmsg_len - CMSG_LEN(0)) / sizeof(int));

            if (count < 0) {
//                jniThrowException(env, "java/io/IOException", "invalid cmsg length");
                return -1;
            }

            AutoPtr< ArrayOf<IFileDescriptor*> > fdArray = ArrayOf<IFileDescriptor*>::Alloc(count);

            if (fdArray == NULL) {
                return -1;
            }

            for (Int32 i = 0; i < count; i++) {
                AutoPtr<IFileDescriptor> fdObject;
                CFileDescriptor::New((IFileDescriptor**)&fdObject);
                fdObject->SetDescriptor(pDescriptors[i]);

                fdArray->Set(i, fdObject);
            }

            thisObj->mInboundFileDescriptors = fdArray;
        }
    }

    return 0;
}

/**
 * Reads data from a socket into buf, processing any ancillary data
 * and adding it to thisJ.
 *
 * Returns the length of normal data read, or -1 if an exception has
 * been thrown in this function.
 */
static ssize_t socket_read_all(
    /* [in] */ LocalSocketImpl* thisObj,
    /* [in] */ Int32 fd,
    /* [in] */ void *buffer,
    /* [in] */ size_t len)
{
    ssize_t ret;
    struct msghdr msg;
    struct iovec iv;
    unsigned char *buf = (unsigned char *)buffer;
    // Enough buffer for a pile of fd's. We throw an exception if
    // this buffer is too small.
    struct cmsghdr cmsgbuf[2*sizeof(cmsghdr) + 0x100];

    memset(&msg, 0, sizeof(msg));
    memset(&iv, 0, sizeof(iv));

    iv.iov_base = buf;
    iv.iov_len = len;

    msg.msg_iov = &iv;
    msg.msg_iovlen = 1;
    msg.msg_control = cmsgbuf;
    msg.msg_controllen = sizeof(cmsgbuf);

    do {
        ret = recvmsg(fd, &msg, MSG_NOSIGNAL);
    } while (ret < 0 && errno == EINTR);

    if (ret < 0 && errno == EPIPE) {
        // Treat this as an end of stream
        return 0;
    }

    if (ret < 0) {
//        jniThrowIOException(env, errno);
        return -1;
    }

    if ((msg.msg_flags & (MSG_CTRUNC | MSG_OOB | MSG_ERRQUEUE)) != 0) {
        // To us, any of the above flags are a fatal error

//        jniThrowException(env, "java/io/IOException",
//                "Unexpected error or truncation during recvmsg()");
        return -1;
    }

    if (ret >= 0) {
        socket_process_cmsg(thisObj, &msg);
    }

    return ret;
}

/**
 * Writes all the data in the specified buffer to the specified socket.
 *
 * Returns 0 on success or -1 if an exception was thrown.
 */
static Int32 socket_write_all(
    /* [in] */ LocalSocketImpl* thisObj,
    /* [in] */ Int32 fd,
    /* [in] */ void* buf,
    /* [in] */ size_t len)
{
    ssize_t ret;
    struct msghdr msg;
    unsigned char *buffer = (unsigned char *)buf;
    memset(&msg, 0, sizeof(msg));

    AutoPtr< ArrayOf<IFileDescriptor*> > outboundFds =
            thisObj->mOutboundFileDescriptors;

    struct cmsghdr *cmsg;
    Int32 countFds = outboundFds == NULL ? 0 : outboundFds->GetLength();
    Int32 fds[countFds];
    char msgbuf[CMSG_SPACE(countFds)];

    // Add any pending outbound file descriptors to the message
    if (outboundFds != NULL) {
        for (Int32 i = 0; i < countFds; i++) {
            AutoPtr<IFileDescriptor> fdObject = (*outboundFds)[i];

            fdObject->GetDescriptor(&fds[i]);
        }

        // See "man cmsg" really
        msg.msg_control = msgbuf;
        msg.msg_controllen = sizeof msgbuf;
        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        cmsg->cmsg_len = CMSG_LEN(sizeof fds);
        memcpy(CMSG_DATA(cmsg), fds, sizeof fds);
    }

    // We only write our msg_control during the first write
    while (len > 0) {
        struct iovec iv;
        memset(&iv, 0, sizeof(iv));

        iv.iov_base = buffer;
        iv.iov_len = len;

        msg.msg_iov = &iv;
        msg.msg_iovlen = 1;

        do {
            ret = sendmsg(fd, &msg, MSG_NOSIGNAL);
        } while (ret < 0 && errno == EINTR);

        if (ret < 0) {
            // jniThrowIOException(env, errno);
            return -1;
        }

        buffer += ret;
        len -= ret;

        // Wipes out any msg_control too
        memset(&msg, 0, sizeof(msg));
    }

    return 0;
}

LocalSocketImpl::LocalSocketImpl()
    : mFdCreatedInternally(FALSE)
    , mFdCreatedExternally(FALSE)
{}

LocalSocketImpl::~LocalSocketImpl()
{
    Close();
}

ECode LocalSocketImpl::NativePending(
    /* [in] */ IFileDescriptor* fileDescriptor,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;

    int fd;

    if (fileDescriptor == NULL) {
        return NOERROR;
    }
    fileDescriptor->GetDescriptor(&fd);

    int pending;
    int ret = ioctl(fd, TIOCOUTQ, &pending);

    // If this were a non-socket fd, there would be other cases to worry
    // about...

    //ALOGD("socket_pending, ioctl ret:%d, pending:%d", ret, pending);
    if (ret < 0) {
        *result = 0;
        return E_IO_EXCEPTION;
    }

    *result = pending;
    return NOERROR;
}

ECode LocalSocketImpl::NativeAvailable(
    /* [in] */ IFileDescriptor* fileDescriptor,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 fd;

    fileDescriptor->GetDescriptor(&fd);

    int avail;
    int ret = ioctl(fd, FIONREAD, &avail);

    // If this were a non-socket fd, there would be other cases to worry
    // about...
    if (ret < 0) {
        //jniThrowIOException(env, errno);
        *result = 0;
        return E_IO_EXCEPTION;
    }

    *result = avail;
    return NOERROR;
}

ECode LocalSocketImpl::NativeRead(
    /* [in] */ IFileDescriptor* fileDescriptor,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Int32 fd;
    Int32 err;

    if (fileDescriptor == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    fileDescriptor->GetDescriptor(&fd);

    unsigned char buf;

    err = socket_read_all(this, fd, &buf, 1);

    if (err < 0) {
        *result = 0;
        return E_IO_EXCEPTION;
    }

    if (err == 0) {
        // end of file
        *result = -1;
        return NOERROR;
    }

    *result = (Int32)buf;
    return NOERROR;
}

ECode LocalSocketImpl::NativeReadba(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len,
    /* [in] */ IFileDescriptor* fileDescriptor,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Int32 fd;
    Byte* byteBuffer;
    int ret;

    if (fileDescriptor == NULL || buffer == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    if (off < 0 || len < 0 || (off + len) > buffer->GetLength()) {
        *result = -1;
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (len == 0) {
        // because socket_read_all returns 0 on EOF
        *result = 0;
        return NOERROR;
    }

    fileDescriptor->GetDescriptor(&fd);

    byteBuffer = buffer->GetPayload();

    ret = socket_read_all(this, fd, byteBuffer + off, len);

    *result = ((ret == 0) ? -1 : ret);
    return NOERROR;
}

ECode LocalSocketImpl::NativeWriteba(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len,
    /* [in] */ IFileDescriptor* fileDescriptor)
{
    Int32 fd;
    int err;
    Byte* byteBuffer;

    if (fileDescriptor == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    if (off < 0 || len < 0 || (off + len) > buffer->GetLength()) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    fileDescriptor->GetDescriptor(&fd);

    byteBuffer = buffer->GetPayload();

    err = socket_write_all(this, fd,
            byteBuffer + off, len);

    // A return of -1 above means an exception is pending
    if (err < 0) return E_IO_EXCEPTION;
    return NOERROR;
}

ECode LocalSocketImpl::NativeWrite(
    /* [in] */ Int32 b,
    /* [in] */ IFileDescriptor* fileDescriptor)
{
    Int32 fd;
    int err;

    if (fileDescriptor == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    fileDescriptor->GetDescriptor(&fd);

    err = socket_write_all(this, fd, &b, 1);

    // A return of -1 above means an exception is pending
    if (err < 0) return E_IO_EXCEPTION;
    return NOERROR;
}

ECode LocalSocketImpl::NativeAccept(
    /* [in] */ IFileDescriptor* fdObj,
    /* [in] */ ILocalSocketImpl* s,
    /* [out] */ IFileDescriptor** resultFd)
{
    VALIDATE_NOT_NULL(resultFd)
    *resultFd = NULL;

    union {
        struct sockaddr address;
        struct sockaddr_un un_address;
    } sa;

    int ret;
    int retFD;
    int fd;
    socklen_t addrlen;

    if (s == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    FAIL_RETURN(fdObj->GetDescriptor(&fd))

    do {
        addrlen = sizeof(sa);
        ret = accept(fd, &(sa.address), &addrlen);
    } while (ret < 0 && errno == EINTR);

    if (ret < 0) {
        Logger::E("LocalSocketImpl", "failed to Accept %s, ret:%d", TO_CSTR(fdObj), ret);
        return E_IO_EXCEPTION;
    }

    retFD = ret;

    CFileDescriptor::New(resultFd);
    (*resultFd)->SetDescriptor(retFD);
    return NOERROR;
}

ECode LocalSocketImpl::ConnectLocal(
    /* [in] */ IFileDescriptor* fileDescriptor,
    /* [in] */ const String& name,
    /* [in] */ Int32 namespaceId)
{
    int ret;
    Int32 fd;

    fileDescriptor->GetDescriptor(&fd);

    const char * nameUtf8 = name.string();

    ret = socket_local_client_connect(
                fd,
                nameUtf8,
                namespaceId,
                SOCK_STREAM);

    if (ret < 0) {
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode LocalSocketImpl::BindLocal(
    /* [in] */ IFileDescriptor* fileDescriptor,
    /* [in] */ const String& name,
    /* [in] */ Int32 namespaceId)
{
    int ret;
    Int32 fd;

    if (name.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    fileDescriptor->GetDescriptor(&fd);

    const char* nameUtf8 = name.string();

    ret = socket_local_server_bind(fd, nameUtf8, namespaceId);

    if (ret < 0) {
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode LocalSocketImpl::NativeListen(
    /* [in] */ IFileDescriptor* fileDescriptor,
    /* [in] */ Int32 backlog)
{
    int ret;
    Int32 fd;

    fileDescriptor->GetDescriptor(&fd);

    ret = listen(fd, backlog);

    if (ret < 0) {
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode LocalSocketImpl::Shutdown(
    /* [in] */ IFileDescriptor* fileDescriptor,
    /* [in] */ Boolean shutdownInput)
{
    int ret;
    Int32 fd;

    fileDescriptor->GetDescriptor(&fd);

    ret = shutdown(fd, shutdownInput ? SHUT_RD : SHUT_WR);

    if (ret < 0) {
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode LocalSocketImpl::NativeGetPeerCredentials(
    /* [in] */ IFileDescriptor* fileDescriptor,
    /* [out] */ ICredentials** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    int err;
    Int32 fd;

    if (fileDescriptor == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    fileDescriptor->GetDescriptor(&fd);

    struct ucred creds;

    memset(&creds, 0, sizeof(creds));
    socklen_t szCreds = sizeof(creds);

    err = getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &creds, &szCreds);

    if (err < 0) {
        *result = NULL;
        return E_IO_EXCEPTION;
    }

    if (szCreds == 0) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<ICredentials>  crden;
    CCredentials::New(creds.pid, creds.uid, creds.gid, (ICredentials**)&crden);
    *result = crden;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode LocalSocketImpl::NativeGetOption(
    /* [in] */ IFileDescriptor* fileDescriptor,
    /* [in] */ Int32 optID,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Int32 ret, value;
    int opt, level;
    Int32 fd;

    socklen_t size = sizeof(int);

    if (!droid_opt_to_real(optID, &opt, &level)) {
        *result = 0;
        return E_IO_EXCEPTION;
    }

    fileDescriptor->GetDescriptor(&fd);

    switch (opt)
    {
        case SO_LINGER:
        {
            struct linger lingr;
            size = sizeof(lingr);
            ret = getsockopt(fd, level, opt, &lingr, &size);
            if (!lingr.l_onoff) {
                value = -1;
            } else {
                value = lingr.l_linger;
            }
            break;
        }
        default:
            ret = getsockopt(fd, level, opt, &value, &size);
            break;
    }


    if (ret != 0) {
        *result = 0;
        return E_IO_EXCEPTION;
    }

    *result = value;
    return NOERROR;
}

ECode LocalSocketImpl::NativeSetOption(
    /* [in] */ IFileDescriptor* fileDescriptor,
    /* [in] */ Int32 optID,
    /* [in] */ Int32 boolValue,
    /* [in] */ Int32 intValue)
{
    Int32 ret;
    Int32 optname;
    Int32 level;
    Int32 fd;

    if (!droid_opt_to_real(optID, &optname, &level)) {
        return E_IO_EXCEPTION;
    }

    fileDescriptor->GetDescriptor(&fd);

    switch (optname) {
        case SO_LINGER: {
            /*
             * SO_LINGER is special because it needs to use a special
             * "linger" struct as well as use the incoming boolean
             * argument specially.
             */
            struct linger lingr;
            lingr.l_onoff = boolValue ? 1 : 0; // Force it to be 0 or 1.
            lingr.l_linger = intValue;
            ret = setsockopt(fd, level, optname, &lingr, sizeof(lingr));
            break;
        }
        case SO_SNDTIMEO: {
            /*
             * SO_TIMEOUT from the core library gets converted to
             * SO_SNDTIMEO, but the option is supposed to set both
             * send and receive timeouts. Note: The incoming timeout
             * value is in milliseconds.
             */
            struct timeval timeout;
            timeout.tv_sec = intValue / 1000;
            timeout.tv_usec = (intValue % 1000) * 1000;

            ret = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO,
                    (void *)&timeout, sizeof(timeout));

            if (ret == 0) {
                ret = setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO,
                        (void *)&timeout, sizeof(timeout));
            }

            break;
        }
        default: {
            /*
             * In all other cases, the translated option level and
             * optname may be used directly for a call to setsockopt().
             */
            ret = setsockopt(fd, level, optname, &intValue, sizeof(intValue));
            break;
        }
    }

    if (ret != 0) {
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode LocalSocketImpl::Accept(
    /* [in] */ IFileDescriptor* fileDescriptor,
    /* [in] */ ILocalSocketImpl* s,
    /* [out] */ IFileDescriptor** result)
{
    return NativeAccept(fileDescriptor, s, result);
}

ECode LocalSocketImpl::constructor()
{
    return NOERROR;
}

ECode LocalSocketImpl::constructor(
    /* [in] */ IFileDescriptor* fd)
{
    mFd = fd;
    assert(mFd != NULL);
    return NOERROR;
}

ECode LocalSocketImpl::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder sb("LocalSocketImpl: fd:");
    sb += Object::ToString(mFd);
    *result = sb.ToString();
    return NOERROR;
}

ECode LocalSocketImpl::Create(
    /* [in] */ Int32 sockType)
{
    // no error if socket already created
    // need this for LocalServerSocket.accept()
    if (mFd == NULL) {
        Int32 osType;
        switch (sockType) {
            case ILocalSocket::SOCKET_DGRAM:
                osType = OsConstants::_SOCK_DGRAM;
                break;
            case ILocalSocket::SOCKET_STREAM:
                osType = OsConstants::_SOCK_STREAM;
                break;
            case ILocalSocket::SOCKET_SEQPACKET:
                osType = OsConstants::_SOCK_SEQPACKET;
                break;
            default:
                Logger::E("LocalSocketImpl", "unknown sockType %d");
                return E_ILLEGAL_STATE_EXCEPTION;
        }
        // try {
        ECode ec = Elastos::Droid::System::Os::Socket(OsConstants::_AF_UNIX, osType, 0, (IFileDescriptor**)&mFd);
        if (FAILED(ec)) {
            if (ec == (ECode)E_ERRNO_EXCEPTION)
                return E_IO_EXCEPTION;
            return ec;
        }
        mFdCreatedInternally = TRUE;
        // } catch (ErrnoException e) {
        // }
    }
    return NOERROR;
}

ECode LocalSocketImpl::Close()
{
    {    AutoLock syncLock(this);
        if ((mFd == NULL) || ((mFdCreatedInternally == FALSE) && (mFdCreatedExternally == FALSE))) {
            mFd = NULL;
            return NOERROR;
        }
        // try {
        ECode ec = Elastos::Droid::System::Os::Close(mFd);
        // } catch (ErrnoException e) {
        if (FAILED(ec)) {
            if (ec == (ECode)E_ERRNO_EXCEPTION) return E_IO_EXCEPTION;
            return ec;
        }
        // }
        mFd = NULL;
    }
    return NOERROR;
}

ECode LocalSocketImpl::Connect(
    /* [in] */ ILocalSocketAddress* address,
    /* [in] */ Int32 timeout)
{
    if (mFd == NULL) {
        Logger::E("LocalSocketImpl", "socket not created");
        return E_IO_EXCEPTION;
    }

    String name;
    address->GetName(&name);
    LocalSocketAddressNamespace ns;
    address->GetNamespace(&ns);
    return ConnectLocal(mFd, name, ns);
}

ECode LocalSocketImpl::Bind(
    /* [in] */ ILocalSocketAddress* endpoint)
{
    if (mFd == NULL) {
        Logger::E("LocalSocketImpl", "socket not created");
        return E_IO_EXCEPTION;
    }

    String name;
    endpoint->GetName(&name);
    LocalSocketAddressNamespace ns;
    endpoint->GetNamespace(&ns);
    return BindLocal(mFd, name, ns);
}

ECode LocalSocketImpl::Listen(
    /* [in] */ Int32 backlog)
{
    if (mFd == NULL) {
        Logger::E("LocalSocketImpl", "socket not created");
        return E_IO_EXCEPTION;
    }

    return NativeListen(mFd, backlog);
}

ECode LocalSocketImpl::Accept(
    /* [in] */ ILocalSocketImpl* s)
{
    if (mFd == NULL) {
        Logger::E("LocalSocketImpl", "socket not created");
        return E_IO_EXCEPTION;
    }

    AutoPtr<IFileDescriptor> fd;
    LocalSocketImpl* lsi = (LocalSocketImpl*)s;
    FAIL_RETURN(NativeAccept(mFd, s, (IFileDescriptor**)&fd))
    lsi->mFd = fd;
    lsi->mFdCreatedInternally = TRUE;
    return NOERROR;
}

ECode LocalSocketImpl::GetInputStream(
    /* [out] */ IInputStream** is)
{
    VALIDATE_NOT_NULL(is);
    *is = NULL;

    if (mFd == NULL) {
        Logger::E("LocalSocketImpl", "socket not created");
        return E_IO_EXCEPTION;
    }

    {    AutoLock syncLock(this);
        if (mFis == NULL) {
            mFis = new SocketInputStream(this);
        }

        *is = IInputStream::Probe(mFis);
        REFCOUNT_ADD(*is)
    }
    return NOERROR;
}

ECode LocalSocketImpl::GetOutputStream(
    /* [out] */ IOutputStream** os)
{
    VALIDATE_NOT_NULL(os);
    *os = NULL;

    if (mFd == NULL) {
        Logger::E("LocalSocketImpl", "socket not created");
        return E_IO_EXCEPTION;
    }

    {    AutoLock syncLock(this);
        if (mFos == NULL) {
            mFos = new SocketOutputStream(this);
        }

        *os = IOutputStream::Probe(mFos);
        REFCOUNT_ADD(*os)
    }
    return NOERROR;
}

ECode LocalSocketImpl::CloseExternalFd()
{
    if (mFd == NULL) {
        Logger::E("LocalSocketImpl", "socket not created");
        return E_IO_EXCEPTION;
    }

    mFdCreatedExternally = TRUE;
    return NOERROR;
}

ECode LocalSocketImpl::Available(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IFileDescriptor> myFd = mFd;
    if (myFd == NULL) {
        Logger::E("LocalSocketImpl", "socket closed");
        return E_IO_EXCEPTION;
    }

    return NativeAvailable(myFd, result);
}

ECode LocalSocketImpl::ShutdownInput()
{
    if (mFd == NULL) {
        Logger::E("LocalSocketImpl", "socket not created");
        return E_IO_EXCEPTION;
    }

    return Shutdown(mFd, TRUE);
}

ECode LocalSocketImpl::ShutdownOutput()
{
    if (mFd == NULL) {
        Logger::E("LocalSocketImpl", "socket not created");
        return E_IO_EXCEPTION;
    }

    return Shutdown(mFd, FALSE);
}

ECode LocalSocketImpl::GetFileDescriptor(
    /* [out] */ IFileDescriptor** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mFd;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode LocalSocketImpl::SupportsUrgentData(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode LocalSocketImpl::SendUrgentData(
    /* [in] */ Int32 data)
{
    Logger::E("LocalSocketImpl", "not impled");
    return E_RUNTIME_EXCEPTION;
}

ECode LocalSocketImpl::GetOption(
    /* [in] */ Int32 optID,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (mFd == NULL) {
        Logger::E("LocalSocketImpl", "socket not created");
        return E_IO_EXCEPTION;
    }

    if (optID == ISocketOptions::_SO_TIMEOUT) {
        AutoPtr<IInteger32> i32;
        CInteger32::New(0, (IInteger32**)&i32);
        *result = i32.Get();
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    Int32 value;
    FAIL_RETURN(NativeGetOption(mFd, optID, &value));
    AutoPtr<IInteger32> i32;
    switch (optID) {
        case ISocketOptions::_SO_RCVBUF:
        case ISocketOptions::_SO_SNDBUF: {
            CInteger32::New(value, (IInteger32**)&i32);
            *result = i32.Get();
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
        case ISocketOptions::_SO_REUSEADDR:
        default: {
            CInteger32::New(value, (IInteger32**)&i32);
            *result = i32.Get();
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode LocalSocketImpl::SetOption(
    /* [in] */ Int32 optID,
    /* [in] */ IInterface* value)
{
    /*
     * Boolean.FALSE is used to disable some options, so it
     * is important to distinguish between FALSE and unset.
     * We define it here that -1 is unset, 0 is FALSE, and 1
     * is TRUE.
     */
    Int32 boolValue = -1;
    Int32 intValue = 0;

    if (mFd == NULL) {
        Logger::E("LocalSocketImpl", "socket not created");
        return E_IO_EXCEPTION;
    }

    if (IInteger32::Probe(value) != NULL) {
        IInteger32::Probe(value)->GetValue(&intValue);
    }
    else if (IBoolean::Probe(value) != NULL) {
        Boolean bv;
        IBoolean::Probe(value)->GetValue(&bv);
        boolValue = bv? 1 : 0;
    }
    else {
        Logger::E("LocalSocketImpl", "bad value: %s", TO_CSTR(value));
        return E_IO_EXCEPTION;
    }

    return NativeSetOption(mFd, optID, boolValue, intValue);
}

ECode LocalSocketImpl::SetFileDescriptorsForSend(
    /* [in] */ ArrayOf<IFileDescriptor*>* fds)
{
    {    AutoLock syncLock(mWriteMonitor);

        mOutboundFileDescriptors = fds;
    }
    return NOERROR;
}

ECode LocalSocketImpl::GetAncillaryFileDescriptors(
    /* [out, callee] */ ArrayOf<IFileDescriptor*>** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<IFileDescriptor*> > rev;
    {    AutoLock syncLock(mReadMonitor);
        rev = mInboundFileDescriptors;

        mInboundFileDescriptors = NULL;
    }
    *result = rev;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode LocalSocketImpl::GetPeerCredentials(
    /* [out] */ ICredentials** result)
{
    return NativeGetPeerCredentials(mFd, result);
}

ECode LocalSocketImpl::GetSockAddress(
    /* [out] */ ILocalSocketAddress** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
    //TODO implement this
    //return getSockName_native(fd);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
