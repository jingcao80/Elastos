#ifndef __ELASTOS_IO_INT32ARRAYBUFFER_H__
#define __ELASTOS_IO_INT32ARRAYBUFFER_H__

#include "Int32Buffer.h"

namespace Elastos {
namespace IO {

/**
 * IntArrayBuffer, ReadWriteIntArrayBuffer and ReadOnlyIntArrayBuffer compose
 * the implementation of array based int buffers.
 * <p>
 * IntArrayBuffer implements all the shared readonly methods and is extended by
 * the other two classes.
 * </p>
 * <p>
 * All methods are marked final for runtime performance.
 * </p>
 *
 */
class Int32ArrayBuffer
    : public Int32Buffer
{
public:
    Int32ArrayBuffer();

    CARAPI constructor(
        /* [in] */ ArrayOf<Int32>* array);

    CARAPI constructor(
        /* [in] */ Int32 capacity,
        /* [in] */ ArrayOf<Int32>* backingArray,
        /* [in] */ Int32 offset,
        /* [in] */ Boolean isReadOnly);

    CARAPI Get(
        /* [out] */ Int32* value);

    CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

    CARAPI Get(
        /* [out] */ ArrayOf<Int32>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 int32Count);

    CARAPI IsDirect(
        /* [out] */ Boolean* isDirect);

    CARAPI GetOrder(
        /* [out] */ ByteOrder* byteOrder);

    CARAPI AsReadOnlyBuffer(
        /* [out] */ IInt32Buffer** buffer);

    CARAPI Compact();

    CARAPI Duplicate(
        /* [out] */ IInt32Buffer** buffer);

    CARAPI ProtectedArray(
        /* [out, callee] */ ArrayOf<Int32>** array);

    CARAPI ProtectedArrayOffset(
        /* [out] */ Int32* offset);

    CARAPI ProtectedHasArray(
        /* [out] */ Boolean* hasArray);

    CARAPI Put(
        /* [in] */ Int32 d);

    CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Int32 d);

    CARAPI Put(
        /* [in] */ const ArrayOf<Int32>& src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 doubleCount);

    CARAPI Slice(
        /* [out] */ IInt32Buffer** buffer);

    CARAPI IsReadOnly(
        /* [out] */ Boolean* value);

private:
    static CARAPI Copy(
        /* [in] */ Int32ArrayBuffer* other,
        /* [in] */ Int32 markOfOther,
        /* [in] */ Boolean isReadOnly,
        /* [out] */ Int32ArrayBuffer** buffer);

public:
    AutoPtr< ArrayOf<Int32> > mBackingArray;

    Int32 mArrayOffset;

    Boolean mIsReadOnly;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_INT32ARRAYBUFFER_H__
