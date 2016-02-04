#ifndef __ELASTOS_IO_INT64ARRAYBUFFER_H__
#define __ELASTOS_IO_INT64ARRAYBUFFER_H__

#include "Int64Buffer.h"

namespace Elastos {
namespace IO {

/**
 * LongArrayBuffer, ReadWriteLongArrayBuffer and ReadOnlyLongArrayBuffer compose
 * the implementation of array based long buffers.
 * <p>
 * LongArrayBuffer implements all the shared readonly methods and is extended by
 * the other two classes.
 * </p>
 * <p>
 * All methods are marked final for runtime performance.
 * </p>
 *
 */
class Int64ArrayBuffer
    : public Int64Buffer
{
public:
    Int64ArrayBuffer();

    CARAPI constructor(
        /* [in] */ ArrayOf<Int64>* array);

    CARAPI constructor(
        /* [in] */ Int32 capacity,
        /* [in] */ ArrayOf<Int64>* backingArray,
        /* [in] */ Int32 offset,
        /* [in] */ Boolean isReadOnly);

    CARAPI GetPrimitiveArray(
        /* [out] */ Handle64* arrayHandle);

    CARAPI Get(
        /* [out] */ Int64* value);

    CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Int64* value);

    CARAPI Get(
        /* [out] */ ArrayOf<Int64>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 int64Count);

    CARAPI IsDirect(
        /* [out] */ Boolean* isDirect);

    CARAPI GetOrder(
       /* [out] */ ByteOrder* byteOrder);

    CARAPI AsReadOnlyBuffer(
        /* [out] */ IInt64Buffer** buffer);

    CARAPI Compact();

    CARAPI Duplicate(
        /* [out] */ IInt64Buffer** buffer);

    CARAPI ProtectedArray(
        /* [out, callee] */ ArrayOf<Int64>** array);

    CARAPI ProtectedArrayOffset(
        /* [out] */ Int32* offset);

    CARAPI ProtectedHasArray(
        /* [out] */ Boolean* hasArray);

    CARAPI Put(
        /* [in] */ Int64 d);

    CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Int64 d);

    CARAPI Put(
        /* [in] */ const ArrayOf<Int64>& src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 doubleCount);

    CARAPI Slice(
        /* [out] */ IInt64Buffer** buffer);

    CARAPI IsReadOnly(
        /* [out] */ Boolean* value);

private:
    static CARAPI Copy(
        /* [in] */ Int64ArrayBuffer* other,
        /* [in] */ Int32 markOfOther,
        /* [in] */ Boolean isReadOnly,
        /* [out] */ Int64ArrayBuffer** buffer);

public:
    AutoPtr< ArrayOf<Int64> > mBackingArray;

    Int32 mArrayOffset;

    Boolean mIsReadOnly;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_INT64ARRAYBUFFER_H__
