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

#include "ForwardingOs.h"

namespace Libcore {
namespace IO {

// {216d5978-6282-44ec-aad1-ebe79d89e532}
extern const _ELASTOS ClassID ECLSID_ForwardingOs = {
    { 0x216d5978, 0x6282, 0x44ec, { 0xaa, 0xd1, 0xeb, 0xe7, 0x9d, 0x89, 0xe5, 0x32 }},
    (char *)c_pElastos_CoreLibraryUunm,
    0x2d2b67d7 };

CAR_OBJECT_IMPL(ForwardingOs)

CAR_INTERFACE_IMPL(ForwardingOs, Object, IOs)

ForwardingOs::ForwardingOs(
    /* [in] */ IOs* os)
    : mOs(os)
{}


ECode ForwardingOs::Accept(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetSocketAddress* peerAddress,
    /* [out] */ IFileDescriptor** retFd)
{
    VALIDATE_NOT_NULL(retFd);

    return mOs->Accept(fd, peerAddress, retFd);
}

ECode ForwardingOs::Access(
    /* [in] */ const String& path,
    /* [in] */ Int32 mode,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    return mOs->Access(path, mode, succeeded);
}

ECode ForwardingOs::Bind(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port)
{
    return mOs->Bind(fd, address, port);
}

ECode ForwardingOs::Chmod(
    /* [in] */ const String& path,
    /* [in] */ Int32 mode)
{
    return mOs->Chmod(path, mode);
}

ECode ForwardingOs::Chown(
    /* [in] */ const String& path,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid)
{
    return mOs->Chown(path, uid, gid);
}

ECode ForwardingOs::Close(
    /* [in] */ IFileDescriptor* fd)
{
    return mOs->Close(fd);
}

ECode ForwardingOs::Connect(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port)
{
    return mOs->Connect(fd, address, port);
}

ECode ForwardingOs::Dup(
    /* [in] */ IFileDescriptor* oldFd,
    /* [out] */ IFileDescriptor** retFd)
{
    VALIDATE_NOT_NULL(retFd);

    return mOs->Dup(oldFd, retFd);
}

ECode ForwardingOs::Dup2(
    /* [in] */ IFileDescriptor* oldFd,
    /* [in] */ Int32 newFd,
    /* [out] */ IFileDescriptor** retFd)
{
    VALIDATE_NOT_NULL(retFd);

    return mOs->Dup2(oldFd, newFd, retFd);
}

ECode ForwardingOs::Environ(
    /* [out, callee] */ ArrayOf<String>** env)
{
    return mOs->Environ(env);
}

ECode ForwardingOs::Fchmod(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 mode)
{
    return mOs->Fchmod(fd, mode);
}

ECode ForwardingOs::Fchown(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid)
{
    return mOs->Fchown(fd, uid, gid);
}

ECode ForwardingOs::FcntlVoid(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 cmd,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    return mOs->FcntlVoid(fd, cmd, result);
}

ECode ForwardingOs::FcntlInt64(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 cmd,
    /* [in] */ Int64 arg,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    return mOs->FcntlInt64(fd, cmd, arg, result);
}

ECode ForwardingOs::FcntlFlock(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 cmd,
    /* [in] */ IStructFlock* arg,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    return mOs->FcntlFlock(fd, cmd, arg, result);
}

ECode ForwardingOs::Fdatasync(
    /* [in] */ IFileDescriptor* fd)
{
    return mOs->Fdatasync(fd);
}

ECode ForwardingOs::Fstat(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IStructStat** stat)
{
    VALIDATE_NOT_NULL(stat);

    return mOs->Fstat(fd, stat);
}

ECode ForwardingOs::Fstatvfs(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IStructStatVfs** statVfs)
{
    VALIDATE_NOT_NULL(statVfs);

    return mOs->Fstatvfs(fd, statVfs);
}

ECode ForwardingOs::Fsync(
    /* [in] */ IFileDescriptor* fd)
{
    return mOs->Fsync(fd);
}

ECode ForwardingOs::Ftruncate(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 length)
{
    return mOs->Ftruncate(fd, length);
}

ECode ForwardingOs::Gai_strerror(
    /* [in] */ Int32 error,
    /* [out] */ String* strerror)
{
    VALIDATE_NOT_NULL(strerror);

    return mOs->Gai_strerror(error, strerror);
}

ECode ForwardingOs::Elastos_getaddrinfo(
    /* [in] */ const String& node,
    /* [in] */ IStructAddrinfo* hints,
    /* [in] */ Int32 netId,
    /* [out, callee] */ ArrayOf<IInetAddress*>** addrinfo)
{
    return mOs->Elastos_getaddrinfo(node, hints, netId, addrinfo);
}

ECode ForwardingOs::Getegid(
    /* [out] */ Int32* egid)
{
    VALIDATE_NOT_NULL(egid);

    return mOs->Getegid(egid);
}

ECode ForwardingOs::Geteuid(
    /* [out] */ Int32* eEuid)
{
    VALIDATE_NOT_NULL(eEuid);

    return mOs->Geteuid(eEuid);
}

ECode ForwardingOs::Getgid(
    /* [out] */ Int32* gid)
{
    VALIDATE_NOT_NULL(gid);

    return mOs->Getgid(gid);
}

ECode ForwardingOs::Getenv(
    /* [in] */ const String& name,
    /* [out] */ String* env)
{
    VALIDATE_NOT_NULL(env);

    return mOs->Getenv(name, env);
}

ECode ForwardingOs::Getnameinfo(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 flags,
    /* [out] */ String* nameinfo)
{
    VALIDATE_NOT_NULL(nameinfo);

    return mOs->Getnameinfo(address, flags, nameinfo);
}

ECode ForwardingOs::Getpid(
    /* [out] */ Int32* pid)
{
    VALIDATE_NOT_NULL(pid);

    return mOs->Getpid(pid);
}

ECode ForwardingOs::Getppid(
    /* [out] */ Int32* ppid)
{
    VALIDATE_NOT_NULL(ppid);

    return mOs->Getppid(ppid);
}

ECode ForwardingOs::Getpwnam(
    /* [in] */ const String& name,
    /* [out] */ IStructPasswd** pwnam)
{
    VALIDATE_NOT_NULL(pwnam);

    return mOs->Getpwnam(name, pwnam);
}

ECode ForwardingOs::Getpwuid(
    /* [in] */ Int32 uid,
    /* [out] */ IStructPasswd** pwuid)
{
    VALIDATE_NOT_NULL(pwuid);

    return mOs->Getpwuid(uid, pwuid);
}

ECode ForwardingOs::Getsockname(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ ISocketAddress** sockname)
{
    VALIDATE_NOT_NULL(sockname);

    return mOs->Getsockname(fd, sockname);
}

ECode ForwardingOs::GetsockoptByte(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [out] */ Int32* sockopt)
{
    VALIDATE_NOT_NULL(sockopt);

    return mOs->GetsockoptByte(fd, level, option, sockopt);
}

ECode ForwardingOs::GetsockoptInAddr(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [out] */ IInetAddress** addr)
{
    VALIDATE_NOT_NULL(addr);

    return mOs->GetsockoptInAddr(fd, level, option, addr);
}

ECode ForwardingOs::GetsockoptInt32(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [out] */ Int32* sockopt)
{
    VALIDATE_NOT_NULL(sockopt);

    return mOs->GetsockoptInt32(fd, level, option, sockopt);
}

ECode ForwardingOs::GetsockoptLinger(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [out] */ IStructLinger** linger)
{
    VALIDATE_NOT_NULL(linger);

    return mOs->GetsockoptLinger(fd, level, option, linger);
}

ECode ForwardingOs::GetsockoptTimeval(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [out] */ IStructTimeval** timeval)
{
    VALIDATE_NOT_NULL(timeval);

    return mOs->GetsockoptTimeval(fd, level, option, timeval);
}

ECode ForwardingOs::Getuid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid);

