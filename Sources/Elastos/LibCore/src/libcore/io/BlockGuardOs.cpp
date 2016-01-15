
#include "Elastos.CoreLibrary.IO.h"
#include "BlockGuardOs.h"
#include "CLibcore.h"
#include "BlockGuard.h"
#include "droid/system/OsConstants.h"
#include "SocketTagger.h"

using Elastos::Core::BlockGuard;
using Elastos::Core::IBlockGuardPolicy;
using Elastos::Droid::System::OsConstants;
using Elastos::Droid::System::IStructLinger;
using Elastos::Droid::System::IStructStat;
using Elastos::Droid::System::IStructStatVfs;

#ifndef BLOCKGUARD_THREADPOLICY_CALL
#define BLOCKGUARD_THREADPOLICY_CALL(callname)                                   \
AutoPtr<IBlockGuardPolicy> policy;                                               \
FAIL_RETURN(BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy))           \
FAIL_RETURN(policy->callname())

#endif

namespace Libcore {
namespace IO {

BlockGuardOs::BlockGuardOs(
    /* [in] */ IOs* os)
    : ForwardingOs(os)
{}

ECode BlockGuardOs::TagSocket(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IFileDescriptor** retFd)
{
    VALIDATE_NOT_NULL(retFd);

    //try {
    AutoPtr<ISocketTagger> tagger;
    FAIL_RETURN(SocketTagger::Get((ISocketTagger**)&tagger))
    FAIL_RETURN(tagger->Tag(fd))
    *retFd = fd;
    REFCOUNT_ADD(*retFd)
    //} catch (SocketException e) {
        //throw new ErrnoException("socket", EINVAL, e);
    //}

    return NOERROR;
}

ECode BlockGuardOs::UntagSocket(
    /* [in] */ IFileDescriptor* fd)
{
    //try {
    AutoPtr<ISocketTagger> tagger;
    FAIL_RETURN(SocketTagger::Get((ISocketTagger**)&tagger))
    FAIL_RETURN(tagger->Untag(fd))
    //} catch (SocketException e) {
        //throw new ErrnoException("socket", EINVAL, e);
    //}

    return NOERROR;
}

ECode BlockGuardOs::Accept(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetSocketAddress* peerAddress,
    /* [out] */ IFileDescriptor** retFd)
{
    VALIDATE_NOT_NULL(retFd);
    BLOCKGUARD_THREADPOLICY_CALL(OnNetwork)
    AutoPtr<IFileDescriptor> tmp;
    FAIL_RETURN(mOs->Accept(fd, peerAddress, (IFileDescriptor**)&tmp))
    return TagSocket(tmp, retFd);
}

ECode BlockGuardOs::Access(
    /* [in] */ String path,
    /* [in] */ Int32 mode,
    /* [out] */ Boolean* succeed)
{
    VALIDATE_NOT_NULL(succeed);
    BLOCKGUARD_THREADPOLICY_CALL(OnReadFromDisk)
    return mOs->Access(path, mode, succeed);
}

ECode BlockGuardOs::Chmod(
    /* [in] */ String path,
    /* [in] */ Int32 mode)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Chmod(path, mode);
}

ECode BlockGuardOs::Chown(
    /* [in] */ String path,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Chown(path, uid, gid);
}

ECode BlockGuardOs::Close(
    /* [in] */ IFileDescriptor* fd)
{
    // try {
        // The usual case is that this _isn't_ a socket, so the getsockopt(2) call in
        // isLingerSocket will throw, and that's really expensive. Try to avoid asking
        // if we don't care.
        Boolean isSocket;
        if (fd->IsSocket(&isSocket), isSocket) {
            Boolean isLingerSocket;
            if (IsLingerSocket(fd, &isLingerSocket), isLingerSocket) {
                // If the fd is a socket with SO_LINGER set, we might block indefinitely.
                // We allow non-linger sockets so that apps can close their network
                // connections in methods like onDestroy which will run on the UI thread.
                BLOCKGUARD_THREADPOLICY_CALL(OnNetwork)
            }
            UntagSocket(fd);
        }
    // } catch (ErrnoException ignored) {
        // We're called via Socket.close (which doesn't ask for us to be called), so we
        // must not throw here, because Socket.close must not throw if asked to close an
        // already-closed socket. Also, the passed-in FileDescriptor isn't necessarily
        // a socket at all.
    // }
    return mOs->Close(fd);
}

ECode BlockGuardOs::IsLingerSocket(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IStructLinger> struLinger;
    FAIL_RETURN(CLibcore::sOs->GetsockoptLinger(fd, OsConstants::_SOL_SOCKET, OsConstants::_SO_LINGER, (IStructLinger**)&struLinger))
    Boolean isOn;
    FAIL_RETURN(struLinger->IsOn(&isOn))
    Int32 linger;
    FAIL_RETURN(struLinger->GetLinger(&linger))
    *result = isOn && linger > 0;

    return NOERROR;
}

ECode BlockGuardOs::Connect(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnNetwork)
    return mOs->Connect(fd, address, port);
}

