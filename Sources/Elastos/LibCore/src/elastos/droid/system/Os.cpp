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

#include "Os.h"
#include "CLibcore.h"

using Libcore::IO::CLibcore;

namespace Elastos {
namespace Droid {
namespace System {

ECode Os::Accept(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetSocketAddress* peerAddress,
    /* [out] */ IFileDescriptor** retFd)
{
    return CLibcore::sOs->Accept(fd, peerAddress, retFd);
}

ECode Os::Access(
    /* [in] */ const String& path,
    /* [in] */ Int32 mode,
    /* [out] */ Boolean* succeed)
{
    return CLibcore::sOs->Access(path, mode, succeed);
}

ECode Os::Elastos_getaddrinfo(
    /* [in] */ const String& node,
    /* [in] */ IStructAddrinfo* hints,
    /* [in] */ Int32 netId,
    /* [out, callee] */ ArrayOf<IInetAddress*>** info)
{
    return CLibcore::sOs->Elastos_getaddrinfo(node, hints, netId, info);
}

ECode Os::Bind(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port)
{
    return CLibcore::sOs->Bind(fd, address, port);
}

ECode Os::Chmod(
    /* [in] */ const String& path,
    /* [in] */ Int32 mode)
{
    return CLibcore::sOs->Chmod(path, mode);
}

ECode Os::Chown(
    /* [in] */ const String& path,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid)
{
    return CLibcore::sOs->Chown(path, uid, gid);
}

ECode Os::Close(
    /* [in] */ IFileDescriptor* fd)
{
    return CLibcore::sOs->Close(fd);
}

ECode Os::Connect(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port)
{
    return CLibcore::sOs->Connect(fd, address, port);
}

ECode Os::Dup(
    /* [in] */ IFileDescriptor* oldFd,
    /* [out] */ IFileDescriptor** retFd)
{
    return CLibcore::sOs->Dup(oldFd, retFd);
}

ECode Os::Dup2(
    /* [in] */ IFileDescriptor* oldFd,
    /* [in] */ Int32 newFd,
    /* [out] */ IFileDescriptor** retFd)
{
    return CLibcore::sOs->Dup2(oldFd, newFd, retFd);
}

ECode Os::Environ(
    /* [out, callee] */ ArrayOf<String>** env)
{
    return CLibcore::sOs->Environ(env);
}

ECode Os::Execv(
    /* [in] */ const String& filename,
    /* [in] */ ArrayOf<String>* argv)
{
    return CLibcore::sOs->Execv(filename, argv);
}

ECode Os::Execve(
    /* [in] */ const String& filename,
    /* [in] */ ArrayOf<String>* argv,
    /* [in] */ ArrayOf<String>* envp)
{
    return CLibcore::sOs->Execve(filename, argv, envp);
}

ECode Os::Fchmod(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 mode)
{
    return CLibcore::sOs->Fchmod(fd, mode);
}

ECode Os::Fchown(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid)
{
    return CLibcore::sOs->Fchown(fd, uid, gid);
}

ECode Os::FcntlVoid(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 cmd,
    /* [out] */ Int32* result)
{
    return CLibcore::sOs->FcntlVoid(fd, cmd, result);
}

ECode Os::FcntlInt64(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 cmd,
    /* [in] */ Int64 arg,
    /* [out] */ Int32* result)
{
    return CLibcore::sOs->FcntlInt64(fd, cmd, arg, result);
}

ECode Os::FcntlFlock(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 cmd,
    /* [in] */ IStructFlock* arg,
    /* [out] */ Int32* result)
{
    return CLibcore::sOs->FcntlFlock(fd, cmd, arg, result);
}

ECode Os::Fdatasync(
    /* [in] */ IFileDescriptor* fd)
{
    return CLibcore::sOs->Fdatasync(fd);
}

ECode Os::Fstat(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IStructStat** stat)
{
    return CLibcore::sOs->Fstat(fd, stat);
}

ECode Os::Fstatvfs(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IStructStatVfs** statFs)
{
    return CLibcore::sOs->Fstatvfs(fd, statFs);
}

ECode Os::Fsync(
    /* [in] */ IFileDescriptor* fd)
{
    return CLibcore::sOs->Fsync(fd);
}

ECode Os::Ftruncate(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 length)
{
    return CLibcore::sOs->Ftruncate(fd, length);
}

ECode Os::Gai_strerror(
    /* [in] */ Int32 error,
    /* [out] */ String* strerror)
{
    return CLibcore::sOs->Gai_strerror(error, strerror);
}

ECode Os::Getegid(
    /* [out] */ Int32* egid)
{
    return CLibcore::sOs->Getegid(egid);
}

ECode Os::Geteuid(
    /* [out] */ Int32* euid)
{
    return CLibcore::sOs->Geteuid(euid);
}

ECode Os::Getgid(
    /* [out] */ Int32* gid)
{
    return CLibcore::sOs->Getgid(gid);
}

ECode Os::Getenv(
    /* [in] */ const String& name,
    /* [out] */ String* env)
{
    return CLibcore::sOs->Getenv(name, env);
}

/* TODO: break into getnameinfoHost and getnameinfoService? */
ECode Os::Getnameinfo(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 flags,
    /* [out] */ String* nameinfo)
{
    return CLibcore::sOs->Getnameinfo(address, flags, nameinfo);
}

ECode Os::Getpeername(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ ISocketAddress** peername)
{
    return CLibcore::sOs->Getpeername(fd, peername);
}

ECode Os::Getpid(
    /* [out] */ Int32* pid)
{
    return CLibcore::sOs->Getpid(pid);
}

ECode Os::Getppid(
    /* [out] */ Int32* ppid)
{
    return CLibcore::sOs->Getppid(ppid);
}

ECode Os::Getpwnam(
    /* [in] */ const String& name,
    /* [out] */ IStructPasswd** pwnam)
{
    return CLibcore::sOs->Getpwnam(name, pwnam);
}

ECode Os::Getpwuid(
    /* [in] */ Int32 uid,
    /* [out] */ IStructPasswd** pwuid)
{
    return CLibcore::sOs->Getpwuid(uid, pwuid);
}

ECode Os::Getsockname(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ ISocketAddress** sockname)
{
    return CLibcore::sOs->Getsockname(fd, sockname);
}

ECode Os::GetsockoptByte(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [out] */ Int32* sockopt)
{
    return CLibcore::sOs->GetsockoptByte(fd, level, option, sockopt);
}

ECode Os::GetsockoptInAddr(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [out] */ IInetAddress** addr)
{
    return CLibcore::sOs->GetsockoptInAddr(fd, level, option, addr);
}

ECode Os::GetsockoptInt32(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [out] */ Int32* sockopt)
{
    return CLibcore::sOs->GetsockoptInt32(fd, level, option, sockopt);
}

ECode Os::GetsockoptLinger(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [out] */ IStructLinger** linger)
{
    return CLibcore::sOs->GetsockoptLinger(fd, level, option, linger);
}

ECode Os::GetsockoptTimeval(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [out] */ IStructTimeval** timeval)
{
    return CLibcore::sOs->GetsockoptTimeval(fd, level, option, timeval);
}

ECode Os::GetsockoptUcred(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [out] */ IStructUcred** ucred)
{
    return CLibcore::sOs->GetsockoptUcred(fd, level, option, ucred);
}

ECode Os::Gettid(
    /* [out] */ Int32* tid)
{
    return CLibcore::sOs->Gettid(tid);
}

ECode Os::Getuid(
    /* [out] */ Int32* uid)
{
    return CLibcore::sOs->Getuid(uid);
}

ECode Os::If_indextoname(
    /* [in] */ Int32 index,
    /* [out] */ String* name)
{
    return CLibcore::sOs->If_indextoname(index, name);
}

ECode Os::Inet_pton(
    /* [in] */ Int32 family,
    /* [in] */ const String& address,
    /* [out] */ IInetAddress** addr)
{
    return CLibcore::sOs->Inet_pton(family, address, addr);
}

ECode Os::IoctlInetAddress(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 cmd,
    /* [in] */ const String& interfaceName,
    /* [out] */ IInetAddress** addr)
{
    return CLibcore::sOs->IoctlInetAddress(fd, cmd, interfaceName, addr);
}

ECode Os::IoctlInt(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 cmd,
    /* [out] */ Int32* arg,
    /* [out] */ Int32* result)
{
    return CLibcore::sOs->IoctlInt(fd, cmd, arg, result);
}

ECode Os::Isatty(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Boolean* isatty)
{
    return CLibcore::sOs->Isatty(fd, isatty);
}

ECode Os::Kill(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 signal)
{
    return CLibcore::sOs->Kill(pid, signal);
}

ECode Os::Lchown(
    /* [in] */ const String& path,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid)
{
    return CLibcore::sOs->Lchown(path, uid, gid);
}

ECode Os::Link(
    /* [in] */ const String& oldPath,
    /* [in] */ const String& newPath)
{
    return CLibcore::sOs->Link(oldPath, newPath);
}

ECode Os::Listen(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 backlog)
{
    return CLibcore::sOs->Listen(fd, backlog);
}

ECode Os::Lseek(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int32 whence,
    /* [out] */ Int64* result)
{
    return CLibcore::sOs->Lseek(fd, offset, whence, result);
}

ECode Os::Lstat(
    /* [in] */ const String& path,
    /* [out] */ IStructStat** stat)
{
    return CLibcore::sOs->Lstat(path, stat);
}

ECode Os::Mincore(
    /* [in] */ Int64 address,
    /* [in] */ Int64 byteCount,
    /* [in] */ ArrayOf<Byte>* vector)
{
    return CLibcore::sOs->Mincore(address, byteCount, vector);
}

ECode Os::Mkdir(
    /* [in] */ const String& path,
    /* [in] */ Int32 mode)
{
    return CLibcore::sOs->Mkdir(path, mode);
}

ECode Os::Mlock(
    /* [in] */ Int64 address,
    /* [in] */ Int64 byteCount)
{
    return CLibcore::sOs->Mlock(address, byteCount);
}

ECode Os::Mkfifo(
    /* [in] */ const String& path,
    /* [in] */ Int32 mode)
{
    return CLibcore::sOs->Mkfifo(path, mode);
}

ECode Os::Mmap(
    /* [in] */ Int64 address,
    /* [in] */ Int64 byteCount,
    /* [in] */ Int32 prot,
    /* [in] */ Int32 flags,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [out] */ Int64* result)
{
    return CLibcore::sOs->Mmap(address, byteCount, prot, flags, fd, offset, result);
}

ECode Os::Msync(
    /* [in] */ Int64 address,
    /* [in] */ Int64 byteCount,
    /* [in] */ Int32 flags)
{
    return CLibcore::sOs->Msync(address, byteCount, flags);
}

ECode Os::Munlock(
    /* [in] */ Int64 address,
    /* [in] */ Int64 byteCount)
{
    return CLibcore::sOs->Munlock(address, byteCount);
}

ECode Os::Munmap(
    /* [in] */ Int64 address,
    /* [in] */ Int64 byteCount)
{
    return CLibcore::sOs->Munmap(address, byteCount);
}

ECode Os::Open(
    /* [in] */ const String& path,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 mode,
    /* [out] */ IFileDescriptor** fd)
{
    return CLibcore::sOs->Open(path, flags, mode, fd);
}

ECode Os::Pipe(
    /* [out, callee] */ ArrayOf<IFileDescriptor*>** fds)
{
    return CLibcore::sOs->Pipe(fds);
}

/* TODO: if we used the non-standard ppoll(2) behind the scenes, we could take a long timeout. */
ECode Os::Poll(
    /* [in] */ ArrayOf<IStructPollfd*>* fds,
    /* [in] */ Int32 timeoutMs,
    /* [out] */ Int32* result)
{
    return CLibcore::sOs->Poll(fds, timeoutMs, result);
}

ECode Os::Posix_fallocate(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length)
{
    return CLibcore::sOs->Posix_fallocate(fd, offset, length);
}

ECode Os::Prctl(
    /* [in] */ Int32 option,
    /* [in] */ Int64 arg2,
    /* [in] */ Int64 arg3,
    /* [in] */ Int64 arg4,
    /* [in] */ Int64 arg5,
    /* [out] */ Int32* prctl)
{
    return CLibcore::sOs->Prctl(option, arg2, arg3, arg4, arg5, prctl);
}

ECode Os::Pread(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int64 offset,
    /* [out] */ Int32* num)
{
    return CLibcore::sOs->Pread(fd, bytes, byteOffset, byteCount, offset, num);
}

ECode Os::Pread(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int64 offset,
    /* [out] */ Int32* num)
{
    return CLibcore::sOs->Pread(fd, buffer, offset, num);
}

ECode Os::Pwrite(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int64 offset,
    /* [out] */ Int32* num)
{
    return CLibcore::sOs->Pwrite(fd, bytes, byteOffset, byteCount, offset, num);
}

ECode Os::Pwrite(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int64 offset,
    /* [out] */ Int32* num)
{
    return CLibcore::sOs->Pwrite(fd, buffer, offset, num);
}

ECode Os::Read(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* num)
{
    return CLibcore::sOs->Read(fd, bytes, byteOffset, byteCount, num);
}

ECode Os::Read(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ Int32* num)
{
    return CLibcore::sOs->Read(fd, buffer, num);
}

ECode Os::Readlink(
    /* [in] */ const String& path,
    /* [out] */ String* link)
{
    return CLibcore::sOs->Readlink(path, link);
}

ECode Os::Readv(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<IInterface*>* buffers,
    /* [in] */ ArrayOf<Int32>* offsets,
    /* [in] */ ArrayOf<Int32>* byteCounts,
    /* [out] */ Int32* num)
{
    return CLibcore::sOs->Readv(fd, buffers, offsets, byteCounts, num);
}

ECode Os::Recvfrom(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int32 flags,
    /* [in] */ IInetSocketAddress* srcAddress,
    /* [out] */ Int32* num)
{
    return CLibcore::sOs->Recvfrom(fd, bytes, byteOffset, byteCount, flags, srcAddress, num);
}

ECode Os::Recvfrom(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 flags,
    /* [in] */ IInetSocketAddress* srcAddress,
    /* [out] */ Int32* num)
{
    return CLibcore::sOs->Recvfrom(fd, buffer, flags, srcAddress, num);
}

ECode Os::Remove(
    /* [in] */ const String& path)
{
    return CLibcore::sOs->Remove(path);
}

ECode Os::Rename(
    /* [in] */ const String& oldPath,
    /* [in] */ const String& newPath)
{
    return CLibcore::sOs->Rename(oldPath, newPath);
}

ECode Os::Sendto(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int32 flags,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [out] */ Int32* result)
{
    return CLibcore::sOs->Sendto(fd, bytes, byteOffset, byteCount, flags, inetAddress, port, result);
}

ECode Os::Sendto(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 flags,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [out] */ Int32* result)
{
    return CLibcore::sOs->Sendto(fd, buffer, flags, inetAddress, port, result);
}

ECode Os::Sendfile(
    /* [in] */ IFileDescriptor* outFd,
    /* [in] */ IFileDescriptor* inFd,
    /* [out] */ Int64* inOffset,
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* result)
{
    return CLibcore::sOs->Sendfile(outFd, inFd, inOffset, byteCount, result);
}

ECode Os::Setegid(
    /* [in] */ Int32 egid)
{
    return CLibcore::sOs->Setegid(egid);
}

ECode Os::Setenv(
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [in] */ Boolean overwrite)
{
    return CLibcore::sOs->Setenv(name, value, overwrite);
}

ECode Os::Seteuid(
    /* [in] */ Int32 euid)
{
    return CLibcore::sOs->Seteuid(euid);
}

ECode Os::Setgid(
    /* [in] */ Int32 gid)
{
    return CLibcore::sOs->Setgid(gid);
}

ECode Os::Setsid(
    /* [out] */ Int32* sid)
{
    return CLibcore::sOs->Setsid(sid);
}

ECode Os::SetsockoptByte(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ Int32 value)
{
    return CLibcore::sOs->SetsockoptByte(fd, level, option, value);
}

ECode Os::SetsockoptIfreq(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ const String& interfaceName)
{
    return CLibcore::sOs->SetsockoptIfreq(fd, level, option, interfaceName);
}

ECode Os::SetsockoptInt32(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ Int32 value)
{
    return CLibcore::sOs->SetsockoptInt32(fd, level, option, value);
}

ECode Os::SetsockoptIpMreqn(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ Int32 value)
{
    return CLibcore::sOs->SetsockoptIpMreqn(fd, level, option, value);
}

ECode Os::SetsockoptGroupReq(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ IStructGroupReq* value)
{
    return CLibcore::sOs->SetsockoptGroupReq(fd, level, option, value);
}

ECode Os::SetsockoptGroupSourceReq(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ IStructGroupSourceReq* value)
{
    return CLibcore::sOs->SetsockoptGroupSourceReq(fd, level, option, value);
}

ECode Os::SetsockoptLinger(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ IStructLinger* value)
{
    return CLibcore::sOs->SetsockoptLinger(fd, level, option, value);
}

ECode Os::SetsockoptTimeval(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ IStructTimeval* value)
{
    return CLibcore::sOs->SetsockoptTimeval(fd, level, option, value);
}

ECode Os::Setuid(
    /* [in] */ Int32 uid)
{
    return CLibcore::sOs->Setuid(uid);
}

ECode Os::Shutdown(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 how)
{
    return CLibcore::sOs->Shutdown(fd, how);
}

ECode Os::Socket(
    /* [in] */ Int32 socketDomain,
    /* [in] */ Int32 type,
    /* [in] */ Int32 protocol,
    /* [out] */ IFileDescriptor** fd)
{
    return CLibcore::sOs->Socket(socketDomain, type, protocol, fd);
}

ECode Os::Socketpair(
    /* [in] */ Int32 socketDomain,
    /* [in] */ Int32 type,
    /* [in] */ Int32 protocol,
    /* [in] */ IFileDescriptor* fd1,
    /* [in] */ IFileDescriptor* fd2)
{
    return CLibcore::sOs->Socketpair(socketDomain, type, protocol, fd1, fd2);
}

ECode Os::Stat(
    /* [in] */ const String& path,
    /* [out] */ IStructStat** stat)
{
    return CLibcore::sOs->Stat(path, stat);
}

/* TODO: replace statfs with statvfs. */
ECode Os::StatVfs(
    /* [in] */ const String& path,
    /* [out] */ IStructStatVfs** statfs)
{
    return CLibcore::sOs->StatVfs(path, statfs);
}

ECode Os::Strerror(
    /* [in] */ Int32 errno,
    /* [out] */ String* strerr)
{
    return CLibcore::sOs->Strerror(errno, strerr);
}

ECode Os::Symlink(
    /* [in] */ const String& oldPath,
    /* [in] */ const String& newPath)
{
    return CLibcore::sOs->Symlink(oldPath, newPath);
}

ECode Os::Sysconf(
    /* [in] */ Int32 name,
    /* [out] */ Int64* result)
{
    return CLibcore::sOs->Sysconf(name, result);
}

ECode Os::Tcdrain(
    /* [in] */ IFileDescriptor* fd)
{
    return CLibcore::sOs->Tcdrain(fd);
}

ECode Os::Umask(
    /* [in] */ Int32 mask,
    /* [out] */ Int32* result)
{
    return CLibcore::sOs->Umask(mask, result);
}

ECode Os::Uname(
    /* [out] */ IStructUtsname** uname)
{
    return CLibcore::sOs->Uname(uname);
}

ECode Os::Unsetenv(
    /* [in] */ const String& name)
{
    return CLibcore::sOs->Unsetenv(name);
}

ECode Os::Waitpid(
    /* [in] */ Int32 pid,
    /* [out] */ Int32* status,
    /* [in] */ Int32 options,
    /* [out] */ Int32* result)
{
    return CLibcore::sOs->Waitpid(pid, status, options, result);
}

ECode Os::Write(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* num)
{
    return CLibcore::sOs->Write(fd, bytes, byteOffset, byteCount, num);
}

ECode Os::Write(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ Int32* num)
{
    return CLibcore::sOs->Write(fd, buffer, num);
}

ECode Os::Writev(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<IInterface*>* buffers,
    /* [in] */ ArrayOf<Int32>* offsets,
    /* [in] */ ArrayOf<Int32>* byteCounts,
    /* [out] */ Int32* result)
{
    return CLibcore::sOs->Writev(fd, buffers, offsets, byteCounts, result);
}

}// namespace System
}// namespace Droid
}// namespace Elastos