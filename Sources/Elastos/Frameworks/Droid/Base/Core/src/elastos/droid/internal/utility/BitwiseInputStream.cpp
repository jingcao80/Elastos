
#include "elastos/droid/internal/utility/BitwiseInputStream.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(BitwiseInputStream, Object, IBitwiseInputStream)

ECode BitwiseInputStream::constructor(
    /* [in] */ ArrayOf<Byte>* buf)
{
    mBuf = buf;
    mEnd = buf->GetLength() << 3;
    mPos = 0;
    return NOERROR;
}

ECode BitwiseInputStream::Available(
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    *num = mEnd - mPos;
    return NOERROR;
}

ECode BitwiseInputStream::Read(
    /* [in] */ Int32 bits,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    Int32 index = (UInt32)mPos >> 3;// unsigned shift
    Int32 offset = 16 - (mPos & 0x07) - bits;  // &7==%8
    if ((bits < 0) || (bits > 8) || ((mPos + bits) > mEnd)) {
        Logger::E("BitwiseInputStream", "illegal read (pos %d, end %d, bits %d)",
                mPos, mEnd, bits);
        return E_ACCESS_EXCEPTION;
    }
    Int32 data = ((*mBuf)[index] & 0xFF) << 8;
    if (offset < 8)
        data |= (*mBuf)[index + 1] & 0xFF;
    data = (UInt32)data >> offset;// unsigned shift
    data &= (UInt32)-1 >> (32 - bits);// unsigned shift;
    mPos += bits;
    *num = data;
    return NOERROR;
}

ECode BitwiseInputStream::ReadByteArray(
    /* [in] */ Int32 bits,
    /* [out] */ ArrayOf<Byte>** buf)
{
    Int32 bytes = ((UInt32)bits >> 3) + ((bits & 0x07) > 0 ? 1 : 0);  // &7==%8
    AutoPtr<ArrayOf<Byte> > arr = ArrayOf<Byte>::Alloc(bytes);
    for (Int32 i = 0; i < bytes; i++) {
        Int32 increment = Elastos::Core::Math::Min(8, bits - (i << 3));
        Int32 num;
        FAIL_RETURN(Read(increment, &num));
        (*arr)[i] = (Byte)(num << (8 - increment));
    }
    *buf = arr;
    REFCOUNT_ADD(*buf)
    return NOERROR;
}

ECode BitwiseInputStream::Skip(
    /* [in] */ Int32 bits)
{
    if ((mPos + bits) > mEnd) {
        Logger::E("BitwiseInputStream", "illegal skip (pos %d, end %d, bits %d)",
                mPos, mEnd, bits);
        return E_ACCESS_EXCEPTION;
    }
    mPos += bits;
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
