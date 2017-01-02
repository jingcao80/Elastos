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

#ifndef __LIBCORE_IO_BLOCKGUARDOS_H__
#define __LIBCORE_IO_BLOCKGUARDOS_H__

#include "ForwardingOs.h"

using Libcore::IO::IOs;
using Elastos::IO::IFileDescriptor;
using Elastos::Net::IInetSocketAddress;
using Elastos::Net::IInetAddress;

namespace Libcore {
namespace IO {

class BlockGuardOs : public ForwardingOs
{
public:
    BlockGuardOs(
        /* [in] */ IOs* os);

    CARAPI Accept(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IInetSocketAddress* peerAddress,
        /* [out] */ IFileDescriptor** retFd);

    CARAPI Access(
        /* [in] */ const String& path,
        /* [in] */ Int32 mode,
        /* [out] */ Boolean* succeed);

    CARAPI Chmod(
        /* [in] */ const String& path,
        /* [in] */ Int32 mode);

    CARAPI Chown(
        /* [in] */ const String& path,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 gid);


    CARAPI Close(
        /* [in] */ IFileDescriptor* fd);

    CARAPI Connect(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 port);

    CARAPI Fchmod(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 mode);

    CARAPI Fchown(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 gid);

    // TODO: Untag newFd when needed for dup2(FileDescriptor oldFd, int newFd)

    CARAPI Fdatasync(
        /* [in] */ IFileDescriptor* fd);

    CARAPI Fstat(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ IStructStat** stat);

    CARAPI Fstatvfs(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ IStructStatVfs** statVfs);

    CARAPI Fsync(
        /* [in] */ IFileDescriptor* fd);

    CARAPI Ftruncate(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int64 length);

    CARAPI Lchown(
        /* [in] */ const String& path,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 gid);

    CARAPI Link(
        /* [in] */ const String& oldPath,
        /* [in] */ const String& newPath);

    CARAPI Lseek(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int64 offset,
        /* [in] */ Int32 whence,
        /* [out] */ Int64* result);

    CARAPI Lstat(
        /* [in] */ const String& path,
        /* [out] */ IStructStat** stat);

    CARAPI Mkdir(
        /* [in] */ const String& path,
        /* [in] */ Int32 mode);
    CARAPI Mkfifo(
        /* [in] */ const String& path,
        /* [in] */ Int32 mode);

    CARAPI Open(
        /* [in] */ const String& path,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 mode,
        /* [out] */ IFileDescriptor** fd);

    CARAPI Poll(
        /* [in] */ ArrayOf<IStructPollfd*>* fdStructs,
        /* [in] */ Int32 timeoutMs,
        /* [out] */ Int32* result);

    CARAPI Posix_fallocate(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int64 offset,
        /* [in] */ Int64 length);

    CARAPI Pread(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [in] */ Int64 offset,
        /* [out] */ Int32* num);

    CARAPI Pread(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Int64 offset,
        /* [out] */ Int32* num);

    CARAPI Pwrite(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [in] */ Int64 offset,
        /* [out] */ Int32* num);

    CARAPI Read(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* num);

    CARAPI Pwrite(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Int64 offset,
        /* [out] */ Int32* num);

    CARAPI Read(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [out] */ Int32* num);

    CARAPI Readlink(
        /* [in] */ const String& path,
        /* [out] */ String* link);

    CARAPI Readv(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<IInterface*>* buffers,
        /* [in] */ ArrayOf<Int32>* offsets,
        /* [in] */ ArrayOf<Int32>* byteCounts,
        /* [out] */ Int32* num);

    CARAPI Recvfrom(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [in] */ Int32 flags,
        /* [in] */ IInetSocketAddress* srcAddress,
        /* [out] */ Int32* num);

    CARAPI Recvfrom(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Int32 flags,
        /* [in] */ IInetSocketAddress* srcAddress,
        /* [out] */ Int32* num);

    CARAPI Remove(
        /* [in] */ const String& path);

    CARAPI Rename(
        /* [in] */ const String& oldPath,
        /* [in] */ const String& newPath);

    CARAPI Sendfile(
        /* [in] */ IFileDescriptor* outFd,
        /* [in] */ IFileDescriptor* inFd,
        /* [out] */ Int64* inOffset,
        /* [in] */ Int64 byteCount,
        /* [out] */ Int64* result);

    CARAPI Sendto(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [in] */ Int32 flags,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Int32 port,
        /* [out] */ Int32* result);

    CARAPI Sendto(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Int32 flags,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Int32 port,
        /* [out] */ Int32* result);

    CARAPI Socket(
        /* [in] */ Int32 socketDomain,
        /* [in] */ Int32 type,
        /* [in] */ Int32 protocol,
        /* [out] */ IFileDescriptor** fd);

    CARAPI Socketpair(
        /* [in] */ Int32 socketDomain,
        /* [in] */ Int32 type,
        /* [in] */ Int32 protocol,
        /* [in] */ IFileDescriptor* fd1,
        /* [in] */ IFileDescriptor* fd2);

    CARAPI Stat(
        /* [in] */ const String& path,
        /* [out] */ IStructStat** stat);

    /* TODO: replace statfs with statvfs. */
    CARAPI StatVfs(
        /* [in] */ const String& path,
        /* [out] */ IStructStatVfs** statfs);

    CARAPI Symlink(
        /* [in] */ const String& oldPath,
        /* [in] */ const String& newPath);

    CARAPI Write(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [out] */ Int32* num);

    CARAPI Write(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* num);

    CARAPI Writev(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<IInterface*>* buffers,
        /* [in] */ ArrayOf<Int32>* offsets,
        /* [in] */ ArrayOf<Int32>* byteCounts,
        /* [out] */ Int32* result);

private:
    CARAPI TagSocket(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ IFileDescriptor** retFd);

    CARAPI UntagSocket(
        /* [in] */ IFileDescriptor* fd);

    static CARAPI IsLingerSocket(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ Boolean* result);
};

} // namespace IO
} // namespace Libcore

#endif // __LIBCORE_IO_BLOCKGUARDOS_H__
