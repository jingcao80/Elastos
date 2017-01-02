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

#include "CIoBridge.h"
#include "IoBridge.h"

namespace Libcore {
namespace IO {

CAR_SINGLETON_IMPL(CIoBridge)

CAR_INTERFACE_IMPL(CIoBridge, Singleton, IIoBridge)

ECode CIoBridge::Available(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Int32* avail)
{
    return IoBridge::Available(fd, avail);
}

ECode CIoBridge::Bind(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port)
{
    return IoBridge::Bind(fd, inetAddress, port);
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
    return IoBridge::Connect(fd, inetAddress, port, timeoutMs);
}

ECode CIoBridge::CloseAndSignalBlockedThreads(
    /* [in] */ IFileDescriptor* fd)
{
    return IoBridge::CloseAndSignalBlockedThreads(fd);
}

ECode CIoBridge::IsConnected(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [in] */ Int32 timeoutMs,
    /* [in] */ Int32 remainingTimeoutMs,
    /* [out] */ Boolean* isConnected)
{
    return IoBridge::IsConnected(fd, inetAddress, port, timeoutMs, remainingTimeoutMs, isConnected);
}

ECode CIoBridge::GetSocketOption(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 option,
    /* [out] */ IInterface** value)
{
    return IoBridge::GetSocketOption(fd, option, value);
}

ECode CIoBridge::SetSocketOption(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 option,
    /* [in] */ IInterface* value)
{
    return IoBridge::SetSocketOption(fd, option, value);
}

ECode CIoBridge::Open(
    /* [in] */ const String& path,
    /* [in] */ Int32 flags,
    /* [out] */ IFileDescriptor** fd)
{
    return IoBridge::Open(path, flags, fd);
}

ECode CIoBridge::Read(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* count)
{
    return IoBridge::Read(fd, bytes, byteOffset, byteCount, count);
}

ECode CIoBridge::Write(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount)
{
    return IoBridge::Write(fd, bytes, byteOffset, byteCount);
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
    return IoBridge::Sendto(fd, bytes, byteOffset, byteCount, flags, inetAddress, port, result);
}

ECode CIoBridge::Sendto(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 flags,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [out] */ Int32* result)
{
    return IoBridge::Sendto(fd, buffer, flags, inetAddress, port, result);
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
    return IoBridge::Recvfrom(isRead, fd, bytes, byteOffset, byteCount, flags, packet, isConnected, result);
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
    return IoBridge::Recvfrom(isRead, fd, buffer, flags, packet, isConnected, result);
}

ECode CIoBridge::Socket(
    /* [in] */ Boolean stream,
    /* [out] */ IFileDescriptor** fd)
{
    return IoBridge::Socket(stream, fd);
}

ECode CIoBridge::GetSocketLocalAddress(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IInetAddress** address)
{
    return IoBridge::GetSocketLocalAddress(fd, address);
}

ECode CIoBridge::GetSocketLocalPort(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Int32* port)
{
    return IoBridge::GetSocketLocalPort(fd, port);
}

} // namespace IO
} // namespace Libcore
