#ifndef __ELASTOS_IO_CHARARRAYBUFFER_H__
#define __ELASTOS_IO_CHARARRAYBUFFER_H__

#include "CharBuffer.h"

namespace Elastos {
namespace IO {

/**
 * CharArrayBuffer, ReadWriteCharArrayBuffer and ReadOnlyCharArrayBuffer compose
 * the implementation of array based char buffers.
 * <p>
 * CharArrayBuffer implements all the shared readonly methods and is extended by
 * the other two classes.
 * </p>
 * <p>
 * All methods are marked final for runtime performance.
 * </p>
 *
 */
class CharArrayBuffer
    : public CharBuffer
{
public:
    CharArrayBuffer();

    CARAPI constructor(
        /* [in] */ ArrayOf<Char32>* array);

    CARAPI constructor(
        /* [in] */ Int32 capacity,
        /* [in] */ ArrayOf<Char32>* backingArray,
        /* [in] */ Int32 offset,
        /* [in] */ Boolean isReadOnly);

    CARAPI GetPrimitiveArray(
        /* [out] */ Handle64* arrayHandle);

    CARAPI Get(
        /* [out] */ Char32* value);

    CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Char32* value);

    CARAPI Get(
        /* [out] */ ArrayOf<Char32>* dst);

    CARAPI Get(
        /* [out] */ ArrayOf<Char32>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 charCount);

    CARAPI IsDirect(
        /* [out] */ Boolean* isDirect);

    CARAPI GetOrder(
        /* [out] */ ByteOrder* byteOrder);

    CARAPI SubSequence(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ ICharSequence** csq);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI IsReadOnly(
        /* [out] */ Boolean* value);

    CARAPI AsReadOnlyBuffer(
        /* [out] */ ICharBuffer** buffer);

    CARAPI Compact();

    CARAPI Duplicate(
        /* [out] */ ICharBuffer** buffer);

    CARAPI Put(
        /* [in] */ Char32 c);

    CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Char32 c);

    CARAPI Slice(
        /* [out] */ ICharBuffer** buffer);

protected:
    CARAPI ProtectedArray(
        /* [out, callee] */ ArrayOf<Char32>** array);

    CARAPI ProtectedArrayOffset(
        /* [out] */ Int32* offset);

    CARAPI ProtectedHasArray(
        /* [out] */ Boolean* result);

private:
    static CARAPI Copy(
        /* [in] */ CharArrayBuffer* other,
        /* [in] */ Int32 markOfOther,
        /* [in] */ Boolean isReadOnly,
        /* [out] */ CharArrayBuffer** cab);

public:
    AutoPtr< ArrayOf<Char32> > mBackingArray;

    Int32 mArrayOffset;;

    Boolean mIsReadOnly;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CHARARRAYBUFFER_H__