    return mOs->Getuid(uid);
}

ECode ForwardingOs::If_indextoname(
    /* [in] */ Int32 index,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    return mOs->If_indextoname(index, name);
}

ECode ForwardingOs::Inet_pton(
    /* [in] */ Int32 family,
    /* [in] */ const String& address,
    /* [out] */ IInetAddress** addr)
{
    VALIDATE_NOT_NULL(addr);

    return mOs->Inet_pton(family, address, addr);
}

ECode ForwardingOs::IoctlInetAddress(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 cmd,
    /* [in] */ const String& interfaceName,
    /* [out] */ IInetAddress** addr)
{
    VALIDATE_NOT_NULL(addr);

    return mOs->IoctlInetAddress(fd, cmd, interfaceName, addr);
}

ECode ForwardingOs::IoctlInt(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 cmd,
    /* [out] */ Int32* arg,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    return mOs->IoctlInt(fd, cmd, arg, result);
}

ECode ForwardingOs::Isatty(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Boolean* isatty)
{
    VALIDATE_NOT_NULL(isatty);

    return mOs->Isatty(fd, isatty);
}

ECode ForwardingOs::Kill(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 signal)
{
    return mOs->Kill(pid, signal);
}

ECode ForwardingOs::Lchown(
    /* [in] */ const String& path,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid)
{
    return mOs->Lchown(path, uid, gid);
}

