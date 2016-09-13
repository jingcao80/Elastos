#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <pwd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#ifndef __APPLE__
#include <sys/prctl.h>
#endif
#include <sys/socket.h>
#include <sys/stat.h>
#ifdef __APPLE__
#include <sys/statvfs.h>
#endif
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <cutils/log.h>

#include <stdio.h>
#include <string.h>
#include <sys/un.h>
#include <dns/resolv_netid.h>
#include "CPosix.h"
#include "AsynchronousCloseMonitor.h"
#include "io/CFileDescriptor.h"
#include "NetworkUtilities.h"
#include "droid/system/CStructPasswd.h"
#include "droid/system/CStructStat.h"
#include "droid/system/CStructLinger.h"
#include "droid/system/CStructTimeval.h"
#include "droid/system/CStructUcred.h"
#include "droid/system/CStructStatVfs.h"
#include "droid/system/CStructUtsname.h"
#include "Portability.h"
#include "core/UniquePtr.h"
#include "ErrnoRestorer.h"
#include "LocalArray.h"
#include <elastos/utility/etl/Vector.h>
#include "net/CInetSocketAddress.h"

using Elastos::Core::UniquePtr;
using Elastos::IO::IBuffer;
using Elastos::IO::CFileDescriptor;
using Elastos::Net::CInetSocketAddress;
using Elastos::Utility::Etl::Vector;
using Elastos::Droid::System::CStructPasswd;
using Elastos::Droid::System::CStructStat;
using Elastos::Droid::System::CStructLinger;
using Elastos::Droid::System::CStructTimeval;
using Elastos::Droid::System::CStructUcred;
using Elastos::Droid::System::CStructStatVfs;
using Elastos::Droid::System::CStructUtsname;

#define NET_FAILURE_RETRY(return_type, return_value, syscall_name, fd, ...) ({ \
    return_type _rc = -1; \
    return_value = NOERROR; \
    do { \
        Boolean _wasSignaled; \
        Int32 _syscallErrno; \
        { \
            Int32 _fd; \
            fd->GetDescriptor(&_fd); \
            AsynchronousCloseMonitor _monitor(_fd); \
            _rc = syscall_name(_fd, __VA_ARGS__); \
            _syscallErrno = errno; \
            _wasSignaled = _monitor.WasSignaled(); \
        } \
        if (_wasSignaled) { \
            ALOGE("Libcore::IO::Posix: Socket closed"); \
            return_value = E_SOCKET_EXCEPTION; \
            _rc = -1; \
            break; \
        } \
        if (_rc == -1 && _syscallErrno != EINTR) { \
            /* TODO: with a format string we could show the arguments too, like strace(1). */ \
            ALOGE("Libcore::IO::Posix: " # syscall_name "error"); \
            return_value = E_SOCKET_EXCEPTION; \
            break; \
        } \
    } while (_rc == -1); /* _syscallErrno == EINTR && !_wasSignaled */ \
    _rc; })

#define IO_FAILURE_RETRY(return_type, return_value, syscall_name, fd, ...) ({ \
    return_type _rc = -1; \
    return_value = NOERROR; \
    Int32 _fd; \
    fd->GetDescriptor(&_fd); \
    if (_fd == -1) { \
        return_value = E_IO_EXCEPTION; \
        ALOGE("Libcore::IO::Posix: File descriptor closed"); \
    } else { \
        do { \
            Boolean _wasSignaled; \
            Int32 _syscallErrno; \
            { \
                AsynchronousCloseMonitor _monitor(_fd); \
                _rc = syscall_name(_fd, __VA_ARGS__); \
                _syscallErrno = errno; \
                _wasSignaled = _monitor.WasSignaled(); \
            } \
            if (_wasSignaled) { \
                ALOGE("Libcore::IO::Posix: " # syscall_name "interrupted"); \
                return_value = E_IO_EXCEPTION; \
                _rc = -1; \
                break; \
            } \
            if (_rc == -1 && _syscallErrno != EINTR) { \
                /* TODO: with a format string we could show the arguments too, like strace(1). */ \
                ALOGE("Libcore::IO::Posix: " # syscall_name "error"); \
                return_value = E_IO_EXCEPTION; \
                break; \
            } \
        } while (_rc == -1); /* && _syscallErrno == EINTR && !_wasSignaled */ \
    } \
    _rc; })

struct addrinfo_deleter {
    void operator()(addrinfo* p) const {
        if (p != NULL) { // bionic's freeaddrinfo(3) crashes when passed NULL.
            freeaddrinfo(p);
        }
    }
};

template <typename rc_t>
static rc_t ErrorIfMinusOne(const char* name, rc_t rc, ECode* ec, const char* param = NULL) {
    *ec = NOERROR;
    if (rc == rc_t(-1)) {
        if (param != NULL)
            ALOGE("CPosix: System-call error: %s %s, param: %s", name, strerror(errno), param);
        else
            ALOGE("CPosix: System-call error: %s %s", name, strerror(errno));
        *ec = E_ERRNO_EXCEPTION;
    }
    return rc;
}

extern char** environ; // Standard, but not in any header file.

