
#include "elastos/droid/app/backup/CBackupDataInputStream.h"
#include <elastos/coredef.h>
#include <elastos/core/Math.h>

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CAR_INTERFACE_IMPL(CBackupDataInputStream, Object, IBackupDataInputStream)

CAR_OBJECT_IMPL(CBackupDataInputStream)

ECode CBackupDataInputStream::constructor(
    /* [in] */ IBackupDataInput* data)
{
    mData = data;
    return NOERROR;
}

ECode CBackupDataInputStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<ArrayOf<Byte> > one = mOneByte;
    if (mOneByte == NULL) {
        mOneByte = ArrayOf<Byte>::Alloc(1);
        one = mOneByte;
    }

    Int32 readSize;
    FAIL_RETURN(mData->ReadEntityData(one, 0, 1, &readSize))
    *value = (*one)[0];
    return NOERROR;
}

ECode CBackupDataInputStream::ReadBytes(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);

    return mData->ReadEntityData(buffer, offset, length, number);
}

ECode CBackupDataInputStream::ReadBytes(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);

    return mData->ReadEntityData(buffer, 0, buffer->GetLength(), number);
}

ECode CBackupDataInputStream::Close()
{
    return NOERROR;
}

ECode CBackupDataInputStream::Available(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);

    *number = 0;
    return NOERROR;
}

ECode CBackupDataInputStream::Mark(
    /* [in] */ Int32 value)
{
    return NOERROR;
}

ECode CBackupDataInputStream::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);

    *supported = FALSE;
    return NOERROR;
}

ECode CBackupDataInputStream::Reset()
{
    return E_IO_EXCEPTION;
}

ECode CBackupDataInputStream::Skip(
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number);

    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(4096);

    Int64 skipped = 0;
    while (skipped < byteCount) {
        Int32 toRead = (Int32)Elastos::Core::Math::Min(byteCount - skipped, (Int64)buffer->GetLength());
        Int32 read = 0;
        FAIL_RETURN(ReadBytes(buffer.Get(), 0, toRead, &read));
        if (read == -1) {
            break;
        }
        skipped += read;
        if (read < toRead) {
            break;
        }
    }

    *number = skipped;

    return NOERROR;
}

ECode CBackupDataInputStream::GetKey(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = mKey;
    return NOERROR;
}

ECode CBackupDataInputStream::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);

    *size = mDataSize;
    return NOERROR;
}

ECode CBackupDataInputStream::SetKey(
    /* [in] */ const String& str)
{
    mKey = str;
    return NOERROR;
}

ECode CBackupDataInputStream::SetSize(
    /* [in] */ Int32 size)
{
    mDataSize = size;
    return NOERROR;
}

ECode CBackupDataInputStream::GetLock(
    /* [out] */ IInterface** lockobj)
{
    VALIDATE_NOT_NULL(lockobj);
    assert(0 && "TODO");
    // AutoPtr<IInterface> obj = InputStream::GetLock();
    // *lockobj = obj;
    // REFCOUNT_ADD(*lockobj);
    return NOERROR;
}

ECode CBackupDataInputStream::GetData(
    /* [out] */ IBackupDataInput** data)
{
    VALIDATE_NOT_NULL(data);
    *data = mData;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

} //namespace Elastos
} //namespace Droid
} //namespace App
} //namespace Elastos