ECode ForwardingOs::Listen(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 backlog)
{
    return mOs->Listen(fd, backlog);
}

ECode ForwardingOs::Lseek(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int32 whence,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    return mOs->Lseek(fd, offset, whence, result);
}

ECode ForwardingOs::Lstat(
    /* [in] */ const String& path,
    /* [out] */ IStructStat** stat)
{
    VALIDATE_NOT_NULL(stat);

    return mOs->Lstat(path, stat);
}

ECode ForwardingOs::Mincore(
    /* [in] */ Int64 address,
    /* [in] */ Int64 byteCount,
    /* [in] */ ArrayOf<Byte>* vector)
{
    return mOs->Mincore(address, byteCount, vector);
}

ECode ForwardingOs::Mkdir(
    /* [in] */ const String& path,
    /* [in] */ Int32 mode)
{
    return mOs->Mkdir(path, mode);
}

ECode ForwardingOs::Mlock(
    /* [in] */ Int64 address,
    /* [in] */ Int64 byteCount)
{
    return mOs->Mlock(address, byteCount);
}

ECode ForwardingOs::Mmap(
    /* [in] */ Int64 address,
    /* [in] */ Int64 byteCount,
    /* [in] */ Int32 prot,
    /* [in] */ Int32 flags,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    return mOs->Mmap(address, byteCount, prot, flags, fd, offset, result);
}

ECode ForwardingOs::Msync(
    /* [in] */ Int64 address,
    /* [in] */ Int64 byteCount,
    /* [in] */ Int32 flags)
{
    return mOs->Msync(address, byteCount, flags);
}

ECode ForwardingOs::Munlock(
    /* [in] */ Int64 address,
    /* [in] */ Int64 byteCount)
{
    return mOs->Munlock(address, byteCount);
}

ECode ForwardingOs::Munmap(
    /* [in] */ Int64 address,
    /* [in] */ Int64 byteCount)
{
    return mOs->Munmap(address, byteCount);
}

ECode ForwardingOs::Open(
    /* [in] */ const String& path,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 mode,
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd);

    return mOs->Open(path, flags, mode, fd);
}

ECode ForwardingOs::Pipe(
    /* [out, callee] */ ArrayOf<IFileDescriptor*>** fds)
{
    return mOs->Pipe(fds);
}

ECode ForwardingOs::Poll(
    /* [in] */ ArrayOf<IStructPollfd*>* fdStructs,
    /* [in] */ Int32 timeoutMs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    return mOs->Poll(fdStructs, timeoutMs, result);
}

ECode ForwardingOs::Pread(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int64 offset,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(bytes);
    VALIDATE_NOT_NULL(num);

    return mOs->Pread(fd, bytes, byteOffset, byteCount, offset, num);
}

ECode ForwardingOs::Pwrite(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int64 offset,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);

    return mOs->Pwrite(fd, bytes, byteOffset, byteCount, offset, num);
}

ECode ForwardingOs::Read(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);

    return mOs->Read(fd, bytes, byteOffset, byteCount, num);
}

ECode ForwardingOs::Recvfrom(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int32 flags,
    /* [in] */ IInetSocketAddress* srcAddress,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(bytes);
    VALIDATE_NOT_NULL(num);

    return mOs->Recvfrom(fd, bytes, byteOffset, byteCount, flags, srcAddress, num);
}

ECode ForwardingOs::Remove(
    /* [in] */ const String& path)
{
    return mOs->Remove(path);
}

ECode ForwardingOs::Rename(
    /* [in] */ const String& oldPath,
    /* [in] */ const String& newPath)
{
    return mOs->Rename(oldPath, newPath);
}

ECode ForwardingOs::Sendto(
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

    return mOs->Sendto(fd, bytes, byteOffset, byteCount, flags, inetAddress, port, result);
}

ECode ForwardingOs::Sendfile(
    /* [in] */ IFileDescriptor* outFd,
    /* [in] */ IFileDescriptor* inFd,
    /* [in, out] */ Int64* inOffset,
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(inOffset);
    VALIDATE_NOT_NULL(result);

    return mOs->Sendfile(outFd, inFd, inOffset, byteCount, result);
}