namespace Libcore {
namespace IO {

static AutoPtr<ISocketAddress> MakeSocketAddress(
    /* [in] */ const sockaddr_storage& ss)
{
    Int32 port;
    AutoPtr<IInetAddress> inetAddress = SockaddrToInetAddress(ss, &port);
    if (inetAddress == NULL) {
        return NULL;
    }
    AutoPtr<ISocketAddress> rst;
    CInetSocketAddress::New(inetAddress, port, (ISocketAddress**)&rst);
    return rst;
}

static AutoPtr<IStructPasswd> MakeStructPasswd(
    /* [in] */ const struct passwd& pw)
{
    String pw_name = String(pw.pw_name);
    String pw_dir = String(pw.pw_dir);
    String pw_shell = String(pw.pw_shell);

    AutoPtr<IStructPasswd> rst;
    CStructPasswd::New(
        pw_name,
        static_cast<Int32>(pw.pw_uid),
        static_cast<Int32>(pw.pw_gid),
        pw_dir,
        pw_shell,
        (IStructPasswd**)&rst);
    return rst;
}

static AutoPtr<IStructStat> MakeStructStat(
    /* [in] */ const struct stat& sb)
{
    AutoPtr<IStructStat> rst;
    CStructStat::New(
        static_cast<Int64>(sb.st_dev), static_cast<Int64>(sb.st_ino),
        static_cast<Int32>(sb.st_mode), static_cast<Int64>(sb.st_nlink),
        static_cast<Int32>(sb.st_uid), static_cast<Int32>(sb.st_gid),
        static_cast<Int64>(sb.st_rdev), static_cast<Int64>(sb.st_size),
        static_cast<Int64>(sb.st_atime), static_cast<Int64>(sb.st_mtime),
        static_cast<Int64>(sb.st_ctime), static_cast<Int64>(sb.st_blksize),
        static_cast<Int64>(sb.st_blocks), (IStructStat**)&rst);
    return rst;
}

static AutoPtr<IStructStatVfs> MakeStructStatVfs(
    /* [in] */ const struct ::statvfs& sb)
{
#if defined(__APPLE__)
    // Mac OS has no f_namelen field in struct statfs.
    Int64 max_name_length = 255; // __DARWIN_MAXNAMLEN
#else
    Int64 max_name_length = static_cast<Int64>(sb.f_namemax);
#endif
    AutoPtr<IStructStatVfs> rst;
    CStructStatVfs::New(
        static_cast<Int32>(sb.f_bsize),
        static_cast<Int32>(sb.f_frsize),
        static_cast<Int32>(sb.f_blocks),
        static_cast<Int32>(sb.f_bfree),
        static_cast<Int32>(sb.f_bavail),
        static_cast<Int32>(sb.f_files),
        static_cast<Int32>(sb.f_ffree),
        static_cast<Int32>(sb.f_favail),
        static_cast<Int32>(sb.f_fsid),
        static_cast<Int32>(sb.f_flag),
        max_name_length, (IStructStatVfs**)&rst);
    return rst;
}

static AutoPtr<IStructLinger> MakeStructLinger(
    /* [in] */ const struct linger& l)
{
    AutoPtr<IStructLinger> rst;
    CStructLinger::New(l.l_onoff, l.l_linger, (IStructLinger**)&rst);
    return rst;
}

static AutoPtr<IStructTimeval> MakeStructTimeval(
    /* [in] */ const struct timeval& tv)
{
    AutoPtr<IStructTimeval> rst;
    CStructTimeval::New(static_cast<Int32>(tv.tv_sec), static_cast<Int32>(tv.tv_usec), (IStructTimeval**)&rst);
    return rst;
}

static AutoPtr<IStructUcred> MakeStructUcred(
    /* [in] */ const struct ucred& u __unused)
{
#ifdef __APPLE__
    ALOGE("unimplemented support for ucred on a Mac");
    return NULL;
#else
    AutoPtr<IStructUcred> rst;
    CStructUcred::New(u.pid, u.uid, u.gid, (IStructUcred**)&rst);
    return rst;
#endif
}

static AutoPtr<IStructUtsname> MakeStructUtsname(
    /* [in] */ const struct utsname& buf)
{
    String sysname = String(buf.sysname);
    String nodename = String(buf.nodename);
    String release = String(buf.release);
    String version = String(buf.version);
    String machine = String(buf.machine);

    AutoPtr<IStructUtsname> rst;
    CStructUtsname::New(sysname, nodename, release, version, machine, (IStructUtsname**)&rst);
    return rst;
};


static AutoPtr<ISocketAddress> DoGetSockName(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Boolean is_sockname)
{
  Int32 _fd;
  fd->GetDescriptor(&_fd);
  sockaddr_storage ss;
  sockaddr* sa = reinterpret_cast<sockaddr*>(&ss);
  socklen_t byteCount = sizeof(ss);
  memset(&ss, 0, byteCount);
  Int32 rc = is_sockname ? TEMP_FAILURE_RETRY(getsockname(_fd, sa, &byteCount))
      : TEMP_FAILURE_RETRY(getpeername(_fd, sa, &byteCount));
  if (rc == -1) {
    // throwErrnoException(env, is_sockname ? "getsockname" : "getpeername");
    if (is_sockname) {
        ALOGE("System-call getsockname error, errno = %d",errno);
    } else {
        ALOGE("System-call getpeername error, errno = %d",errno);
    }
    return NULL;
  }
  return MakeSocketAddress(ss);
}

static Boolean FillIfreq(
    /* [in] */ const String& interfaceName,
    /* [in] */ struct ifreq& req) {
    if (interfaceName == NULL) {
        return FALSE;
    }
    memset(&req, 0, sizeof(req));
    strncpy(req.ifr_name, interfaceName, sizeof(req.ifr_name));
    req.ifr_name[sizeof(req.ifr_name) - 1] = '\0';
    return TRUE;
}

static Boolean FillInetSocketAddress(
    /* [in] */ Int32 rc,
    /* [in] */ IInetSocketAddress* socketAddress,
    /* [in] */ const sockaddr_storage& ss) {
    if (rc == -1 || socketAddress == NULL) {
        return TRUE;
    }
    // Fill out the passed-in InetSocketAddress with the sender's IP address and port number.
    Int32 port;
    AutoPtr<IInetAddress> sender = SockaddrToInetAddress(ss, &port);
    if (sender == NULL) {
        return FALSE;
    }

    socketAddress->SetAddress(sender);
    socketAddress->SetPort(port);
    return TRUE;
}

static ECode DoStat(
    /* [in] */ const String& path,
    /* [in] */ Boolean isLstat,
    /* [out] */ IStructStat** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (path == NULL) {
        return NOERROR;
    }
    struct stat sb;
    Int32 rc = isLstat ? TEMP_FAILURE_RETRY(lstat(path, &sb))
                     : TEMP_FAILURE_RETRY(stat(path, &sb));
    if (rc == -1) {
        // throwErrnoException(env, isLstat ? "lstat" : "stat");
        // if (isLstat) {
        //     ALOGE("System-call lstat error, errno = %d", errno);
        // } else {
        //     ALOGE("System-call stat error, errno = %d", errno);
        // }
        return E_ERRNO_EXCEPTION;
    }

    AutoPtr<IStructStat> ss = MakeStructStat(sb);
    *result = ss;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

class Passwd
{
public:
    Passwd() : mResult(NULL) {
        mBufferSize = sysconf(_SC_GETPW_R_SIZE_MAX);
        mBuffer.reset(new char[mBufferSize]);
    }

    AutoPtr<IStructPasswd> getpwnam(const char* name) {
        return process("getpwnam_r", getpwnam_r(name, &mPwd, mBuffer.get(), mBufferSize, &mResult));
    }

    AutoPtr<IStructPasswd> getpwuid(uid_t uid) {
        return process("getpwuid_r", getpwuid_r(uid, &mPwd, mBuffer.get(), mBufferSize, &mResult));
    }

    struct passwd* get() {
        return mResult;
    }

private:
    AutoPtr<IStructPasswd> process(const char* syscall, int error) {
        if (mResult == NULL) {
            errno = error;
            // throwErrnoException(mEnv, syscall);
            ALOGE("System-call %s error, errno = %d", syscall, errno);
            return NULL;
        }
        return MakeStructPasswd(*mResult);
    }

    UniquePtr<char[]> mBuffer;
    size_t mBufferSize;
    struct passwd mPwd;
    struct passwd* mResult;
};

bool readlink(const char* path, String& result) {
    // // We can't know how big a buffer readlink(2) will need, so we need to
    // // loop until it says "that fit".
    size_t bufSize = 512;
    while (true) {
        LocalArray<512> buf(bufSize);
        ssize_t len = ::readlink(path, &buf[0], buf.size());
        if (len == -1) {
            // An error occurred.
            return false;
        }
        if (static_cast<size_t>(len) < buf.size()) {
            // The buffer was big enough.
            result = String(&buf[0], len);
            return true;
        }
        // Try again with a bigger buffer.
        bufSize *= 2;
    }
}

CAR_OBJECT_IMPL(CPosix)

CAR_INTERFACE_IMPL(CPosix, Object, IOs)

ECode CPosix::Accept(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetSocketAddress* peerAddress,
    /* [out] */ IFileDescriptor** retFd)
{
    VALIDATE_NOT_NULL(retFd)
    *retFd = NULL;
    sockaddr_storage ss;
    socklen_t sl = sizeof(ss);
    memset(&ss, 0, sizeof(ss));
    sockaddr* peer = (peerAddress != NULL) ? reinterpret_cast<sockaddr*>(&ss) : NULL;
    socklen_t* peerLength = (peerAddress != NULL) ? &sl : 0;
    ECode ec;
    Int32 clientFd = NET_FAILURE_RETRY(Int32, ec, accept, fd, peer, peerLength);
    if (clientFd == -1 || !FillInetSocketAddress(clientFd, peerAddress, ss)) {
        close(clientFd);
    } else if (clientFd != -1) {
        CFileDescriptor::New(retFd);
        (*retFd)->SetDescriptor(clientFd);
    }
    return ec;
}

ECode CPosix::Access(
    /* [in] */ const String& path,
    /* [in] */ Int32 mode,
    /* [out] */ Boolean* succeed)
{
    VALIDATE_NOT_NULL(succeed)
    *succeed = FALSE;
    if (path == NULL) {
        return NOERROR;
    }
    Int32 rc = TEMP_FAILURE_RETRY(access(path, mode));
    if (rc == -1) {
        // ALOGE("System-call : access Error, errno = %d", errno);
        return E_ERRNO_EXCEPTION;
    }
    *succeed = (rc == 0);
    return NOERROR;
}

ECode CPosix::Elastos_getaddrinfo(
    /* [in] */ const String& node,
    /* [in] */ IStructAddrinfo* _hints,
    /* [in] */ Int32 netId,
    /* [out, callee] */ ArrayOf<IInetAddress*>** info)
{
    VALIDATE_NOT_NULL(info)
    *info = NULL;
    if (node == NULL) {
        return NOERROR;
    }

    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    Int32 valueTmp;
    _hints->GetFlags(&valueTmp);
    hints.ai_flags = valueTmp;
    _hints->GetFamily(&valueTmp);
    hints.ai_family = valueTmp;
    _hints->GetSocktype(&valueTmp);
    hints.ai_socktype = valueTmp;
    _hints->GetProtocol(&valueTmp);
    hints.ai_protocol = valueTmp;

    addrinfo* addressList = NULL;
    errno = 0;
    Int32 rc = android_getaddrinfofornet(node, NULL, &hints, netId, 0, &addressList);
    UniquePtr<addrinfo, addrinfo_deleter> addressListDeleter(addressList);
    if (rc != 0) {
        // throwGaiException(env, "android_getaddrinfo", rc);
        ALOGE("GaiException: Elastos_getaddrinfo");
        return E_GAI_EXCEPTION;
    }

    // Count results so we know how to size the output array.
    Int32 addressCount = 0;
    for (addrinfo* ai = addressList; ai != NULL; ai = ai->ai_next) {
        if (ai->ai_family == AF_INET || ai->ai_family == AF_INET6) {
            ++addressCount;
        } else {
            ALOGE("android_getaddrinfo unexpected ai_family %i", ai->ai_family);
        }
    }
    if (addressCount == 0) {
        return NOERROR;
    }

    // Prepare output array.
    AutoPtr<ArrayOf<IInetAddress*> > result = ArrayOf<IInetAddress*>::Alloc(addressCount);

    // Examine returned addresses one by one, save them in the output array.
    int index = 0;
    for (addrinfo* ai = addressList; ai != NULL; ai = ai->ai_next) {
        if (ai->ai_family != AF_INET && ai->ai_family != AF_INET6) {
            // Unknown address family. Skip this address.
            ALOGE("android_getaddrinfo unexpected ai_family %i", ai->ai_family);
            continue;
        }

        // Convert each IP address into a Java byte array.
        sockaddr_storage& address = *reinterpret_cast<sockaddr_storage*>(ai->ai_addr);
        AutoPtr<IInetAddress> inetAddress = SockaddrToInetAddress(address, NULL);
        if (inetAddress == NULL) {
            return NOERROR;
        }
        result->Set(index, inetAddress);
        ++index;
    }
    *info = result;
    REFCOUNT_ADD(*info)
    return NOERROR;
}

ECode CPosix::Bind(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port)
{
    sockaddr_storage ss;
    socklen_t sa_len;
    if (!InetAddressToSockaddr(address, port, ss, sa_len)) {
        return NOERROR;
    }
    const sockaddr* sa = reinterpret_cast<const sockaddr*>(&ss);
    // We don't need the return value because we'll already have thrown.
    ECode ec;
    (void) NET_FAILURE_RETRY(int, ec, bind, fd, sa, sa_len);
    return ec;
}

ECode CPosix::Chmod(
    /* [in] */ const String& path,
    /* [in] */ Int32 mode)
{
    if (path == NULL) {
        return NOERROR;
    }
    ECode ec;
    ErrorIfMinusOne("chmod", TEMP_FAILURE_RETRY(chmod(path, mode)), &ec);
    return ec;
}

ECode CPosix::Chown(
    /* [in] */ const String& path,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid)
{
    if (path == NULL) {
        return NOERROR;
    }
    ECode ec;
    ErrorIfMinusOne("chown", TEMP_FAILURE_RETRY(chown(path, uid, gid)), &ec);
    return ec;
}

ECode CPosix::Close(
    /* [in] */ IFileDescriptor* fd)
{
    // Get the FileDescriptor's 'fd' field and clear it.
    // We need to do this before we can throw an IOException (http://b/3222087).
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    fd->SetDescriptor(-1);

    // Even if close(2) fails with EINTR, the fd will have been closed.
    // Using TEMP_FAILURE_RETRY will either lead to EBADF or closing someone else's fd.
    // http://lkml.indiana.edu/hypermail/linux/kernel/0509.1/0877.html
    ECode ec;
    ErrorIfMinusOne("close", close(_fd), &ec);
    return ec;
}

ECode CPosix::Connect(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port)
{
    sockaddr_storage ss;
    socklen_t sa_len;
    if (!InetAddressToSockaddr(address, port, ss, sa_len)) {
        return NOERROR;
    }
    const sockaddr* sa = reinterpret_cast<const sockaddr*>(&ss);
    // We don't need the return value because we'll already have thrown.
    ECode ec;
    (void) NET_FAILURE_RETRY(Int32, ec, connect, fd, sa, sa_len);
    return ec;
}

ECode CPosix::Dup(
    /* [in] */ IFileDescriptor* oldFd,
    /* [out] */ IFileDescriptor** retFd)
{
    VALIDATE_NOT_NULL(retFd)
    *retFd = NULL;

    Int32 _oldFd;
    oldFd->GetDescriptor(&_oldFd);
    ECode ec;
    Int32 newFd = ErrorIfMinusOne("dup", TEMP_FAILURE_RETRY(dup(_oldFd)), &ec);
    FAIL_RETURN(ec)
    if (newFd != -1) {
        CFileDescriptor::New(retFd);
        (*retFd)->SetDescriptor(newFd);
    }
    return ec;
}

ECode CPosix::Dup2(
    /* [in] */ IFileDescriptor* oldFd,
    /* [in] */ Int32 newFd,
    /* [out] */ IFileDescriptor** retFd)
{
    VALIDATE_NOT_NULL(retFd)
    *retFd = NULL;

    Int32 _oldFd;
    oldFd->GetDescriptor(&_oldFd);
    ECode ec;
    Int32 fd = ErrorIfMinusOne("dup2", TEMP_FAILURE_RETRY(dup2(_oldFd, newFd)), &ec);
    FAIL_RETURN(ec)
    if (fd != -1) {
        CFileDescriptor::New(retFd);
        (*retFd)->SetDescriptor(fd);
    }
    return ec;
}

ECode CPosix::Environ(
    /* [out, callee] */ ArrayOf<String>** env)
{
    VALIDATE_NOT_NULL(env)
    *env = NULL;

    Int32 size = 0;
    while (environ[size]) size++;
    AutoPtr<ArrayOf<String> > envTmp = ArrayOf<String>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        String str = String(environ[i]);
        envTmp->Set(i, str);
    }
    *env = envTmp;
    REFCOUNT_ADD(*env);
    return NOERROR;
}

ECode CPosix::Execv(
    /* [in] */ const String& filename,
    /* [in] */ ArrayOf<String>* argv)
{
    if (filename == NULL) {
        return NOERROR;
    }

    Int32 length = argv->GetLength();
    char const ** array_ = new char const*[length + 1];
    array_[length] = NULL;
    for (Int32 i = 0; i < length; i++) {
        array_[i] = (*argv)[i];
    }
    execv(filename, (char* const*)array_);

    ALOGE("execv returned errno = %d", errno);
    return E_ERRNO_EXCEPTION;
    // throwErrnoException(env, "execv");
}

ECode CPosix::Execve(
    /* [in] */ const String& filename,
    /* [in] */ ArrayOf<String>* argv,
    /* [in] */ ArrayOf<String>* envp)
{
    if (filename == NULL) {
        return NOERROR;
    }

    Int32 argvLength = argv->GetLength();
    char** argvArray = new char*[argvLength + 1];
    argvArray[argvLength] = NULL;
    for (Int32 i = 0; i < argvLength; i++) {
        argvArray[i] = (char*)(*argv)[i].string();
    }

    Int32 envpLength = envp->GetLength();
    char** envpArray = new char*[envpLength + 1];
    envpArray[envpLength] = NULL;
    for (Int32 i = 0; i < envpLength; i++) {
        envpArray[i] = (char*)(*envp)[i].string();
    }

    execve(filename, argvArray, envpArray);

    delete[] argvArray;
    delete[] envpArray;

    ALOGE("execve returned errno = %d", errno);
    return E_ERRNO_EXCEPTION;
}

ECode CPosix::Fchmod(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 mode)
{
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    ECode ec;
    ErrorIfMinusOne("fchmod", TEMP_FAILURE_RETRY(fchmod(_fd, mode)), &ec);
    return ec;
}

ECode CPosix::Fchown(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid)
{
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    ECode ec;
    ErrorIfMinusOne("fchown", TEMP_FAILURE_RETRY(fchown(_fd, uid, gid)), &ec);
    return ec;
}

ECode CPosix::FcntlVoid(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 cmd,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    ECode ec;
    ErrorIfMinusOne("fcntl", TEMP_FAILURE_RETRY(fcntl(_fd, cmd)), &ec);
    return ec;
}

ECode CPosix::FcntlInt64(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 cmd,
    /* [in] */ Int64 arg,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    ECode ec;
    ErrorIfMinusOne("fcntl", TEMP_FAILURE_RETRY(fcntl(_fd, cmd, arg)), &ec);
    return ec;
}

ECode CPosix::FcntlFlock(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 cmd,
    /* [in] */ IStructFlock* flock,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    struct flock64 lock;
    memset(&lock, 0, sizeof(lock));
    Int16 typeTmp, whenceTmp;
    Int64 startTmp, lenTmp;
    Int32 pidTmp;
    flock->GetType(&typeTmp);
    flock->GetWhence(&whenceTmp);
    flock->GetStart(&startTmp);
    flock->GetLen(&lenTmp);
    flock->GetPid(&pidTmp);
    lock.l_type = typeTmp;
    lock.l_whence = whenceTmp;
    lock.l_start = startTmp;
    lock.l_len = lenTmp;
    lock.l_pid = pidTmp;

    ECode ec;
    Int32 rc = IO_FAILURE_RETRY(Int32, ec, fcntl, fd, cmd, &lock);
    if (rc != -1) {
        flock->SetType(lock.l_type);
        flock->SetWhence(lock.l_whence);
        flock->SetStart(lock.l_start);
        flock->SetLen(lock.l_len);
        flock->SetPid(lock.l_pid);
    }
    *result = rc;
    return ec;
}

ECode CPosix::Fdatasync(
    /* [in] */ IFileDescriptor* fd)
{
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    ECode ec;
    ErrorIfMinusOne("fdatasync", TEMP_FAILURE_RETRY(fdatasync(_fd)), &ec);
    return ec;
}

ECode CPosix::Fstat(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IStructStat** statout)
{
    VALIDATE_NOT_NULL(statout)
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    struct stat sb;
    Int32 rc = TEMP_FAILURE_RETRY(fstat(_fd, &sb));
    if (rc == -1) {
        // throwErrnoException(env, "fstat");
        ALOGE("System-call fstat error: errno = %d", errno);
        *statout = NULL;
        return E_ERRNO_EXCEPTION;
    }
    AutoPtr<IStructStat> statObj = MakeStructStat(sb);
    *statout = statObj;
    REFCOUNT_ADD(*statout)
    return NOERROR;
}

ECode CPosix::Fstatvfs(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IStructStatVfs** statFs)
{
    VALIDATE_NOT_NULL(statFs)
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    struct statvfs sb;
    Int32 rc = TEMP_FAILURE_RETRY(fstatvfs(_fd, &sb));
    if (rc == -1) {
        // throwErrnoException(env, "fstatvfs");
        ALOGE("System-call fstatvfs error: errno = %d", errno);
        *statFs = NULL;
        return E_ERRNO_EXCEPTION;
    }
    AutoPtr<IStructStatVfs> statObj = MakeStructStatVfs(sb);
    *statFs = statObj;
    REFCOUNT_ADD(*statFs)
    return NOERROR;
}

ECode CPosix::Fsync(
    /* [in] */ IFileDescriptor* fd)
{
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    ECode ec;
    ErrorIfMinusOne("fsync", TEMP_FAILURE_RETRY(fsync(_fd)), &ec);
    return ec;
}

ECode CPosix::Ftruncate(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 length)
{
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    ECode ec;
    ErrorIfMinusOne("ftruncate", TEMP_FAILURE_RETRY(ftruncate64(_fd, length)), &ec);
    return ec;
}

ECode CPosix::Gai_strerror(
    /* [in] */ Int32 error,
    /* [out] */ String* strerror)
{
    VALIDATE_NOT_NULL(strerror)
    *strerror = gai_strerror(error);
    return NOERROR;
}

ECode CPosix::Getegid(
    /* [out] */ Int32* egid)
{
    VALIDATE_NOT_NULL(egid)
    *egid = getegid();
    return NOERROR;
}

ECode CPosix::Geteuid(
    /* [out] */ Int32* euid)
{
    VALIDATE_NOT_NULL(euid)
    *euid = geteuid();
    return NOERROR;
}

ECode CPosix::Getgid(
    /* [out] */ Int32* gid)
{
    VALIDATE_NOT_NULL(gid)
    *gid = getgid();
    return NOERROR;
}

ECode CPosix::Getenv(
    /* [in] */ const String& name,
    /* [out] */ String* env)
{
    VALIDATE_NOT_NULL(env)
    if (name == NULL) {
        *env = NULL;
        return NOERROR;
    }
    *env = getenv(name);
    return NOERROR;
}

