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

#ifndef __ELASTOS_DROID_SYSTEM_OS_H__
#define __ELASTOS_DROID_SYSTEM_OS_H__

#include <elastos/core/Object.h>

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IByteBuffer;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInetSocketAddress;
using Elastos::Net::ISocketAddress;
using Elastos::Droid::System::IStructAddrinfo;
using Elastos::Droid::System::IStructFlock;
using Elastos::Droid::System::IStructGroupReq;
using Elastos::Droid::System::IStructGroupSourceReq;
using Elastos::Droid::System::IStructLinger;
using Elastos::Droid::System::IStructPasswd;
using Elastos::Droid::System::IStructPollfd;
using Elastos::Droid::System::IStructStat;
using Elastos::Droid::System::IStructStatVfs;
using Elastos::Droid::System::IStructTimeval;
using Elastos::Droid::System::IStructUcred;
using Elastos::Droid::System::IStructUtsname;

namespace Elastos {
namespace Droid {
namespace System {

class ECO_PUBLIC Os
{
public:
    static CARAPI Accept(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IInetSocketAddress* peerAddress,
        /* [out] */ IFileDescriptor** retFd);

    static CARAPI Access(
        /* [in] */ const String& path,
        /* [in] */ Int32 mode,
        /* [out] */ Boolean* succeed);

    static CARAPI Elastos_getaddrinfo(
        /* [in] */ const String& node,
        /* [in] */ IStructAddrinfo* hints,
        /* [in] */ Int32 netId,
        /* [out, callee] */ ArrayOf<IInetAddress*>** info);

    static CARAPI Bind(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 port);

    static CARAPI Chmod(
        /* [in] */ const String& path,
        /* [in] */ Int32 mode);

    static CARAPI Chown(
        /* [in] */ const String& path,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 gid);

    static CARAPI Close(
        /* [in] */ IFileDescriptor* fd);

    static CARAPI Connect(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 port);

    static CARAPI Dup(
        /* [in] */ IFileDescriptor* oldFd,
        /* [out] */ IFileDescriptor** retFd);

    static CARAPI Dup2(
        /* [in] */ IFileDescriptor* oldFd,
        /* [in] */ Int32 newFd,
        /* [out] */ IFileDescriptor** retFd);

    static CARAPI Environ(
        /* [out, callee] */ ArrayOf<String>** env);

    static CARAPI Execv(
        /* [in] */ const String& filename,
        /* [in] */ ArrayOf<String>* argv);

    static CARAPI Execve(
        /* [in] */ const String& filename,
        /* [in] */ ArrayOf<String>* argv,
        /* [in] */ ArrayOf<String>* envp);

    static CARAPI Fchmod(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 mode);

    static CARAPI Fchown(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 gid);

    static CARAPI FcntlVoid(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 cmd,
        /* [out] */ Int32* result);

    static CARAPI FcntlInt64(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 cmd,
        /* [in] */ Int64 arg,
        /* [out] */ Int32* result);

    static CARAPI FcntlFlock(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 cmd,
        /* [in] */ IStructFlock* arg,
        /* [out] */ Int32* result);

    static CARAPI Fdatasync(
        /* [in] */ IFileDescriptor* fd);

    static CARAPI Fstat(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ IStructStat** stat);

    static CARAPI Fstatvfs(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ IStructStatVfs** statFs);

    static CARAPI Fsync(
        /* [in] */ IFileDescriptor* fd);

    static CARAPI Ftruncate(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int64 length);

    static CARAPI Gai_strerror(
        /* [in] */ Int32 error,
        /* [out] */ String* strerror);

    static CARAPI Getegid(
        /* [out] */ Int32* egid);

    static CARAPI Geteuid(
        /* [out] */ Int32* euid);

    static CARAPI Getgid(
        /* [out] */ Int32* gid);

    static CARAPI Getenv(
        /* [in] */ const String& name,
        /* [out] */ String* env);

    /* TODO: break into getnameinfoHost and getnameinfoService? */
    static CARAPI Getnameinfo(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 flags,
        /* [out] */ String* nameinfo);

    static CARAPI Getpeername(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ ISocketAddress** peername);

    static CARAPI Getpid(
        /* [out] */ Int32* pid);

    static CARAPI Getppid(
        /* [out] */ Int32* ppid);

    static CARAPI Getpwnam(
        /* [in] */ const String& name,
        /* [out] */ IStructPasswd** pwnam);

    static CARAPI Getpwuid(
        /* [in] */ Int32 uid,
        /* [out] */ IStructPasswd** pwuid);

    static CARAPI Getsockname(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ ISocketAddress** sockname);

    static CARAPI GetsockoptByte(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 level,
        /* [in] */ Int32 option,
        /* [out] */ Int32* sockopt);

    static CARAPI GetsockoptInAddr(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 level,
        /* [in] */ Int32 option,
        /* [out] */ IInetAddress** addr);

