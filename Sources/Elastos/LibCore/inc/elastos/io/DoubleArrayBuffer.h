#ifndef __ELASTOS_IO_DOUBLEARRAYBUFFER_H__
#define __ELASTOS_IO_DOUBLEARRAYBUFFER_H__

#include "DoubleBuffer.h"

namespace Elastos {
namespace IO {

/**
 * DoubleArrayBuffer, ReadWriteDoubleArrayBuffer and ReadOnlyDoubleArrayBuffer
 * compose the implementation of array based double buffers.
 * <p>
 * DoubleArrayBuffer implements all the shared readonly methods and is extended
 * by the other two classes.
 * </p>
 * <p>
 * All methods are marked final for runtime performance.
 * </p>
 *
 */
class DoubleArrayBuffer
    : public DoubleBuffer
{
public:
    DoubleArrayBuffer();

    CARAPI constructor(
        /* [in] */ ArrayOf<Double>* array);

    CARAPI constructor(
        /* [in] */ Int32 capacity,
        /* [in] */ ArrayOf<Double>* backingArray,
        /* [in] */ Int32 offset,
        /* [in] */ Boolean isReadOnly);

    CARAPI GetPrimitiveArray(
        /* [out] */ Handle64* arrayHandle);

    CARAPI Get(
        /* [out] */ Double* value);

    CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Double* value);

    CARAPI Get(
        /* [out] */ ArrayOf<Double>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 doubleCount);

    CARAPI IsDirect(
        /* [out] */ Boolean* isDirect);

    CARAPI GetOrder(
        /* [out] */ ByteOrder* byteOrder);

    CARAPI AsReadOnlyBuffer(
        /* [out] */ IDoubleBuffer** buffer);

    CARAPI Compact();

    CARAPI Duplicate(
        /* [out] */ IDoubleBuffer** buffer);

    CARAPI ProtectedArray(
        /* [out, callee] */ ArrayOf<Double>** array);

    CARAPI ProtectedArrayOffset(
        /* [out] */ Int32* offset);

    CARAPI ProtectedHasArray(
        /* [out] */ Boolean* hasArray);

    CARAPI Put(
        /* [in] */ Double d);

    CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Double d);

    CARAPI Put(
        /* [in] */ const ArrayOf<Double>& src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 doubleCount);

    CARAPI Slice(
        /* [out] */ IDoubleBuffer** buffer);

    CARAPI IsReadOnly(
        /* [out] */ Boolean* value);

private:
    static CARAPI Copy(
        /* [in] */ DoubleArrayBuffer* other,
        /* [in] */ Int32 markOfOther,
        /* [in] */ Boolean isReadOnly,
        /* [out] */ DoubleArrayBuffer** buffer);

public:
    AutoPtr< ArrayOf<Double> > mBackingArray;

    Int32 mArrayOffset;

    Boolean mIsReadOnly;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_DOUBLEARRAYBUFFER_H__