ECode ForwardingOs::Setegid(
    /* [in] */ Int32 egid)
{
    return mOs->Setegid(egid);
}

ECode ForwardingOs::Seteuid(
    /* [in] */ Int32 euid)
{
    return mOs->Seteuid(euid);
}

ECode ForwardingOs::Setgid(
    /* [in] */ Int32 gid)
{
    return mOs->Setgid(gid);
}

ECode ForwardingOs::Setsid(
    /* [out] */ Int32* sid)
{
    VALIDATE_NOT_NULL(sid);

    return mOs->Setsid(sid);
}

ECode ForwardingOs::SetsockoptByte(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ Int32 value)
{
    return mOs->SetsockoptByte(fd, level, option, value);
}

ECode ForwardingOs::SetsockoptIfreq(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ const String& interfaceName)
{
    return mOs->SetsockoptIfreq(fd, level, option, interfaceName);
}

ECode ForwardingOs::SetsockoptInt32(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ Int32 value)
{
    return mOs->SetsockoptInt32(fd, level, option, value);
}

ECode ForwardingOs::SetsockoptIpMreqn(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ Int32 value)
{
    return mOs->SetsockoptIpMreqn(fd, level, option, value);
}

ECode ForwardingOs::SetsockoptGroupReq(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ IStructGroupReq* value)
{
    return mOs->SetsockoptGroupReq(fd, level, option, value);
}

ECode ForwardingOs::SetsockoptLinger(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ IStructLinger* value)
{
    return mOs->SetsockoptLinger(fd, level, option, value);
}

ECode ForwardingOs::SetsockoptTimeval(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ IStructTimeval* value)
{
    return mOs->SetsockoptTimeval(fd, level, option, value);
}

ECode ForwardingOs::Setuid(
    /* [in] */ Int32 uid)
{
    return mOs->Setuid(uid);
}

ECode ForwardingOs::Shutdown(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 how)
{
    return mOs->Shutdown(fd, how);
}

ECode ForwardingOs::Socket(
    /* [in] */ Int32 socketDomain,
    /* [in] */ Int32 type,
    /* [in] */ Int32 protocol,
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd);

    return mOs->Socket(socketDomain, type, protocol, fd);
}

ECode ForwardingOs::Socketpair(
    /* [in] */ Int32 socketDomain,
    /* [in] */ Int32 type,
    /* [in] */ Int32 protocol,
    /* [in] */ IFileDescriptor* fd1,
    /* [in] */ IFileDescriptor* fd2)
{
    return mOs->Socketpair(socketDomain, type, protocol, fd1, fd2);
}

ECode ForwardingOs::Stat(
    /* [in] */ const String& path,
    /* [out] */ IStructStat** stat)
{
    VALIDATE_NOT_NULL(stat);

    return mOs->Stat(path, stat);
}

ECode ForwardingOs::StatVfs(
    /* [in] */ const String& path,
    /* [out] */ IStructStatVfs** statVfs)
{
    VALIDATE_NOT_NULL(statVfs);

    return mOs->StatVfs(path, statVfs);
}

ECode ForwardingOs::Strerror(
    /* [in] */ Int32 errnum,
    /* [out] */ String* strerr)
{
    VALIDATE_NOT_NULL(strerr);

    return mOs->Strerror(errnum, strerr);
}

ECode ForwardingOs::Symlink(
    /* [in] */ const String& oldPath,
    /* [in] */ const String& newPath)
{
    return mOs->Symlink(oldPath, newPath);
}

ECode ForwardingOs::Sysconf(
    /* [in] */ Int32 name,
    /* [out] */ Int64* result)

{
    VALIDATE_NOT_NULL(result);

    return mOs->Sysconf(name, result);
}

ECode ForwardingOs::Tcdrain(
    /* [in] */ IFileDescriptor* fd)
{
    return mOs->Tcdrain(fd);
}

ECode ForwardingOs::Umask(
    /* [in] */ Int32 mask,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    return mOs->Umask(mask, result);
}

ECode ForwardingOs::Uname(
    /* [out] */ IStructUtsname** utsname)
{
    VALIDATE_NOT_NULL(utsname);

    return mOs->Uname(utsname);
}

ECode ForwardingOs::Unsetenv(
    /* [in] */ const String& name)
{
    return mOs->Unsetenv(name);
}

ECode ForwardingOs::Waitpid(
    /* [in] */ Int32 pid,
    /* [in, out] */ Int32* _status,
    /* [in] */ Int32 options,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(_status);
    VALIDATE_NOT_NULL(result);

    return mOs->Waitpid(pid, _status, options, result);
}