    static CARAPI GetsockoptInt32(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 level,
        /* [in] */ Int32 option,
        /* [out] */ Int32* sockopt);

    static CARAPI GetsockoptLinger(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 level,
        /* [in] */ Int32 option,
        /* [out] */ IStructLinger** linger);

    static CARAPI GetsockoptTimeval(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 level,
        /* [in] */ Int32 option,
        /* [out] */ IStructTimeval** timeval);

    static CARAPI GetsockoptUcred(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 level,
        /* [in] */ Int32 option,
        /* [out] */ IStructUcred** ucred);

    static CARAPI Gettid(
        /* [out] */ Int32* tid);

    static CARAPI Getuid(
        /* [out] */ Int32* uid);

    static CARAPI If_indextoname(
        /* [in] */ Int32 index,
        /* [out] */ String* name);

    static CARAPI Inet_pton(
        /* [in] */ Int32 family,
        /* [in] */ const String& address,
        /* [out] */ IInetAddress** addr);

    static CARAPI IoctlInetAddress(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 cmd,
        /* [in] */ const String& interfaceName,
        /* [out] */ IInetAddress** addr);

    static CARAPI IoctlInt(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 cmd,
        /* [out] */ Int32* arg,
        /* [out] */ Int32* result);

    static CARAPI Isatty(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ Boolean* isatty);

    static CARAPI Kill(
        /* [in] */ Int32 pid,
        /* [in] */ Int32 signal);

    static CARAPI Lchown(
        /* [in] */ const String& path,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 gid);

    static CARAPI Link(
        /* [in] */ const String& oldPath,
        /* [in] */ const String& newPath);

    static CARAPI Listen(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 backlog);

    static CARAPI Lseek(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int64 offset,
        /* [in] */ Int32 whence,
        /* [out] */ Int64* result);

    static CARAPI Lstat(
        /* [in] */ const String& path,
        /* [out] */ IStructStat** stat);

    static CARAPI Mincore(
        /* [in] */ Int64 address,
        /* [in] */ Int64 byteCount,
        /* [in] */ ArrayOf<Byte>* vector);

    static CARAPI Mkdir(
        /* [in] */ const String& path,
        /* [in] */ Int32 mode);

    static CARAPI Mlock(
        /* [in] */ Int64 address,
        /* [in] */ Int64 byteCount);

    static CARAPI Mkfifo(
        /* [in] */ const String& path,
        /* [in] */ Int32 mode);

    static CARAPI Mmap(
        /* [in] */ Int64 address,
        /* [in] */ Int64 byteCount,
        /* [in] */ Int32 prot,
        /* [in] */ Int32 flags,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int64 offset,
        /* [out] */ Int64* result);

    static CARAPI Msync(
        /* [in] */ Int64 address,
        /* [in] */ Int64 byteCount,
        /* [in] */ Int32 flags);

    static CARAPI Munlock(
        /* [in] */ Int64 address,
        /* [in] */ Int64 byteCount);

    static CARAPI Munmap(
        /* [in] */ Int64 address,
        /* [in] */ Int64 byteCount);

    static CARAPI Open(
        /* [in] */ const String& path,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 mode,
        /* [out] */ IFileDescriptor** fd);

    static CARAPI Pipe(
        /* [out, callee] */ ArrayOf<IFileDescriptor*>** fds);

    /* TODO: if we used the non-standard ppoll(2) behind the scenes, we could take a long timeout. */
    static CARAPI Poll(
        /* [in] */ ArrayOf<IStructPollfd*>* fds,
        /* [in] */ Int32 timeoutMs,
        /* [out] */ Int32* result);

    static CARAPI Posix_fallocate(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int64 offset,
        /* [in] */ Int64 length);

    static CARAPI Prctl(
        /* [in] */ Int32 option,
        /* [in] */ Int64 arg2,
        /* [in] */ Int64 arg3,
        /* [in] */ Int64 arg4,
        /* [in] */ Int64 arg5,
        /* [out] */ Int32* prctl);

    static CARAPI Pread(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [in] */ Int64 offset,
        /* [out] */ Int32* num);

    static CARAPI Pread(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Int64 offset,
        /* [out] */ Int32* num);

    static CARAPI Pwrite(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [in] */ Int64 offset,
        /* [out] */ Int32* num);

    static CARAPI Pwrite(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Int64 offset,
        /* [out] */ Int32* num);

    static CARAPI Read(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* num);

    static CARAPI Read(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [out] */ Int32* num);

    static CARAPI Readlink(
        /* [in] */ const String& path,
        /* [out] */ String* link);

    static CARAPI Readv(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<IInterface*>* buffers,
        /* [in] */ ArrayOf<Int32>* offsets,
        /* [in] */ ArrayOf<Int32>* byteCounts,
        /* [out] */ Int32* num);

