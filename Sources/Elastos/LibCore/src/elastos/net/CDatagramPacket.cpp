
#include "CDatagramPacket.h"
#include "CInetSocketAddress.h"
#include "AutoLock.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CDatagramPacket, Object, IDatagramPacket)

CAR_OBJECT_IMPL(CDatagramPacket)

CDatagramPacket::CDatagramPacket()
    : mLength(0)
    , mUserSuppliedLength(0)
    , mPort(-1)
    , mOffset(0)
{}

CDatagramPacket::~CDatagramPacket()
{
}

ECode CDatagramPacket::constructor(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 length)
{
    return constructor(data, 0, length);;
}

ECode CDatagramPacket::constructor(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    return SetData(data, offset, length);
}

ECode CDatagramPacket::constructor(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ IInetAddress* host,
    /* [in] */ Int32 aPort)
{
    FAIL_RETURN(constructor(data, offset, length));
    SetPort(aPort);
    mAddress = host;
    return NOERROR;
}

ECode CDatagramPacket::constructor(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 length,
    /* [in] */ IInetAddress* host,
    /* [in] */ Int32 port)
{
    return constructor(data, 0, length, host, port);
}

ECode CDatagramPacket::constructor(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 length,
    /* [in] */ ISocketAddress* sockAddr)
{
    FAIL_RETURN(constructor(data, 0, length));
    return SetSocketAddress(sockAddr);
}

ECode CDatagramPacket::constructor(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ ISocketAddress* sockAddr)
{
    FAIL_RETURN(constructor(data, offset, length));
    return SetSocketAddress(sockAddr);
}

ECode CDatagramPacket::GetAddress(
    /* [out] */ IInetAddress** address)
{
    VALIDATE_NOT_NULL(address);

    synchronized(this) {
        *address = mAddress;
        REFCOUNT_ADD(*address);
    }
    return NOERROR;
}

ECode CDatagramPacket::GetData(
    /* [out, callee] */ ArrayOf<Byte>** data)
{
    VALIDATE_NOT_NULL(data);

    synchronized(this) {
        *data = mData;
        REFCOUNT_ADD(*data);
    }
    return NOERROR;
}

ECode CDatagramPacket::GetLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length);

    synchronized(this) {
        *length = mLength;
    }
    return NOERROR;
}

ECode CDatagramPacket::GetOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);

    synchronized(this) {
        *offset = mOffset;
    }
    return NOERROR;
}

ECode CDatagramPacket::ResetLengthForReceive()
{
    mLength = mUserSuppliedLength;
    return NOERROR;
}

ECode CDatagramPacket::SetReceivedLength(
    /* [in] */ Int32 length)
{
    mLength = length;
    return NOERROR;
}

ECode CDatagramPacket::GetPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);

    synchronized(this) {
        *port = mPort;
    }
    return NOERROR;
}

ECode CDatagramPacket::SetAddress(
    /* [in] */ IInetAddress* addr)
{
    synchronized(this) {
        mAddress = addr;
    }
    return NOERROR;
}

ECode CDatagramPacket::SetData(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 anOffset,
    /* [in] */ Int32 aLength)
{
    assert(buf);
    synchronized(this) {
        if (0 > anOffset || anOffset > buf->GetLength() || 0 > aLength
                || aLength > buf->GetLength() - anOffset) {
            //throw new IllegalArgumentException();
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        mData = buf;
        mOffset = anOffset;
        mLength = aLength;
        mUserSuppliedLength = aLength;
    }
    return NOERROR;
}

ECode CDatagramPacket::SetData(
    /* [in] */ ArrayOf<Byte>* buf)
{
    assert(buf);
    synchronized(this) {
        mLength = buf->GetLength(); // This will check for null
        mUserSuppliedLength = mLength;
        mData = buf;
        mOffset = 0;
    }
    return NOERROR;
}

ECode CDatagramPacket::SetLength(
    /* [in] */ Int32 len)
{
    synchronized(this) {
        if (0 > len || mOffset + len > mData->GetLength()) {
            //throw new IndexOutOfBoundsException();
            return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }
        mLength = len;
        mUserSuppliedLength = len;
    }
    return NOERROR;
}

ECode CDatagramPacket::SetPort(
    /* [in] */ Int32 aPort)
{
    synchronized(this) {
        if (aPort < 0 || aPort > 65535) {
            //throw new IllegalArgumentException("Port out of range: " + aPort);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        mPort = aPort;
    }
    return NOERROR;
}

ECode CDatagramPacket::GetSocketAddress(
    /* [out] */ ISocketAddress** sockAddr)
{
    VALIDATE_NOT_NULL(sockAddr);

    synchronized(this) {
        AutoPtr<IInetAddress> addr;
        GetAddress((IInetAddress**)&addr);
        Int32 port;
        GetPort(&port);
        AutoPtr<IInetSocketAddress> sa;
        FAIL_RETURN(CInetSocketAddress::New(addr, port, (IInetSocketAddress**)&sa));
        *sockAddr = ISocketAddress::Probe(sa);
        REFCOUNT_ADD(*sockAddr);
    }
    return NOERROR;
}

ECode CDatagramPacket::SetSocketAddress(
    /* [in] */ ISocketAddress* sockAddr)
{
    synchronized(this) {
        if (sockAddr == NULL || IInetSocketAddress::Probe(sockAddr) == NULL) {
            //throw new IllegalArgumentException("Socket address not an InetSocketAddress: " +
                    //(sockAddr == null ? null : sockAddr.getClass()));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        IInetSocketAddress* inetAddr = IInetSocketAddress::Probe(sockAddr);
        Boolean isUnresolved = FALSE;
        inetAddr->IsUnresolved(&isUnresolved);
        if (isUnresolved) {
            // throw new IllegalArgumentException("Socket address unresolved: " + sockAddr);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        inetAddr->GetPort(&mPort);
        mAddress = NULL;
        inetAddr->GetAddress((IInetAddress**)&mAddress);
    }
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