    /* TODO: break into getnameinfoHost and getnameinfoService? */
ECode CPosix::Getnameinfo(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 flags,
    /* [out] */ String* nameinfo)
{
    VALIDATE_NOT_NULL(nameinfo)
    sockaddr_storage ss;
    socklen_t sa_len;
    if (!InetAddressToSockaddrVerbatim(address, 0, ss, sa_len)) {
        *nameinfo = NULL;
        return NOERROR;
    }
    char buf[NI_MAXHOST]; // NI_MAXHOST is longer than INET6_ADDRSTRLEN.
    errno = 0;
    Int32 rc = getnameinfo(reinterpret_cast<sockaddr*>(&ss), sa_len, buf, sizeof(buf), NULL, 0, flags);
    if (rc != 0) {
        // throwGaiException(env, "getnameinfo", rc);
        ALOGE("System-call getnameinfo error , errno = %d", errno);
        *nameinfo = NULL;
        return E_GAI_EXCEPTION;
    }
    *nameinfo = String(buf);
    return NOERROR;
}

ECode CPosix::Getpeername(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ ISocketAddress** peername)
{
    VALIDATE_NOT_NULL(peername)
    AutoPtr<ISocketAddress> addr = DoGetSockName(fd, FALSE);
    *peername = addr;
    if (NULL == *peername) return E_ERRNO_EXCEPTION;
    REFCOUNT_ADD(*peername)
    return NOERROR;
}

ECode CPosix::Getpid(
    /* [out] */ Int32* pid)
{
    VALIDATE_NOT_NULL(pid)
    *pid = getpid();
    return NOERROR;
}

ECode CPosix::Getppid(
    /* [out] */ Int32* ppid)
{
    VALIDATE_NOT_NULL(ppid)
    *ppid = getppid();
    return NOERROR;
}

ECode CPosix::Getpwnam(
    /* [in] */ const String& name,
    /* [out] */ IStructPasswd** pwnam)
{
    VALIDATE_NOT_NULL(pwnam)
    if (name == NULL) {
        *pwnam = NULL;
    }
    AutoPtr<IStructPasswd> passwd = Passwd().getpwnam(name);
    *pwnam = passwd;
    REFCOUNT_ADD(*pwnam)
    return NOERROR;
}

ECode CPosix::Getpwuid(
    /* [in] */ Int32 uid,
    /* [out] */ IStructPasswd** pwuid)
{
    VALIDATE_NOT_NULL(pwuid)
    AutoPtr<IStructPasswd> passwd = Passwd().getpwuid(uid);
    *pwuid = passwd;
    REFCOUNT_ADD(*pwuid)
    return NOERROR;
}

ECode CPosix::Getsockname(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ ISocketAddress** sockname)
{
    VALIDATE_NOT_NULL(sockname)
    AutoPtr<ISocketAddress> addr = DoGetSockName(fd, TRUE);
    *sockname = addr;
    if (NULL == *sockname) return E_ERRNO_EXCEPTION;
    REFCOUNT_ADD(*sockname)
    return NOERROR;
}

ECode CPosix::GetsockoptByte(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [out] */ Int32* sockopt)
{
    VALIDATE_NOT_NULL(sockopt)
    *sockopt = 0;

    Int32 _fd;
    fd->GetDescriptor(&_fd);
    u_char result = 0;
    socklen_t size = sizeof(result);
    ECode ec;
    ErrorIfMinusOne("getsockopt", TEMP_FAILURE_RETRY(getsockopt(_fd, level, option, &result, &size)), &ec);
    FAIL_RETURN(ec)

    *sockopt = result;
    return ec;
}

ECode CPosix::GetsockoptInAddr(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [out] */ IInetAddress** addr)
{
    VALIDATE_NOT_NULL(addr)
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    sockaddr_storage ss;
    memset(&ss, 0, sizeof(ss));
    ss.ss_family = AF_INET; // This is only for the IPv4-only IP_MULTICAST_IF.
    sockaddr_in* sa = reinterpret_cast<sockaddr_in*>(&ss);
    socklen_t size = sizeof(sa->sin_addr);
    Int32 rc = TEMP_FAILURE_RETRY(getsockopt(_fd, level, option, &sa->sin_addr, &size));
    if (rc == -1) {
        // throwErrnoException(env, "getsockopt");
        ALOGE("System-call getsockopt error, errno = %d", errno);
        *addr = NULL;
        return E_ERRNO_EXCEPTION;
    }
    AutoPtr<IInetAddress> inetAddress = SockaddrToInetAddress(ss, NULL);
    *addr = inetAddress;
    REFCOUNT_ADD(*addr)
    return NOERROR;
}

ECode CPosix::GetsockoptInt32(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [out] */ Int32* sockopt)
{
    VALIDATE_NOT_NULL(sockopt)
    *sockopt = 0;

    Int32 _fd;
    fd->GetDescriptor(&_fd);
    Int32 result = 0;
    socklen_t size = sizeof(result);
    ECode ec;
    ErrorIfMinusOne("getsockopt", TEMP_FAILURE_RETRY(getsockopt(_fd, level, option, &result, &size)), &ec);
    FAIL_RETURN(ec)

    *sockopt = result;
    return ec;
}

ECode CPosix::GetsockoptLinger(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [out] */ IStructLinger** linger)
{
    VALIDATE_NOT_NULL(linger)
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    struct linger l;
    socklen_t size = sizeof(l);
    memset(&l, 0, size);
    Int32 rc = TEMP_FAILURE_RETRY(getsockopt(_fd, level, option, &l, &size));
    if (rc == -1) {
        // throwErrnoException(env, "getsockopt");
        ALOGE("System-call getsockopt error, errno = %d", errno);
        *linger = NULL;
        return E_ERRNO_EXCEPTION;
    }
    AutoPtr<IStructLinger> lingerObj = MakeStructLinger(l);
    *linger = lingerObj;
    REFCOUNT_ADD(*linger)
    return NOERROR;
}

ECode CPosix::GetsockoptTimeval(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [out] */ IStructTimeval** timeval)
{
    VALIDATE_NOT_NULL(timeval)
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    struct timeval tv;
    socklen_t size = sizeof(tv);
    memset(&tv, 0, size);
    Int32 rc = TEMP_FAILURE_RETRY(getsockopt(_fd, level, option, &tv, &size));
    if (rc == -1) {
        // throwErrnoException(env, "getsockopt");
        ALOGE("System-call getsockopt error, errno = %d", errno);
        *timeval = NULL;
        return E_ERRNO_EXCEPTION;
    }
    AutoPtr<IStructTimeval> timevalObj = MakeStructTimeval(tv);
    *timeval = timevalObj;
    REFCOUNT_ADD(*timeval)
    return NOERROR;
}

ECode CPosix::GetsockoptUcred(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [out] */ IStructUcred** ucred)
{
    VALIDATE_NOT_NULL(ucred)
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    struct ucred u;
    socklen_t size = sizeof(u);
    memset(&u, 0, size);
    Int32 rc = TEMP_FAILURE_RETRY(getsockopt(_fd, level, option, &u, &size));
    if (rc == -1) {
        // throwErrnoException(env, "getsockopt");
        ALOGE("System-call getsockopt error, errno = %d", errno);
        *ucred = NULL;
        return E_ERRNO_EXCEPTION;
    }
    AutoPtr<IStructUcred> ucredObj = MakeStructUcred(u);
    *ucred = ucredObj;
    REFCOUNT_ADD(*ucred)
    return NOERROR;
}

ECode CPosix::Gettid(
    /* [out] */ Int32* tid)
{
    VALIDATE_NOT_NULL(tid)
#if defined(__APPLE__)
    uint64_t owner;
    int rc = pthread_threadid_np(NULL, &owner);  // Requires Mac OS 10.6
    if (rc != 0) {
        // throwErrnoException(env, "gettid");
        ALOGE("System-call gettid error, errno = %d", errno);
        *tid = 0;
        return E_ERRNO_EXCEPTION;
    }
    *tid = static_cast<Int32>(owner);
    return NOERROR;
#else
    // Neither bionic nor glibc exposes gettid(2).
    *tid = syscall(__NR_gettid);
    return NOERROR;
#endif
}

ECode CPosix::Getuid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid)
    *uid =getuid();
    return NOERROR;
}