    static CARAPI Recvfrom(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [in] */ Int32 flags,
        /* [in] */ IInetSocketAddress* srcAddress,
        /* [out] */ Int32* num);

    static CARAPI Recvfrom(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Int32 flags,
        /* [in] */ IInetSocketAddress* srcAddress,
        /* [out] */ Int32* num);

    static CARAPI Remove(
        /* [in] */ const String& path);

    static CARAPI Rename(
        /* [in] */ const String& oldPath,
        /* [in] */ const String& newPath);

    static CARAPI Sendto(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [in] */ Int32 flags,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Int32 port,
        /* [out] */ Int32* result);

    static CARAPI Sendto(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Int32 flags,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Int32 port,
        /* [out] */ Int32* result);

    static CARAPI Sendfile(
        /* [in] */ IFileDescriptor* outFd,
        /* [in] */ IFileDescriptor* inFd,
        /* [out] */ Int64* inOffset,
        /* [in] */ Int64 byteCount,
        /* [out] */ Int64* result);

    static CARAPI Setegid(
        /* [in] */ Int32 egid);

    static CARAPI Setenv(
        /* [in] */ const String& name,
        /* [in] */ const String& value,
        /* [in] */ Boolean overwrite);

    static CARAPI Seteuid(
        /* [in] */ Int32 euid);

    static CARAPI Setgid(
        /* [in] */ Int32 gid);

    static CARAPI Setsid(
        /* [out] */ Int32* sid);

    static CARAPI SetsockoptByte(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 level,
        /* [in] */ Int32 option,
        /* [in] */ Int32 value);

    static CARAPI SetsockoptIfreq(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 level,
        /* [in] */ Int32 option,
        /* [in] */ const String& interfaceName);

    static CARAPI SetsockoptInt32(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 level,
        /* [in] */ Int32 option,
        /* [in] */ Int32 value);

    static CARAPI SetsockoptIpMreqn(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 level,
        /* [in] */ Int32 option,
        /* [in] */ Int32 value);

    static CARAPI SetsockoptGroupReq(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 level,
        /* [in] */ Int32 option,
        /* [in] */ IStructGroupReq* value);

    static CARAPI SetsockoptGroupSourceReq(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 level,
        /* [in] */ Int32 option,
        /* [in] */ IStructGroupSourceReq* value);

    static CARAPI SetsockoptLinger(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 level,
        /* [in] */ Int32 option,
        /* [in] */ IStructLinger* value);

    static CARAPI SetsockoptTimeval(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 level,
        /* [in] */ Int32 option,
        /* [in] */ IStructTimeval* value);

    static CARAPI Setuid(
        /* [in] */ Int32 uid);

    static CARAPI Shutdown(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 how);

    static CARAPI Socket(
        /* [in] */ Int32 socketDomain,
        /* [in] */ Int32 type,
        /* [in] */ Int32 protocol,
        /* [out] */ IFileDescriptor** fd);

    static CARAPI Socketpair(
        /* [in] */ Int32 socketDomain,
        /* [in] */ Int32 type,
        /* [in] */ Int32 protocol,
        /* [in] */ IFileDescriptor* fd1,
        /* [in] */ IFileDescriptor* fd2);

    static CARAPI Stat(
        /* [in] */ const String& path,
        /* [out] */ IStructStat** stat);

    /* TODO: replace statfs with statvfs. */
    static CARAPI StatVfs(
        /* [in] */ const String& path,
        /* [out] */ IStructStatVfs** statfs);

    static CARAPI Strerror(
        /* [in] */ Int32 errno,
        /* [out] */ String* strerr);

    static CARAPI Symlink(
        /* [in] */ const String& oldPath,
        /* [in] */ const String& newPath);

    static CARAPI Sysconf(
        /* [in] */ Int32 name,
        /* [out] */ Int64* result);

    static CARAPI Tcdrain(
        /* [in] */ IFileDescriptor* fd);

    static CARAPI Umask(
        /* [in] */ Int32 mask,
        /* [out] */ Int32* result);

    static CARAPI Uname(
        /* [out] */ IStructUtsname** uname);

    static CARAPI Unsetenv(
        /* [in] */ const String& name);

    static CARAPI Waitpid(
        /* [in] */ Int32 pid,
        /* [out] */ Int32* status,
        /* [in] */ Int32 options,
        /* [out] */ Int32* result);

    static CARAPI Write(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* num);

    static CARAPI Write(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [out] */ Int32* num);

    static CARAPI Writev(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<IInterface*>* buffers,
        /* [in] */ ArrayOf<Int32>* offsets,
        /* [in] */ ArrayOf<Int32>* byteCounts,
        /* [out] */ Int32* result);

private:
    ECO_LOCAL Os();
    ECO_LOCAL Os(const Os &);
};

}// namespace System
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEM_OS_H__