
#include "elastos/droid/bluetooth/BluetoothInputStream.h"
#include "elastos/droid/bluetooth/BluetoothSocket.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::IO::EIID_ICloseable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

BluetoothInputStream::BluetoothInputStream(
    /* [in] */ BluetoothSocket* s)
    : mSocket(s)
{}

ECode BluetoothInputStream::Available(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    return mSocket->Available(number);
}

ECode BluetoothInputStream::Close()
{
    return ICloseable::Probe(mSocket)->Close();
}

ECode BluetoothInputStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    AutoPtr< ArrayOf<Byte> > b = ArrayOf<Byte>::Alloc(1);
    Int32 ret;
    mSocket->Read(b, 0, 1, &ret);
    if (ret == 1) {
        *value = ((Int32)(*b)[0]) & 0xff;
    }
    else {
        *value = -1;
    }
    return NOERROR;
}

ECode BluetoothInputStream::ReadBytes(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;

    if (buffer == NULL) {
//        throw new NullPointerException("byte array is null");
        Slogger::E("BluetoothInputStream", "byte array is null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if ((offset | length) < 0 || length > buffer->GetLength() - offset) {
//        throw new ArrayIndexOutOfBoundsException("invalid offset or length");
        Slogger::E("BluetoothInputStream", "invalid offset or length");
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return mSocket->Read(buffer, offset, length, number);
}

//ECode BluetoothInputStream::Mark(
//    /* [in] */ Int32 readLimit)
//{
//    return InputStream::Mark(readLimit);
//}
//
//ECode BluetoothInputStream::IsMarkSupported(
//    /* [out] */ Boolean* supported)
//{
//    return InputStream::IsMarkSupported(supported);
//}
//
//ECode BluetoothInputStream::ReadBytes(
//    /* [out] */ ArrayOf<Byte>* buffer,
//    /* [out] */ Int32* number)
//{
//    return InputStream::ReadBytes(buffer, number);
//}
//
//ECode BluetoothInputStream::Reset()
//{
//    return InputStream::Reset();
//}
//
//ECode BluetoothInputStream::Skip(
//    /* [in] */ Int64 byteCount,
//    /* [out] */ Int64* number)
//{
//    return InputStream::Skip(byteCount, number);
//}
//
//ECode BluetoothInputStream::GetLock(
//    /* [out] */ IInterface** lockobj)
//{
//    VALIDATE_NOT_NULL(lockobj);
//    AutoPtr<IInterface> obj = InputStream::GetLock();
//    *lockobj = obj;
//    REFCOUNT_ADD(*lockobj);
//    return NOERROR;
//}

} // Bluetooth
} // Droid
} // Elastos