ECode BlockGuardOs::Fchmod(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 mode)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Fchmod(fd, mode);
}

ECode BlockGuardOs::Fchown(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Fchown(fd, uid, gid);
}

// TODO: Untag newFd when needed for dup2(FileDescriptor oldFd, int newFd)

ECode BlockGuardOs::Fdatasync(
    /* [in] */ IFileDescriptor* fd)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Fdatasync(fd);
}

ECode BlockGuardOs::Fstat(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IStructStat** stat)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnReadFromDisk)
    return mOs->Fstat(fd, stat);
}

ECode BlockGuardOs::Fstatvfs(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IStructStatVfs** statVfs)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnReadFromDisk)
    return mOs->Fstatvfs(fd, statVfs);
}

ECode BlockGuardOs::Fsync(
    /* [in] */ IFileDescriptor* fd)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Fsync(fd);
}

ECode BlockGuardOs::Ftruncate(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 length)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Ftruncate(fd, length);
}

ECode BlockGuardOs::Lchown(
    /* [in] */ String path,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Lchown(path, uid, gid);
}

ECode BlockGuardOs::Link(
    /* [in] */ String oldPath,
    /* [in] */ String newPath)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Link(oldPath, newPath);
}

ECode BlockGuardOs::Lseek(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int32 whence,
    /* [out] */ Int64* result)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnReadFromDisk)
    return mOs->Lseek(fd, offset, whence, result);
}

ECode BlockGuardOs::Lstat(
    /* [in] */ String path,
    /* [out] */ IStructStat** stat)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnReadFromDisk)
    return mOs->Lstat(path, stat);
}

ECode BlockGuardOs::Mkdir(
    /* [in] */ String path,
    /* [in] */ Int32 mode)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Mkdir(path, mode);
}

ECode BlockGuardOs::Mkfifo(
    /* [in] */ String path,
    /* [in] */ Int32 mode)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Mkdir(path, mode);
}

ECode BlockGuardOs::Open(
    /* [in] */ const String& path,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 mode,
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd);
    BLOCKGUARD_THREADPOLICY_CALL(OnReadFromDisk)
    if ((mode & OsConstants::_O_ACCMODE) != OsConstants::_O_RDONLY) {
        FAIL_RETURN(policy->OnWriteToDisk())
    }
    return mOs->Open(path, flags, mode, fd);
}

ECode BlockGuardOs::Poll(
    /* [in] */ ArrayOf<IStructPollfd*>* fdStructs,
    /* [in] */ Int32 timeoutMs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    // Greater than 0 is a timeout in milliseconds and -1 means "block forever",
    // but 0 means "poll and return immediately", which shouldn't be subject to BlockGuard.
    if (timeoutMs != 0) {
        BLOCKGUARD_THREADPOLICY_CALL(OnNetwork)
    }
    return mOs->Poll(fdStructs, timeoutMs, result);
}


ECode BlockGuardOs::Posix_fallocate(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Posix_fallocate(fd, offset, length);
}

ECode BlockGuardOs::Pread(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int64 offset,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(bytes);
    VALIDATE_NOT_NULL(num);
    BLOCKGUARD_THREADPOLICY_CALL(OnReadFromDisk)
    return mOs->Pread(fd, bytes, byteOffset, byteCount, offset, num);
}


ECode BlockGuardOs::Pread(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int64 offset,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);
    BLOCKGUARD_THREADPOLICY_CALL(OnReadFromDisk)
    return mOs->Pread(fd, buffer, offset, num);
}

ECode BlockGuardOs::Pwrite(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int64 offset,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Pwrite(fd, bytes, byteOffset, byteCount, offset, num);
}

ECode BlockGuardOs::Pwrite(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int64 offset,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Pwrite(fd, buffer, offset, num);
}

ECode BlockGuardOs::Read(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(bytes);
    VALIDATE_NOT_NULL(num);
    BLOCKGUARD_THREADPOLICY_CALL(OnReadFromDisk)
    return mOs->Read(fd, bytes, byteOffset, byteCount, num);
}


ECode BlockGuardOs::Read(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);
    BLOCKGUARD_THREADPOLICY_CALL(OnReadFromDisk)
    return mOs->Read(fd, buffer, num);
}