ECode CPosix::If_indextoname(
    /* [in] */ Int32 index,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    char buf[IF_NAMESIZE];
    char* _name = if_indextoname(index, buf);
    // if_indextoname(3) returns NULL on failure, which will come out of NewStringUTF unscathed.
    // There's no useful information in errno, so we don't bother throwing. Callers can null-check.
    *name = _name;
    return NOERROR;
}

ECode CPosix::Inet_pton(
    /* [in] */ Int32 family,
    /* [in] */ const String& name,
    /* [out] */ IInetAddress** addr)
{
    VALIDATE_NOT_NULL(addr)
    if (name == NULL) {
        *addr = NULL;
        return NOERROR;
    }
    sockaddr_storage ss;
    memset(&ss, 0, sizeof(ss));
    // sockaddr_in and sockaddr_in6 are at the same address, so we can use either here.
    void* dst = &reinterpret_cast<sockaddr_in*>(&ss)->sin_addr;
    if (inet_pton(family, name, dst) != 1) {
        *addr = NULL;
        return NOERROR;
    }
    ss.ss_family = family;
    AutoPtr<IInetAddress> inetAddress = SockaddrToInetAddress(ss, NULL);
    *addr = inetAddress;
    REFCOUNT_ADD(*addr)
    return NOERROR;
}

ECode CPosix::IoctlInetAddress(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 cmd,
    /* [in] */ const String& interfaceName,
    /* [out] */ IInetAddress** addr)
{
    VALIDATE_NOT_NULL(addr)
        *addr = NULL;

    struct ifreq req;
    if (!FillIfreq(interfaceName, req)) {
        return NOERROR;
    }
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    ECode ec;
    Int32 rc = ErrorIfMinusOne("ioctl", TEMP_FAILURE_RETRY(ioctl(_fd, cmd, &req)), &ec);
    FAIL_RETURN(ec)

    if (rc == -1) {
        return ec;
    }
    AutoPtr<IInetAddress> inetAddress = SockaddrToInetAddress(reinterpret_cast<sockaddr_storage&>(req.ifr_addr), NULL);
    *addr = inetAddress;
    REFCOUNT_ADD(*addr)
    return NOERROR;
}

ECode CPosix::IoctlInt(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 cmd,
    /* [in, out] */ Int32* arg,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    // This is complicated because ioctls may return their result by updating their argument
    // or via their return value, so we need to support both.
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    ECode ec;
    Int32 rc = ErrorIfMinusOne("ioctl", TEMP_FAILURE_RETRY(ioctl(_fd, cmd, arg)), &ec);
    FAIL_RETURN(ec)

    *result = rc;
    return ec;
}

