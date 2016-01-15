
#include "BluetoothOutputStream.h"
#include "BluetoothSocket.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::IO::EIID_ICloseable;
using Elastos::IO::EIID_IOutputStream;
using Elastos::IO::EIID_IFlushable;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

BluetoothOutputStream::BluetoothOutputStream(
    /* [in] */ BluetoothSocket* s)
    : mSocket(s)
{}

UInt32 BluetoothOutputStream::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 BluetoothOutputStream::Release()
{
    return ElRefBase::Release();
}

PInterface BluetoothOutputStream::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IOutputStream*)this;
    }
    else if (riid == EIID_IOutputStream) {
        return (IOutputStream*)this;
    }
    if (riid == EIID_ICloseable) {
        return (ICloseable*)(IOutputStream*)this;
    }
    else if (riid == EIID_IFlushable) {
       return (IFlushable*)this;
    }

    return NULL;
}

ECode BluetoothOutputStream::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (NULL == pIID) return E_INVALID_ARGUMENT;

    if (pObject == (IInterface *)(IOutputStream *)this) {
        *pIID = EIID_IOutputStream;
        return NOERROR;
    }
    else if (pObject == (IInterface *)(ICloseable *)this) {
        *pIID = EIID_ICloseable;
        return NOERROR;
    }
    else if (pObject == (IInterface *)(IFlushable *)this) {
        *pIID = EIID_IFlushable;
        return NOERROR;
    }
    return E_INVALID_ARGUMENT;
}

ECode BluetoothOutputStream::Close()
{
    return ((ICloseable*)mSocket->Probe(EIID_ICloseable))->Close();
}

ECode BluetoothOutputStream::Write(
    /* [in] */ Int32 oneByte)
{
    AutoPtr< ArrayOf<Byte> > b = ArrayOf<Byte>::Alloc(1);
    (*b)[0] = (Byte)oneByte;
    Int32 count;
    return mSocket->Write(b, 0, 1, &count);
}

ECode BluetoothOutputStream::WriteBytes(
    /* [in] */ const ArrayOf<Byte> & buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    // if (b == NULL) {
    //     throw new NullPointerException("buffer is null");
    // }
    if ((offset | count) < 0 || count > buffer.GetLength() - offset) {
//        throw new IndexOutOfBoundsException("invalid offset or length");
        Slogger::E("BluetoothOutputStream", "invalid offset or length");
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    Int32 num;
    return mSocket->Write(const_cast<ArrayOf<Byte>*>(&buffer), offset, count, &num);
}

ECode BluetoothOutputStream::Flush()
{
    return OutputStream::Flush();
}

ECode BluetoothOutputStream::WriteBytes(
    /* [in] */ const ArrayOf<Byte> & buffer)
{
    return OutputStream::WriteBytes(buffer);
}

ECode BluetoothOutputStream::CheckError(
    /* [out] */ Boolean* hasError)
{
    return OutputStream::CheckError(hasError);
}

ECode BluetoothOutputStream::GetLock(
    /* [out] */ IInterface** lockobj)
{
    VALIDATE_NOT_NULL(lockobj);
    AutoPtr<IInterface> obj = OutputStream::GetLock();
    *lockobj = obj;
    REFCOUNT_ADD(*lockobj);
    return NOERROR;
}

} // Bluetooth
} // Droid
} // Elastos