ECode BlockGuardOs::Readlink(
    /* [in] */ String path,
    /* [out] */ String* link)
{
    VALIDATE_NOT_NULL(link);
    BLOCKGUARD_THREADPOLICY_CALL(OnReadFromDisk)
    return mOs->Readlink(path, link);
}

ECode BlockGuardOs::Readv(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<IInterface*>* buffers,
    /* [in] */ ArrayOf<Int32>* offsets,
    /* [in] */ ArrayOf<Int32>* byteCounts,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);
    BLOCKGUARD_THREADPOLICY_CALL(OnReadFromDisk)
    return mOs->Readv(fd, buffers, offsets, byteCounts, num);
}

ECode BlockGuardOs::Recvfrom(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 flags,
    /* [in] */ IInetSocketAddress* srcAddress,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);
    BLOCKGUARD_THREADPOLICY_CALL(OnNetwork)
    return mOs->Recvfrom(fd, buffer, flags, srcAddress, num);
}

ECode BlockGuardOs::Recvfrom(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int32 flags,
    /* [in] */ IInetSocketAddress* srcAddress,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);
    BLOCKGUARD_THREADPOLICY_CALL(OnNetwork)
    return mOs->Recvfrom(fd, bytes, byteOffset, byteCount, flags, srcAddress, num);
}

ECode BlockGuardOs::Remove(
    /* [in] */ String path)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Remove(path);
}

ECode BlockGuardOs::Rename(
    /* [in] */ String oldPath,
    /* [in] */ String newPath)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Rename(oldPath, newPath);
}

ECode BlockGuardOs::Sendfile(
    /* [in] */ IFileDescriptor* outFd,
    /* [in] */ IFileDescriptor* inFd,
    /* [out] */ Int64* inOffset,
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* result)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Sendfile(outFd, inFd, inOffset, byteCount, result);
}

ECode BlockGuardOs::Sendto(
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
    // We permit datagrams without hostname lookups.
    if (inetAddress != NULL) {
        BLOCKGUARD_THREADPOLICY_CALL(OnNetwork)
    }
    return mOs->Sendto(fd, bytes, byteOffset, byteCount, flags, inetAddress, port, result);
}

ECode BlockGuardOs::Sendto(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 flags,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [out] */ Int32* result)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnNetwork)
    return mOs->Sendto(fd, buffer, flags, inetAddress, port, result);
}

ECode BlockGuardOs::Socket(
    /* [in] */ Int32 socketDomain,
    /* [in] */ Int32 type,
    /* [in] */ Int32 protocol,
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd);
    AutoPtr<IFileDescriptor> tmp;
    FAIL_RETURN(mOs->Socket(socketDomain, type, protocol, (IFileDescriptor**)&tmp))
    return TagSocket(tmp, fd);
}

ECode BlockGuardOs::Socketpair(
    /* [in] */ Int32 socketDomain,
    /* [in] */ Int32 type,
    /* [in] */ Int32 protocol,
    /* [in] */ IFileDescriptor* fd1,
    /* [in] */ IFileDescriptor* fd2)
{
    FAIL_RETURN(mOs->Socketpair(socketDomain, type, protocol, fd1, fd2))
    AutoPtr<IFileDescriptor> tmp1, tmp2;
    FAIL_RETURN(TagSocket(fd1, (IFileDescriptor**)&tmp1))
    FAIL_RETURN(TagSocket(fd2, (IFileDescriptor**)&tmp2))
    return NOERROR;
}

ECode BlockGuardOs::Stat(
    /* [in] */ String path,
    /* [out] */ IStructStat** stat)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnReadFromDisk)
    return mOs->Stat(path, stat);
}

/* TODO: replace statfs with statvfs. */
ECode BlockGuardOs::StatVfs(
    /* [in] */ String path,
    /* [out] */ IStructStatVfs** statfs)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnReadFromDisk)
    return mOs->StatVfs(path, statfs);
}

ECode BlockGuardOs::Symlink(
    /* [in] */ String oldPath,
    /* [in] */ String newPath)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Symlink(oldPath, newPath);
}

ECode BlockGuardOs::Write(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Write(fd, buffer, num);
}

ECode BlockGuardOs::Write(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Write(fd, bytes, byteOffset, byteCount, num);
}

ECode BlockGuardOs::Writev(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<IInterface*>* buffers,
    /* [in] */ ArrayOf<Int32>* offsets,
    /* [in] */ ArrayOf<Int32>* byteCounts,
    /* [out] */ Int32* result)
{
    BLOCKGUARD_THREADPOLICY_CALL(OnWriteToDisk)
    return mOs->Writev(fd, buffers, offsets, byteCounts, result);
}

} // namespace IO
} // namespace Libcore