ECode CPosix::Isatty(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Boolean* isatty)
{
    VALIDATE_NOT_NULL(isatty)
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    *isatty = TEMP_FAILURE_RETRY(::isatty(_fd)) == 1;
    return NOERROR;
}

ECode CPosix::Kill(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 signal)
{
    ECode ec;
    ErrorIfMinusOne("kill", TEMP_FAILURE_RETRY(kill(pid, signal)), &ec);
    return ec;
}

ECode CPosix::Lchown(
    /* [in] */ const String& path,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid)
{
    if (path == NULL) {
        return NOERROR;
    }
    ECode ec;
    ErrorIfMinusOne("lchown", TEMP_FAILURE_RETRY(lchown(path, uid, gid)), &ec);
    return ec;
}

ECode CPosix::Link(
    /* [in] */ const String& oldPath,
    /* [in] */ const String& newPath)
{
    if (oldPath == NULL) {
        return NOERROR;
    }
    if (newPath == NULL) {
        return NOERROR;
    }
    ECode ec;
    ErrorIfMinusOne("link", TEMP_FAILURE_RETRY(link(oldPath, newPath)), &ec);
    return ec;
}

ECode CPosix::Listen(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 backlog)
{
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    ECode ec;
    ErrorIfMinusOne("listen", TEMP_FAILURE_RETRY(listen(_fd, backlog)), &ec);
    return ec;
}

ECode CPosix::Lseek(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int32 whence,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Int32 _fd;
    fd->GetDescriptor(&_fd);
    ECode ec;
    *result = ErrorIfMinusOne("lseek", TEMP_FAILURE_RETRY(lseek64(_fd, offset, whence)), &ec);
    return ec;
}

ECode CPosix::Lstat(
    /* [in] */ const String& path,
    /* [out] */ IStructStat** stat)
{
    return DoStat(path, TRUE, stat);
}

ECode CPosix::Mincore(
    /* [in] */ Int64 address,
    /* [in] */ Int64 byteCount,
    /* [in] */ ArrayOf<Byte>* vector)
{
    if (vector == NULL) {
        return NOERROR;
    }
    void* ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(address));
    unsigned char* vec = reinterpret_cast<unsigned char*>(vector->GetPayload());
    ECode ec;
    ErrorIfMinusOne("mincore", TEMP_FAILURE_RETRY(mincore(ptr, byteCount, vec)), &ec);
    return ec;
}

ECode CPosix::Mkdir(
    /* [in] */ const String& path,
    /* [in] */ Int32 mode)
{
    if (path == NULL) {
        return NOERROR;
    }
    ECode ec;
    ErrorIfMinusOne("mkdir", TEMP_FAILURE_RETRY(mkdir(path, mode)), &ec, path.string());
    return ec;
}

ECode CPosix::Mlock(
    /* [in] */ Int64 address,
    /* [in] */ Int64 byteCount)
{
    void* ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(address));
    ECode ec;
    ErrorIfMinusOne("mlock", TEMP_FAILURE_RETRY(mlock(ptr, byteCount)), &ec);
    return ec;
}

ECode CPosix::Mkfifo(
    /* [in] */ const String& path,
    /* [in] */ Int32 mode)
{
    if (path == NULL) {
        return NOERROR;
    }
    ECode ec;
    ErrorIfMinusOne("mkfifo", TEMP_FAILURE_RETRY(mkfifo(path, mode)), &ec);
    return ec;
}

ECode CPosix::Mmap(
    /* [in] */ Int64 address,
    /* [in] */ Int64 byteCount,
    /* [in] */ Int32 prot,
    /* [in] */ Int32 flags,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    void* suggestedPtr = reinterpret_cast<void*>(static_cast<uintptr_t>(address));
    void* ptr = mmap(suggestedPtr, byteCount, prot, flags, _fd, offset);
    if (ptr == MAP_FAILED) {
        // throwErrnoException(env, "mmap");
        ALOGE("System-call mmap error, errno = %d", errno);
        *result = 0;
        return E_ERRNO_EXCEPTION;
    }
    *result = static_cast<Int64>(reinterpret_cast<uintptr_t>(ptr));
    return NOERROR;
}

ECode CPosix::Msync(
    /* [in] */ Int64 address,
    /* [in] */ Int64 byteCount,
    /* [in] */ Int32 flags)
{
    void* ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(address));
    ECode ec;
    ErrorIfMinusOne("msync", TEMP_FAILURE_RETRY(msync(ptr, byteCount, flags)), &ec);
    return ec;
}

ECode CPosix::Munlock(
    /* [in] */ Int64 address,
    /* [in] */ Int64 byteCount)
{
    void* ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(address));
    ECode ec;
    ErrorIfMinusOne("munlock", TEMP_FAILURE_RETRY(munlock(ptr, byteCount)), &ec);
    return ec;
}

ECode CPosix::Munmap(
    /* [in] */ Int64 address,
    /* [in] */ Int64 byteCount)
{
    void* ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(address));
    ECode ec;
    ErrorIfMinusOne("munmap", TEMP_FAILURE_RETRY(munmap(ptr, byteCount)), &ec);
    return ec;
}

ECode CPosix::Open(
    /* [in] */ const String& path,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 mode,
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd)
    *fd = NULL;

    if (path == NULL) {
        return NOERROR;
    }
    ECode ec;
    Int32 _fd = ErrorIfMinusOne("open", TEMP_FAILURE_RETRY(open(path, flags, mode)), &ec, path.string());
    FAIL_RETURN(ec)

    if (_fd != -1) {
        CFileDescriptor::New(fd);
        (*fd)->SetDescriptor(_fd);
    }
    return ec;
}

ECode CPosix::Pipe(
    /* [out, callee] */ ArrayOf<IFileDescriptor*>** fds)
{
    VALIDATE_NOT_NULL(fds)
    *fds = NULL;

    Int32 fdsArr[2];
    ECode ec;
    Int32 _fd = ErrorIfMinusOne("pipe", TEMP_FAILURE_RETRY(pipe(&fdsArr[0])), &ec);
    FAIL_RETURN(ec)

    if (_fd != -1) {
        AutoPtr<ArrayOf<IFileDescriptor*> > result = ArrayOf<IFileDescriptor*>::Alloc(2);
        for (Int32 i = 0; i < 2; ++i) {
            AutoPtr<IFileDescriptor> fd;
            CFileDescriptor::New((IFileDescriptor**)&fd);
            if (fd == NULL) {
                *fds = NULL;
                return NOERROR;
            }
            fd->SetDescriptor(fdsArr[i]);
            result->Set(i, fd);
        }
        *fds = result;
        REFCOUNT_ADD(*fds)
    }

    return ec;
}

    /* TODO: if we used the non-standard ppoll(2) behind the scenes, we could take a long timeout. */
ECode CPosix::Poll(
    /* [in] */ ArrayOf<IStructPollfd*>* pollfds,
    /* [in] */ Int32 timeoutMs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;
    // static jfieldID fdFid = env->GetFieldID(JniConstants::structPollfdClass, "fd", "Ljava/io/FileDescriptor;");
    // static jfieldID eventsFid = env->GetFieldID(JniConstants::structPollfdClass, "events", "S");
    // static jfieldID reventsFid = env->GetFieldID(JniConstants::structPollfdClass, "revents", "S");

    // Turn the Java android.system.StructPollfd[] into a C++ struct pollfd[].
    size_t arrayLength = pollfds->GetLength();
    UniquePtr<struct pollfd[]> fds(new struct pollfd[arrayLength]);
    memset(fds.get(), 0, sizeof(struct pollfd) * arrayLength);
    size_t count = 0; // Some trailing array elements may be irrelevant. (See below.)
    for (size_t i = 0; i < arrayLength; ++i) {
        AutoPtr<IStructPollfd> structPollfd = (*pollfds)[i];
        if (structPollfd == NULL) {
            break; // We allow trailing nulls in the array for caller convenience.
        }
        AutoPtr<IFileDescriptor> fd;
        structPollfd->GetFd((IFileDescriptor**)&fd);
        if (fd == NULL) {
            break; // We also allow callers to just clear the fd field (this is what Selector does).
        }
        Int32 fdTmp;
        fd->GetDescriptor(&fdTmp);
        fds[count].fd = fdTmp;
        Int16 eventsTmp;
        structPollfd->GetEvents(&eventsTmp);
        fds[count].events = eventsTmp;
        ++count;
    }

    Vector<AsynchronousCloseMonitor*> monitors;
    for (size_t i = 0; i < count; ++i) {
        monitors.PushBack(new AsynchronousCloseMonitor(fds[i].fd));
    }
    int rc = poll(fds.get(), count, timeoutMs);
    for (size_t i = 0; i < monitors.GetSize(); ++i) {
        delete monitors[i];
    }
    if (rc == -1) {
        // throwErrnoException(env, "poll");
        ALOGE("syscall_name poll error, errno = %d", errno);
        *result = -1;
        return E_ERRNO_EXCEPTION;
    }

    // Update the revents fields in the Java android.system.StructPollfd[].
    for (size_t i = 0; i < count; ++i) {
        AutoPtr<IStructPollfd> structPollfd = (*pollfds)[i];
        if (structPollfd == NULL) {
            *result = -1;
            return NOERROR;
        }
        Int16 reventsTmp;
        structPollfd->GetRevents(&reventsTmp);
        fds[i].revents = reventsTmp;
    }
    *result = rc;
    return NOERROR;
}

ECode CPosix::Posix_fallocate(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length)
{
#ifdef __APPLE__
    ALOGE("fallocate doesn't exist on a Mac");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
#else
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    errno = TEMP_FAILURE_RETRY(posix_fallocate64(_fd, offset, length));
    if (errno != 0) {
        // throwErrnoException(env, "posix_fallocate");
        ALOGE("System-call posix_fallocate error, errno = %d", errno);
        return E_ERRNO_EXCEPTION;
    }
    return NOERROR;
#endif
}

