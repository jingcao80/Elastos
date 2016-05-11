
#ifndef __LIBCORE_IO_CIOBRIDGE_H__
#define __LIBCORE_IO_CIOBRIDGE_H__

#include "Singleton.h"
#include "_Libcore_IO_CIoBridge.h"

using Elastos::Core::Singleton;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IFileDescriptor;
using Elastos::Net::IInetAddress;
using Elastos::Net::IDatagramPacket;
using Elastos::Net::IInetSocketAddress;

namespace Libcore {
namespace IO {

CarClass(CIoBridge)
    , public Singleton
    , public IIoBridge
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Available(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ Int32* avail);

    CARAPI Bind(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Int32 port);

    CARAPI Connect(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Int32 port);

    CARAPI Connect(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Int32 port,
        /* [in] */ Int32 timeoutMs);

    CARAPI CloseAndSignalBlockedThreads(
        /* [in] */ IFileDescriptor* fd);

    CARAPI IsConnected(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Int32 port,
        /* [in] */ Int32 timeoutMs,
        /* [in] */ Int32 remainingTimeoutMs,
        /* [out] */ Boolean* isConnected);

    CARAPI GetSocketOption(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 option,
        /* [out] */ IInterface** value);

    CARAPI SetSocketOption(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 option,
        /* [in] */ IInterface* value);

    CARAPI Open(
        /* [in] */ const String& path,
        /* [in] */ Int32 flags,
        /* [out] */ IFileDescriptor** fd);

    CARAPI Read(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* count);

    CARAPI Write(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount);

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

    CARAPI Recvfrom(
        /* [in] */ Boolean isRead,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [in] */ Int32 flags,
        /* [in] */ IDatagramPacket* packet,
        /* [in] */ Boolean isConnected,
        /* [out] */ Int32* result);

    CARAPI Recvfrom(
        /* [in] */ Boolean isRead,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Int32 flags,
        /* [in] */ IDatagramPacket* packet,
        /* [in] */ Boolean isConnected,
        /* [out] */ Int32* result);

    CARAPI Socket(
        /* [in] */ Boolean stream,
        /* [out] */ IFileDescriptor** fd);

    CARAPI GetSocketLocalAddress(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ IInetAddress** address);

    CARAPI GetSocketLocalPort(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ Int32* port);
};

} // namespace IO
} // namespace Libcore

#endif // __LIBCORE_IO_CIOBRIDGE_H__
