#ifndef __ELASTOS_IO_INT16ARRAYBUFFER_H__
#define __ELASTOS_IO_INT16ARRAYBUFFER_H__

#include "Int16Buffer.h"

namespace Elastos {
namespace IO {

/**
 * ShortArrayBuffer, ReadWriteShortArrayBuffer and ReadOnlyShortArrayBuffer
 * compose the implementation of array based short buffers.
 * <p>
 * ShortArrayBuffer implements all the shared readonly methods and is extended
 * by the other two classes.
 * </p>
 * <p>
 * All methods are marked final for runtime performance.
 * </p>
 *
 */
class Int16ArrayBuffer
    : public Int16Buffer
{
public:
    Int16ArrayBuffer();

    CARAPI constructor(
        /* [in] */ ArrayOf<Int16>* array);

    CARAPI constructor(
        /* [in] */ Int32 capacity,
        /* [in] */ ArrayOf<Int16>* backingArray,
        /* [in] */ Int32 offset,
        /* [in] */ Boolean isReadOnly);

    CARAPI GetPrimitiveArray(
        /* [out] */ Handle64* arrayHandle);

    CARAPI Get(
        /* [out] */ Int16* value);

    CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Int16* value);

    CARAPI Get(
        /* [out] */ ArrayOf<Int16>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 int16Count);

    CARAPI IsDirect(
        /* [out] */ Boolean* isDirect);

    CARAPI GetOrder(
        /* [out] */ ByteOrder* byteOrder);

    CARAPI AsReadOnlyBuffer(
        /* [out] */ IInt16Buffer** buffer);

    CARAPI Compact();

    CARAPI Duplicate(
        /* [out] */ IInt16Buffer** buffer);

    CARAPI ProtectedArray(
        /* [out, callee] */ ArrayOf<Int16>** array);

    CARAPI ProtectedArrayOffset(
        /* [out] */ Int32* offset);

    CARAPI ProtectedHasArray(
        /* [out] */ Boolean* hasArray);

    CARAPI Put(
        /* [in] */ Int16 d);

    CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Int16 d);

    CARAPI Put(
        /* [in] */ const ArrayOf<Int16>& src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 doubleCount);

    CARAPI Slice(
        /* [out] */ IInt16Buffer** buffer);

    CARAPI IsReadOnly(
        /* [out] */ Boolean* value);

private:
    static CARAPI Copy(
        /* [in] */ Int16ArrayBuffer* other,
        /* [in] */ Int32 markOfOther,
        /* [in] */ Boolean isReadOnly,
        /* [out] */ Int16ArrayBuffer** buffer);

public:
    AutoPtr< ArrayOf<Int16> > mBackingArray;

    Int32 mArrayOffset;

    Boolean mIsReadOnly;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_INT16ARRAYBUFFER_H__
