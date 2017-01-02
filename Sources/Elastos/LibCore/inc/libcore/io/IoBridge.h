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

#ifndef __LIBCORE_IO_IOBRIDGE_H__
#define __LIBCORE_IO_IOBRIDGE_H__

#include "Elastos.CoreLibrary.Libcore.h"

using Elastos::IO::IByteBuffer;
using Elastos::IO::IFileDescriptor;
using Elastos::Net::IInetAddress;
using Elastos::Net::IDatagramPacket;
using Elastos::Net::IInetSocketAddress;

namespace Libcore {
namespace IO {

class IoBridge
{
public:
    static CARAPI Available(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ Int32* avail);

    static CARAPI Bind(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Int32 port);

    static CARAPI Connect(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Int32 port);

    static CARAPI Connect(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Int32 port,
        /* [in] */ Int32 timeoutMs);

    static CARAPI CloseAndSignalBlockedThreads(
        /* [in] */ IFileDescriptor* fd);

    static CARAPI IsConnected(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Int32 port,
        /* [in] */ Int32 timeoutMs,
        /* [in] */ Int32 remainingTimeoutMs,
        /* [out] */ Boolean* isConnected);

    static CARAPI GetSocketOption(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 option,
        /* [out] */ IInterface** value);

    static CARAPI SetSocketOption(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 option,
        /* [in] */ IInterface* value);

    static CARAPI Open(
        /* [in] */ const String& path,
        /* [in] */ Int32 flags,
        /* [out] */ IFileDescriptor** fd);

    static CARAPI Read(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* count);

    static CARAPI Write(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount);

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

    static CARAPI Recvfrom(
        /* [in] */ Boolean isRead,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [in] */ Int32 flags,
        /* [in] */ IDatagramPacket* packet,
        /* [in] */ Boolean isConnected,
        /* [out] */ Int32* result);

    static CARAPI Recvfrom(
        /* [in] */ Boolean isRead,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Int32 flags,
        /* [in] */ IDatagramPacket* packet,
        /* [in] */ Boolean isConnected,
        /* [out] */ Int32* result);

    static CARAPI Socket(
        /* [in] */ Boolean stream,
        /* [out] */ IFileDescriptor** fd);

    static CARAPI GetSocketLocalAddress(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ IInetAddress** address);

    static CARAPI GetSocketLocalPort(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ Int32* port);

private:
    static CARAPI ConnectErrno(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Int32 port,
        /* [in] */ Int32 timeoutMs);

    static CARAPI GetSocketOptionErrno(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 option,
        /* [out] */ IInterface** value);

    static CARAPI_(Int32) GetGroupSourceReqOp(
        /* [in] */ Int32 value);

    static CARAPI_(Boolean) BooleanFromInt(
        /* [in] */ Int32 i);

    static CARAPI_(Int32) BooleanToInt(
        /* [in] */ Boolean b);

    static CARAPI SetSocketOptionErrno(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 option,
        /* [in] */ IInterface* value);

    static CARAPI_(Int32) PostRecvfrom(
        /* [in] */ Boolean isRead,
        /* [in] */ IDatagramPacket* packet,
        /* [in] */ Boolean isConnected,
        /* [in] */ IInetSocketAddress* srcAddress,
        /* [in] */ Int32 byteCount);
};

} // namespace IO
} // namespace Libcore

#endif // __LIBCORE_IO_IOBRIDGE_H__