ECode CPosix::Prctl(
    /* [in] */ Int32 option,
    /* [in] */ Int64 arg2,
    /* [in] */ Int64 arg3,
    /* [in] */ Int64 arg4,
    /* [in] */ Int64 arg5,
    /* [out] */ Int32* prctlOut)
{
    VALIDATE_NOT_NULL(prctlOut)
#ifdef __APPLE__
    ALOGE("prctl doesn't exist on a Mac");
    *prctlOut = 0;
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
#else
    int result = prctl(static_cast<int>(option),
                       static_cast<unsigned long>(arg2), static_cast<unsigned long>(arg3),
                       static_cast<unsigned long>(arg4), static_cast<unsigned long>(arg5));
    ECode ec;
    *prctlOut = ErrorIfMinusOne("prctl", result, &ec);
    return ec;
#endif
}

ECode CPosix::Pread(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int64 offset,
    /* [out] */ Int32* num)
{
    return PreadBytes(fd, bytes, byteOffset, byteCount, offset, num);
}

ECode CPosix::Pread(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int64 offset,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    // TODO::
    Int32 position, remaining;
    AutoPtr<IBuffer> buf = IBuffer::Probe(buffer);
    if (buf == NULL) {
        *num = -1;
        return NOERROR;
    }
    buf->GetPosition(&position);
    buf->GetRemaining(&remaining);
    return PreadBytes(fd, buffer, position, remaining, offset, num);
}

ECode CPosix::Pwrite(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int64 offset,
    /* [out] */ Int32* num)
{
    return PwriteBytes(fd, bytes, byteOffset, byteCount, offset, num);
}

ECode CPosix::Pwrite(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int64 offset,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    // TODO::
    Int32 position, remaining;
    AutoPtr<IBuffer> buf = IBuffer::Probe(buffer);
    if (buf == NULL) {
        *num = -1;
        return NOERROR;
    }
    buf->GetPosition(&position);
    buf->GetRemaining(&remaining);
    return PwriteBytes(fd, buffer, position, remaining, offset, num);
}

ECode CPosix::Read(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* num)
{
    return ReadBytes(fd, bytes, byteOffset, byteCount, num);
}

ECode CPosix::Read(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    // TODO::
    Int32 position, remaining;
    AutoPtr<IBuffer> buf = IBuffer::Probe(buffer);
    if (buf == NULL) {
        *num = -1;
        return NOERROR;
    }
    buf->GetPosition(&position);
    buf->GetRemaining(&remaining);
    return ReadBytes(fd, buffer, position, remaining, num);
}

ECode CPosix::Readlink(
    /* [in] */ const String& path,
    /* [out] */ String* link)
{
    VALIDATE_NOT_NULL(link)
    if (path == NULL) {
        *link = NULL;
        return NOERROR;
    }

    String result;
    if (!readlink(path, result)) {
        // throwErrnoException(env, "readlink");
        ALOGE("System-call readlink error, errno = %d", errno);
        *link = NULL;
        return E_ERRNO_EXCEPTION;
    }
    *link = result;
    return NOERROR;
}

