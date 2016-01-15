
#ifndef __LIBCORE_IO_CIOBRIDGE_H__
#define __LIBCORE_IO_CIOBRIDGE_H__

#include "Singleton.h"
#include "_Libcore_IO_CIoBridge.h"

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

public:
    static CARAPI _Available(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ Int32* avail);

    static CARAPI _Bind(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Int32 port);

    static CARAPI _Connect(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Int32 port);

    static CARAPI _Connect(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Int32 port,
        /* [in] */ Int32 timeoutMs);

    static CARAPI _CloseAndSignalBlockedThreads(
        /* [in] */ IFileDescriptor* fd);

    static CARAPI _IsConnected(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Int32 port,
        /* [in] */ Int32 timeoutMs,
        /* [in] */ Int32 remainingTimeoutMs,
        /* [out] */ Boolean* isConnected);

    static CARAPI _GetSocketOption(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 option,
        /* [out] */ IInterface** value);

    static CARAPI _SetSocketOption(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 option,
        /* [in] */ IInterface* value);

    static CARAPI _Open(
        /* [in] */ const String& path,
        /* [in] */ Int32 flags,
        /* [out] */ IFileDescriptor** fd);

    static CARAPI _Read(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* count);

    static CARAPI _Write(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount);

    static CARAPI _Sendto(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [in] */ Int32 flags,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Int32 port,
        /* [out] */ Int32* result);

    static CARAPI _Sendto(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Int32 flags,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Int32 port,
        /* [out] */ Int32* result);

    static CARAPI _Recvfrom(
        /* [in] */ Boolean isRead,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [in] */ Int32 flags,
        /* [in] */ IDatagramPacket* packet,
        /* [in] */ Boolean isConnected,
        /* [out] */ Int32* result);

    static CARAPI _Recvfrom(
        /* [in] */ Boolean isRead,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Int32 flags,
        /* [in] */ IDatagramPacket* packet,
        /* [in] */ Boolean isConnected,
        /* [out] */ Int32* result);

    static CARAPI _Socket(
        /* [in] */ Boolean stream,
        /* [out] */ IFileDescriptor** fd);

    static CARAPI _GetSocketLocalAddress(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ IInetAddress** address);

    static CARAPI _GetSocketLocalPort(
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

#endif // __LIBCORE_IO_CIOBRIDGE_H__