ECode ForwardingOs::Write(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);

    return mOs->Write(fd, bytes, byteOffset, byteCount, num);
}

ECode ForwardingOs::Execv(
    /* [in] */ const String& filename,
    /* [in] */ ArrayOf<String>* argv)
{
    return mOs->Execv(filename, argv);
}

ECode ForwardingOs::Execve(
    /* [in] */ const String& filename,
    /* [in] */ ArrayOf<String>* argv,
    /* [in] */ ArrayOf<String>* envp)
{
    return mOs->Execve(filename, argv, envp);
}

ECode ForwardingOs::Getpeername(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ ISocketAddress** peername)
{
    return mOs->Getpeername(fd, peername);
}

ECode ForwardingOs::GetsockoptUcred(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [out] */ IStructUcred** ucred)
{
    return mOs->GetsockoptUcred(fd, level, option, ucred);
}

ECode ForwardingOs::Gettid(
    /* [out] */ Int32* tid)
{
    return mOs->Gettid(tid);
}

ECode ForwardingOs::Link(
    /* [in] */ const String& oldPath,
    /* [in] */ const String& newPath)
{
    return mOs->Link(oldPath, newPath);
}

ECode ForwardingOs::Mkfifo(
    /* [in] */ const String& path,
    /* [in] */ Int32 mode)
{
    return mOs->Mkfifo(path, mode);
}

ECode ForwardingOs::Posix_fallocate(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length)
{
    return mOs->Posix_fallocate(fd, offset, length);
}

ECode ForwardingOs::Prctl(
    /* [in] */ Int32 option,
    /* [in] */ Int64 arg2,
    /* [in] */ Int64 arg3,
    /* [in] */ Int64 arg4,
    /* [in] */ Int64 arg5,
    /* [out] */ Int32* prctl)
{
    return mOs->Prctl(option, arg2, arg3, arg4, arg5, prctl);
}

ECode ForwardingOs::Pread(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int64 offset,
    /* [out] */ Int32* num)
{
    return mOs->Pread(fd, buffer, offset, num);
}

ECode ForwardingOs::Pwrite(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int64 offset,
    /* [out] */ Int32* num)
{
    return mOs->Pwrite(fd, buffer, offset, num);
}

ECode ForwardingOs::Read(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ Int32* num)
{
    return mOs->Read(fd, buffer, num);
}

ECode ForwardingOs::Readlink(
    /* [in] */ const String& path,
    /* [out] */ String* link)
{
    return mOs->Readlink(path, link);
}

ECode ForwardingOs::Readv(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<IInterface*>* buffers,
    /* [in] */ ArrayOf<Int32>* offsets,
    /* [in] */ ArrayOf<Int32>* byteCounts,
    /* [out] */ Int32* num)
{
    return mOs->Readv(fd,buffers, offsets, byteCounts, num);
}

ECode ForwardingOs::Recvfrom(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 flags,
    /* [in] */ IInetSocketAddress* srcAddress,
    /* [out] */ Int32* num)
{
    return mOs->Recvfrom(fd, buffer, flags, srcAddress, num);
}

ECode ForwardingOs::Sendto(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 flags,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [out] */ Int32* result)
{
    return mOs->Sendto(fd, buffer, flags, inetAddress, port, result);
}

ECode ForwardingOs::Setenv(
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [in] */ Boolean overwrite)
{
    return mOs->Setenv(name, value, overwrite);
}

ECode ForwardingOs::SetsockoptGroupSourceReq(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 level,
    /* [in] */ Int32 option,
    /* [in] */ IStructGroupSourceReq* value)
{
    return mOs->SetsockoptGroupSourceReq(fd, level, option, value);
}

ECode ForwardingOs::Strsignal(
    /* [in] */ Int32 signal,
    /* [in] */ String* strSignal)
{
    return mOs->Strsignal(signal, strSignal);
}

ECode ForwardingOs::Tcsendbreak(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 duration)
{
    return mOs->Tcsendbreak(fd, duration);
}

ECode ForwardingOs::Write(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ Int32* num)
{
    return mOs->Write(fd, buffer, num);
}

ECode ForwardingOs::Writev(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<IInterface*>* buffers,
    /* [in] */ ArrayOf<Int32>* offsets,
    /* [in] */ ArrayOf<Int32>* byteCounts,
    /* [out] */ Int32* result)
{
    return mOs->Writev(fd, buffers, offsets, byteCounts, result);
}

} // namespace IO
} // namespace Libcore