ECode CPosix::Readv(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<IInterface*>* buffers,
    /* [in] */ ArrayOf<Int32>* offsets,
    /* [in] */ ArrayOf<Int32>* byteCounts,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    if (offsets == NULL || byteCounts == NULL || buffers == NULL) {
        *num = -1;
        return NOERROR;
    }
    // TODO: Linux actually has a 1024 buffer limit. glibc works around this, and we should too.
    // TODO: you can query the limit at runtime with sysconf(_SC_IOV_MAX).
    Vector<iovec> ioVec;
    Vector<AutoPtr<ArrayOf<Byte> > > buffersVec;
    Int32 length = buffers->GetLength();
    for (Int32 i = 0; i < length; ++i) {
        AutoPtr<IInterface> buffer = (*buffers)[i];
        AutoPtr<IByteBuffer> byteBuffer = IByteBuffer::Probe(buffer);
        if (byteBuffer == NULL) {
            *num = -1;
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<ArrayOf<Byte> > byteArr;
        byteBuffer->GetArray((ArrayOf<Byte>**)&byteArr);
        if (byteArr == NULL) {
            // TODO:: buffer.isDirect()
            *num = -1;
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        buffersVec.PushBack(byteArr);
        Byte* ptr = const_cast<Byte*>(buffersVec.GetBack()->GetPayload());
        if (ptr == NULL) {
            *num = -1;
            return NOERROR;
        }
        struct iovec iov;
        iov.iov_base = reinterpret_cast<void*>(ptr + (*offsets)[i]);
        iov.iov_len = (*byteCounts)[i];
        ioVec.PushBack(iov);
    }
    ECode ec;
    *num = IO_FAILURE_RETRY(ssize_t, ec, readv, fd, &ioVec[0], length);
    return ec;
}

ECode CPosix::Recvfrom(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int32 flags,
    /* [in] */ IInetSocketAddress* srcAddress,
    /* [out] */ Int32* num)
{
    return RecvfromBytes(fd, bytes, byteOffset, byteCount, flags, srcAddress, num);
}

ECode CPosix::Recvfrom(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 flags,
    /* [in] */ IInetSocketAddress* srcAddress,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    // TODO::
    Int32 position, remaining;
    AutoPtr<IBuffer> buf = IBuffer::Probe(buffer);
    if (buf == NULL) {
        *num = -1;
        return NOERROR;
    }
    buf->GetPosition(&position);
    buf->GetRemaining(&remaining);
    return RecvfromBytes(fd, buffer, position, remaining, flags, srcAddress, num);
}

ECode CPosix::Remove(
    /* [in] */ const String& path)
{
    if (path == NULL) {
        return NOERROR;
    }
    ECode ec;
    ErrorIfMinusOne("remove", TEMP_FAILURE_RETRY(remove(path)), &ec);
    return ec;
}

ECode CPosix::Rename(
    /* [in] */ const String& oldPath,
    /* [in] */ const String& newPath)
{
    if (oldPath == NULL || newPath == NULL) {
        return NOERROR;
    }
    ECode ec;
    ErrorIfMinusOne("rename", TEMP_FAILURE_RETRY(rename(oldPath, newPath)), &ec);
    return ec;
}

ECode CPosix::Sendto(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int32 flags,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [out] */ Int32* num)
{
    return SendtoBytes(fd, bytes, byteOffset, byteCount, flags, inetAddress, port, num);
}

ECode CPosix::Sendto(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 flags,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    // TODO::
    Int32 position, remaining;
    AutoPtr<IBuffer> buf = IBuffer::Probe(buffer);
    if (buf == NULL) {
        *num = -1;
        return NOERROR;
    }
    buf->GetPosition(&position);
    buf->GetRemaining(&remaining);
    return SendtoBytes(fd, buffer, position, remaining, flags, inetAddress, port, num);
}

ECode CPosix::Sendfile(
    /* [in] */ IFileDescriptor* outFd,
    /* [in] */ IFileDescriptor* inFd,
    /* [out] */ Int64* inOffset,
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Int32 _outFd, _inFd;
    outFd->GetDescriptor(&_outFd);
    inFd->GetDescriptor(&_inFd);

    off_t offset = 0;
    off_t* offsetPtr = NULL;
    if (inOffset != NULL) {
        // TODO: fix bionic so we can have a 64-bit off_t!
        offset = *inOffset;
        offsetPtr = &offset;
    }

    ECode ec;
    *result = ErrorIfMinusOne("sendfile", TEMP_FAILURE_RETRY(sendfile(_outFd, _inFd, offsetPtr, byteCount)), &ec);
    FAIL_RETURN(ec)

    if (inOffset != NULL) {
        *inOffset = offset;
    }

    return ec;
}

ECode CPosix::Setegid(
    /* [in] */ Int32 egid)
{
    ECode ec;
    ErrorIfMinusOne("setegid", TEMP_FAILURE_RETRY(setegid(egid)), &ec);
    return ec;
}

ECode CPosix::Setenv(
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [in] */ Boolean overwrite)
{
    if (name == NULL || value == NULL) {
        return NOERROR;
    }
    ECode ec;
    ErrorIfMinusOne("setenv", setenv(name, value, overwrite), &ec);
    return ec;
}

ECode CPosix::Seteuid(
    /* [in] */ Int32 euid)
{
    ECode ec;
    ErrorIfMinusOne("seteuid", TEMP_FAILURE_RETRY(seteuid(euid)), &ec);
    return ec;
}

ECode CPosix::Setgid(
    /* [in] */ Int32 gid)
{
    ECode ec;
    ErrorIfMinusOne("setgid", TEMP_FAILURE_RETRY(setgid(gid)), &ec);
    return ec;
}

ECode CPosix::Setsid(
    /* [out] */ Int32* sid)
{
    ECode ec;
    ErrorIfMinusOne("setsid", TEMP_FAILURE_RETRY(setsid()), &ec);
    return ec;
}

ECode CPosix::SetsockoptByte(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ Int32 value)
{
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    u_char byte = value;
    ECode ec;
    ErrorIfMinusOne("setsockopt", TEMP_FAILURE_RETRY(setsockopt(_fd, level, option, &byte, sizeof(byte))), &ec);
    return ec;
}

ECode CPosix::SetsockoptIfreq(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ const String& interfaceName)
{
    struct ifreq req;
    if (!FillIfreq(interfaceName, req)) {
        return NOERROR;
    }
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    ECode ec;
    ErrorIfMinusOne("setsockopt", TEMP_FAILURE_RETRY(setsockopt(_fd, level, option, &req, sizeof(req))), &ec);
    return ec;
}

ECode CPosix::SetsockoptInt32(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ Int32 value)
{
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    ECode ec;
    ErrorIfMinusOne("setsockopt", TEMP_FAILURE_RETRY(setsockopt(_fd, level, option, &value, sizeof(value))), &ec);
    return ec;
}

ECode CPosix::SetsockoptIpMreqn(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ Int32 value)
{
#if defined(__APPLE__) && MAC_OS_X_VERSION_MAX_ALLOWED < 1070
    abort();
#else
    ip_mreqn req;
    memset(&req, 0, sizeof(req));
    req.imr_ifindex = value;
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    ECode ec;
    ErrorIfMinusOne("setsockopt", TEMP_FAILURE_RETRY(setsockopt(_fd, level, option, &req, sizeof(req))), &ec);
    return ec;
#endif
}

ECode CPosix::SetsockoptGroupReq(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ IStructGroupReq* value)
{
#if defined(__APPLE__) && MAC_OS_X_VERSION_MAX_ALLOWED < 1070
    abort();
#else
    struct group_req req;
    memset(&req, 0, sizeof(req));

    // static jfieldID grInterfaceFid = env->GetFieldID(JniConstants::structGroupReqClass, "gr_interface", "I");
    Int32 gr_interface;
    value->GetGrInterface(&gr_interface);
    req.gr_interface = gr_interface;
    // Get the IPv4 or IPv6 multicast address to join or leave.
    // static jfieldID grGroupFid = env->GetFieldID(JniConstants::structGroupReqClass, "gr_group", "Ljava/net/InetAddress;");
    AutoPtr<IInetAddress> group;
    value->GetGrGroup((IInetAddress**)&group);
    socklen_t sa_len;
    if (!InetAddressToSockaddrVerbatim(group, 0, req.gr_group, sa_len)) {
        return NOERROR;
    }

    Int32 _fd;
    fd->GetDescriptor(&_fd);
    Int32 rc = TEMP_FAILURE_RETRY(setsockopt(_fd, level, option, &req, sizeof(req)));
    if (rc == -1 && errno == EINVAL) {
        // Maybe we're a 32-bit binary talking to a 64-bit kernel?
        // glibc doesn't automatically handle this.
        // http://sourceware.org/bugzilla/show_bug.cgi?id=12080
        struct group_req64 {
            uint32_t gr_interface;
            uint32_t my_padding;
            sockaddr_storage gr_group;
        };
        group_req64 req64;
        req64.gr_interface = req.gr_interface;
        memcpy(&req64.gr_group, &req.gr_group, sizeof(req.gr_group));
        rc = TEMP_FAILURE_RETRY(setsockopt(_fd, level, option, &req64, sizeof(req64)));
    }
    ECode ec;
    ErrorIfMinusOne("setsockopt", rc, &ec);
    return ec;
#endif
}

ECode CPosix::SetsockoptGroupSourceReq(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ IStructGroupSourceReq* value)
{
#if defined(__APPLE__) && MAC_OS_X_VERSION_MAX_ALLOWED < 1070
    abort();
#else
    socklen_t sa_len;
    struct group_source_req req;
    memset(&req, 0, sizeof(req));

    // static jfieldID gsrInterfaceFid = env->GetFieldID(JniConstants::structGroupSourceReqClass, "gsr_interface", "I");
    Int32 gsr_interface;
    value->GetGsrInterface(&gsr_interface);
    req.gsr_interface = gsr_interface;
    // Get the IPv4 or IPv6 multicast address to join or leave.
    // static jfieldID gsrGroupFid = env->GetFieldID(JniConstants::structGroupSourceReqClass, "gsr_group", "Ljava/net/InetAddress;");
    // ScopedLocalRef<jobject> javaGroup(env, env->GetObjectField(javaGroupSourceReq, gsrGroupFid));
    AutoPtr<IInetAddress> group;
    value->GetGsrGroup((IInetAddress**)&group);
    if (!InetAddressToSockaddrVerbatim(group, 0, req.gsr_group, sa_len)) {
        return NOERROR;
    }

    // Get the IPv4 or IPv6 multicast address to add to the filter.
    // static jfieldID gsrSourceFid = env->GetFieldID(JniConstants::structGroupSourceReqClass, "gsr_source", "Ljava/net/InetAddress;");
    // ScopedLocalRef<jobject> javaSource(env, env->GetObjectField(javaGroupSourceReq, gsrSourceFid));
    AutoPtr<IInetAddress> source;
    value->GetGsrSource((IInetAddress**)&source);
    if (!InetAddressToSockaddrVerbatim(source, 0, req.gsr_source, sa_len)) {
        return NOERROR;
    }

    Int32 _fd;
    fd->GetDescriptor(&_fd);
    Int32 rc = TEMP_FAILURE_RETRY(setsockopt(_fd, level, option, &req, sizeof(req)));
    if (rc == -1 && errno == EINVAL) {
        // Maybe we're a 32-bit binary talking to a 64-bit kernel?
        // glibc doesn't automatically handle this.
        // http://sourceware.org/bugzilla/show_bug.cgi?id=12080
        struct group_source_req64 {
            uint32_t gsr_interface;
            uint32_t my_padding;
            sockaddr_storage gsr_group;
            sockaddr_storage gsr_source;
        };
        group_source_req64 req64;
        req64.gsr_interface = req.gsr_interface;
        memcpy(&req64.gsr_group, &req.gsr_group, sizeof(req.gsr_group));
        memcpy(&req64.gsr_source, &req.gsr_source, sizeof(req.gsr_source));
        rc = TEMP_FAILURE_RETRY(setsockopt(_fd, level, option, &req64, sizeof(req64)));
    }
    ECode ec;
    ErrorIfMinusOne("setsockopt", rc, &ec);
    return ec;
#endif
}

ECode CPosix::SetsockoptLinger(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ IStructLinger* strLinger)
{
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    struct linger value;
    Int32 onoff, linger;
    strLinger->GetOnoff(&onoff);
    strLinger->GetLinger(&linger);

    value.l_onoff = onoff;
    value.l_linger = linger;
    ECode ec;
    ErrorIfMinusOne("setsockopt", TEMP_FAILURE_RETRY(setsockopt(_fd, level, option, &value, sizeof(value))), &ec);
    return ec;
}

ECode CPosix::SetsockoptTimeval(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ IStructTimeval* strTimeval)
{
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    struct timeval value;
    Int64 sec, usec;
    strTimeval->GetSec(&sec);
    strTimeval->GetUsec(&usec);
    value.tv_sec = sec;
    value.tv_usec = usec;
    ECode ec;
    ErrorIfMinusOne("setsockopt", TEMP_FAILURE_RETRY(setsockopt(_fd, level, option, &value, sizeof(value))), &ec);
    return ec;
}

ECode CPosix::Setuid(
    /* [in] */ Int32 uid)
{
    ECode ec;
    ErrorIfMinusOne("setuid", TEMP_FAILURE_RETRY(setuid(uid)), &ec);
    return ec;
}

ECode CPosix::Shutdown(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 how)
{
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    ECode ec;
    ErrorIfMinusOne("shutdown", TEMP_FAILURE_RETRY(shutdown(_fd, how)), &ec);
    return ec;
}

ECode CPosix::Socket(
    /* [in] */ Int32 socketDomain,
    /* [in] */ Int32 type,
    /* [in] */ Int32 protocol,
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd)
    *fd = NULL;

    ECode ec;
    Int32 _fd = ErrorIfMinusOne("socket", TEMP_FAILURE_RETRY(socket(socketDomain, type, protocol)), &ec);
    FAIL_RETURN(ec)

    if (_fd != -1) {
        CFileDescriptor::New(fd);
        (*fd)->SetDescriptor(_fd);
    }
    return ec;
}

ECode CPosix::Socketpair(
    /* [in] */ Int32 socketDomain,
    /* [in] */ Int32 type,
    /* [in] */ Int32 protocol,
    /* [in] */ IFileDescriptor* fd1,
    /* [in] */ IFileDescriptor* fd2)
{
    Int32 fds[2];
    ECode ec;
    Int32 rc = ErrorIfMinusOne("socketpair", TEMP_FAILURE_RETRY(socketpair(socketDomain, type, protocol, fds)), &ec);
    FAIL_RETURN(ec)

    if (rc != -1) {
        fd1->SetDescriptor(fds[0]);
        fd2->SetDescriptor(fds[1]);
    }
    return ec;
}

ECode CPosix::Stat(
    /* [in] */ const String& path,
    /* [out] */ IStructStat** stat)
{
    return DoStat(path, FALSE, stat);
}

    /* TODO: replace statfs with statvfs. */
ECode CPosix::StatVfs(
    /* [in] */ const String& path,
    /* [out] */ IStructStatVfs** vfsResult)
{
    VALIDATE_NOT_NULL(vfsResult)
    if (path == NULL) {
        *vfsResult = NULL;
        return NOERROR;
    }
    struct statvfs sb;
    Int32 rc = TEMP_FAILURE_RETRY(statvfs(path, &sb));
    if (rc == -1) {
        // throwErrnoException(env, "statvfs");
        ALOGE("System-call statvfs error, errno = %d", errno);
        *vfsResult = NULL;
        return E_ERRNO_EXCEPTION;
    }
    AutoPtr<IStructStatVfs> statObj = MakeStructStatVfs(sb);
    *vfsResult = statObj;
    if (NULL == *vfsResult) return E_ERRNO_EXCEPTION;
    REFCOUNT_ADD(*vfsResult)
    return NOERROR;
}

ECode CPosix::Strerror(
    /* [in] */ Int32 errnum,
    /* [out] */ String* strerr)
{
    VALIDATE_NOT_NULL(strerr)
    const char* message = strerror(errnum);
    *strerr = message;
    return NOERROR;
}

ECode CPosix::Strsignal(
    /* [in] */ Int32 signal,
    /* [out] */ String* strSignal)
{
    VALIDATE_NOT_NULL(strSignal)
    *strSignal = strsignal(signal);
    return NOERROR;
}

ECode CPosix::Symlink(
    /* [in] */ const String& oldPath,
    /* [in] */ const String& newPath)
{
    if (oldPath == NULL || newPath == NULL) {
        return NOERROR;
    }

    ECode ec;
    ErrorIfMinusOne("symlink", TEMP_FAILURE_RETRY(symlink(oldPath, newPath)), &ec);
    return ec;
}

ECode CPosix::Sysconf(
    /* [in] */ Int32 name,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    // Since -1 is a valid result from sysconf(3), detecting failure is a little more awkward.
    errno = 0;
    *result = sysconf(name);
    if (*result == -1L && errno == EINVAL) {
        // throwErrnoException(env, "sysconf");
        ALOGE("System-call sysconf error, errno = %d", errno);
        return E_ERRNO_EXCEPTION;
    }
    return NOERROR;
}

ECode CPosix::Tcdrain(
    /* [in] */ IFileDescriptor* fd)
{
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    ECode ec;
    ErrorIfMinusOne("tcdrain", TEMP_FAILURE_RETRY(tcdrain(_fd)), &ec);
    return ec;
}

ECode CPosix::Tcsendbreak(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 duration)
{
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    ECode ec;
    ErrorIfMinusOne("tcsendbreak", TEMP_FAILURE_RETRY(tcsendbreak(_fd, duration)), &ec);
    return ec;
}


ECode CPosix::Umask(
    /* [in] */ Int32 mask,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if ((mask & 0777) != mask) {
        ALOGE("Invalid umask: %d", mask);
        // throw new IllegalArgumentException("Invalid umask: " + mask);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return umask(mask);
}

ECode CPosix::Uname(
    /* [out] */ IStructUtsname** unameOut)
{
    VALIDATE_NOT_NULL(unameOut)
    struct utsname buf;
    if (TEMP_FAILURE_RETRY(uname(&buf)) == -1) {
        *unameOut = NULL; // Can't happen.
    }
    AutoPtr<IStructUtsname> utsnameObj = MakeStructUtsname(buf);
    *unameOut = utsnameObj;
    REFCOUNT_ADD(*unameOut)
    return NOERROR;
}

ECode CPosix::Unsetenv(
    /* [in] */ const String& name)
{
    if (name == NULL) {
        return NOERROR;
    }
    ECode ec;
    ErrorIfMinusOne("unsetenv", unsetenv(name), &ec);
    return ec;
}

ECode CPosix::Waitpid(
    /* [in] */ Int32 pid,
    /* [out] */ Int32* statusArg,
    /* [in] */ Int32 options,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;

    Int32 status;
    ECode ec;
    Int32 rc = ErrorIfMinusOne("waitpid", TEMP_FAILURE_RETRY(waitpid(pid, &status, options)), &ec);
    FAIL_RETURN(ec)

    if (rc != -1 && statusArg != NULL) {
        *statusArg = status;
    }
    *result = rc;
    return ec;
}

ECode CPosix::Write(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* num)
{
    return WriteBytes(fd, bytes, byteOffset, byteCount, num);
}

ECode CPosix::Write(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    // TODO::
    Int32 position, remaining;
    AutoPtr<IBuffer> buf = IBuffer::Probe(buffer);
    if (buf == NULL) {
        *num = -1;
        return NOERROR;
    }
    buf->GetPosition(&position);
    buf->GetRemaining(&remaining);
    return WriteBytes(fd, buffer, position, remaining, num);
}

ECode CPosix::Writev(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<IInterface*>* buffers,
    /* [in] */ ArrayOf<Int32>* offsets,
    /* [in] */ ArrayOf<Int32>* byteCounts,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    if (offsets == NULL || byteCounts == NULL || buffers == NULL) {
        *num = -1;
        return NOERROR;
    }
    // TODO: Linux actually has a 1024 buffer limit. glibc works around this, and we should too.
    // TODO: you can query the limit at runtime with sysconf(_SC_IOV_MAX).
    Vector<iovec> ioVec;
    Vector<AutoPtr<ArrayOf<Byte> > > buffersVec;
    Int32 length = buffers->GetLength();
    for (Int32 i = 0; i < length; ++i) {
        AutoPtr<IInterface> buffer = (*buffers)[i];
        AutoPtr<IByteBuffer> byteBuffer = IByteBuffer::Probe(buffer);
        if (byteBuffer == NULL) {
            *num = -1;
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<ArrayOf<Byte> > byteArr;
        byteBuffer->GetArray((ArrayOf<Byte>**)&byteArr);
        if (byteArr == NULL) {
            // TODO:: buffer.isDirect()
            *num = -1;
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        buffersVec.PushBack(byteArr);
        Byte* ptr = const_cast<Byte*>(buffersVec.GetBack()->GetPayload());
        if (ptr == NULL) {
            *num = -1;
            return NOERROR;
        }
        struct iovec iov;
        iov.iov_base = reinterpret_cast<void*>(ptr + (*offsets)[i]);
        iov.iov_len = (*byteCounts)[i];
        ioVec.PushBack(iov);
    }
    ECode ec;
    *num = IO_FAILURE_RETRY(ssize_t, ec, writev, fd, &ioVec[0], length);
    return ec;
}

ECode CPosix::PreadBytes(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 bufferOffset,
    /* [in] */ Int32 bufferCount,
    /* [in] */ Int64 offset,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (buffer == NULL) {
        *result = -1;
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > array_;
    buffer->GetArray((ArrayOf<Byte>**)&array_);
    ECode ec;
    *result = IO_FAILURE_RETRY(ssize_t, ec, pread64, fd, array_->GetPayload() + bufferOffset, bufferCount, offset);
    return ec;
}

ECode CPosix::PreadBytes(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* byteArray,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int64 offset,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (byteArray == NULL) {
        *result = -1;
        return NOERROR;
    }
    ECode ec;
    *result = IO_FAILURE_RETRY(ssize_t, ec, pread64, fd, byteArray->GetPayload() + byteOffset, byteCount, offset);
    return ec;
}

ECode CPosix::PwriteBytes(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 bufferOffset,
    /* [in] */ Int32 bufferCount,
    /* [in] */ Int64 offset,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (buffer == NULL) {
        *result = -1;
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > array_;
    buffer->GetArray((ArrayOf<Byte>**)&array_);
    return PwriteBytes(fd, array_, bufferOffset, bufferCount, offset, result);
}

ECode CPosix::PwriteBytes(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* byteArray,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int64 offset,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (byteArray == NULL) {
        *result = -1;
        return NOERROR;
    }
    ECode ec;
    *result = IO_FAILURE_RETRY(ssize_t, ec, pwrite64, fd, byteArray->GetPayload() + byteOffset, byteCount, offset);
    return ec;
}

ECode CPosix::ReadBytes(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 bufferOffset,
    /* [in] */ Int32 bufferCount,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (buffer == NULL) {
        *result = -1;
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > array_;
    buffer->GetArray((ArrayOf<Byte>**)&array_); // TODO buffer.isDirect
    return ReadBytes(fd, array_, bufferOffset, bufferCount, result);
}

ECode CPosix::ReadBytes(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* byteArray,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (byteArray == NULL) {
        *result = -1;
        return NOERROR;
    }
    ECode ec;
    *result = IO_FAILURE_RETRY(ssize_t, ec, read, fd, byteArray->GetPayload() + byteOffset, byteCount);
    return ec;
}

ECode CPosix::RecvfromBytes(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 bufferOffset,
    /* [in] */ Int32 bufferCount,
    /* [in] */ Int32 flags,
    /* [in] */ IInetSocketAddress* srcAddress,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (buffer == NULL) {
        *result = -1;
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > array_;
    buffer->GetArray((ArrayOf<Byte>**)&array_); // TODO buffer.isDirect()
    return RecvfromBytes(fd, array_, bufferOffset, bufferCount, flags, srcAddress, result);
}

ECode CPosix::RecvfromBytes(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* byteArray,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int32 flags,
    /* [in] */ IInetSocketAddress* srcAddress,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (byteArray == NULL) {
        *result = -1;
        return NOERROR;
    }
    sockaddr_storage ss;
    socklen_t sl = sizeof(ss);
    memset(&ss, 0, sizeof(ss));
    sockaddr* from = (srcAddress != NULL) ? reinterpret_cast<sockaddr*>(&ss) : NULL;
    socklen_t* fromLength = (srcAddress != NULL) ? &sl : 0;
    ECode ec;
    Int32 recvCount = NET_FAILURE_RETRY(ssize_t, ec, recvfrom, fd, byteArray->GetPayload() + byteOffset, byteCount, flags, from, fromLength);
    FillInetSocketAddress(recvCount, srcAddress, ss);
    *result = recvCount;
    return ec;
}

ECode CPosix::SendtoBytes(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 bufferOffset,
    /* [in] */ Int32 bufferCount,
    /* [in] */ Int32 flags,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (buffer == NULL) {
        *result = -1;
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > array_;
    buffer->GetArray((ArrayOf<Byte>**)&array_); //TODO: buffer.isDirect()
    return SendtoBytes(fd, array_, bufferOffset, bufferCount, flags, inetAddress, port, result);
}

ECode CPosix::SendtoBytes(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* byteArray,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int32 flags,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (byteArray == NULL) {
        *result = -1;
        return NOERROR;
    }
    sockaddr_storage ss;
    socklen_t sa_len = 0;
    if (inetAddress != NULL && !InetAddressToSockaddr(inetAddress, port, ss, sa_len)) {
        *result = -1;
        return NOERROR;
    }
    const sockaddr* to = (inetAddress != NULL) ? reinterpret_cast<const sockaddr*>(&ss) : NULL;
    ECode ec;
    *result = NET_FAILURE_RETRY(ssize_t, ec, sendto, fd, byteArray->GetPayload() + byteOffset, byteCount, flags, to, sa_len);
    return ec;
}

ECode CPosix::UmaskImpl(
    /* [in] */ Int32 mask,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = umask(mask);
    return NOERROR;
}

ECode CPosix::WriteBytes(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 bufferOffset,
    /* [in] */ Int32 bufferCount,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (buffer == NULL) {
        *result = -1;
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > array_;
    buffer->GetArray((ArrayOf<Byte>**)&array_); //TODO: buffer.isDirect()
    return WriteBytes(fd, array_, bufferOffset, bufferCount, result);
}

ECode CPosix::WriteBytes(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* byteArray,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (byteArray == NULL) {
        *result = -1;
        return NOERROR;
    }
    ECode ec;
    *result = IO_FAILURE_RETRY(ssize_t, ec, write, fd, byteArray->GetPayload() + byteOffset, byteCount);
    return ec;
}

} // namespace IO
} // namespace Libcore
