
#ifndef __ELASTOS_IO_BYTEARRAYBUFFER_H__
#define __ELASTOS_IO_BYTEARRAYBUFFER_H__

#include "ByteBuffer.h"

namespace Elastos {
namespace IO {

class ByteArrayBuffer
    : public ByteBuffer
    , public IByteArrayBuffer
{
public:
    ByteArrayBuffer();

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* backingArray);

    CARAPI constructor(
        /* [in] */ Int32 capacity,
        /* [in] */ ArrayOf<Byte>* backingArray,
        /* [in] */ Int32 arrayOffset,
        /* [in] */ Boolean isReadOnly);

    CAR_INTERFACE_DECL()

    CARAPI AsReadOnlyBuffer(
        /* [out] */ IByteBuffer** buffer);

    CARAPI Compact();

    CARAPI Duplicate(
        /* [out] */ IByteBuffer** buffer);

    CARAPI Slice(
        /* [out] */ IByteBuffer** buffer);

    CARAPI IsReadOnly(
        /* [out] */ Boolean* isReadOnly);

    CARAPI ProtectedArray(
        /* [out, callee] */ ArrayOf<Byte>** array);

    CARAPI ProtectedArrayOffset(
        /* [out] */ Int32* offset);

    CARAPI ProtectedHasArray(
        /* [out] */ Boolean* hasArray);

    CARAPI Get(
        /* [out] */ Byte* value);

    CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Byte* value);

    CARAPI Get(
        /* [in] */ ArrayOf<Byte>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 byteCount);

    CARAPI GetChar(
        /* [out] */ Char32* value);

    CARAPI GetChar(
        /* [in] */ Int32 index,
        /* [out] */ Char32* value);

    CARAPI GetDouble(
        /* [out] */ Double* value);

    CARAPI GetDouble(
        /* [in] */ Int32 index,
        /* [out] */ Double* value);

    CARAPI GetFloat(
        /* [out] */ Float* value);

    CARAPI GetFloat(
        /* [in] */ Int32 index,
        /* [out] */ Float* value);

    CARAPI GetInt32(
        /* [out] */ Int32* value);

    CARAPI GetInt32(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

    CARAPI GetInt64(
        /* [out] */ Int64* value);

    CARAPI GetInt64(
        /* [in] */ Int32 index,
        /* [out] */ Int64* value);

    CARAPI GetInt16(
        /* [out] */ Int16* value);

    CARAPI GetInt16(
        /* [in] */ Int32 index,
        /* [out] */ Int16* value);

    CARAPI IsDirect(
        /* [out] */ Boolean* isDirect);

    CARAPI Put(
        /* [in] */ Byte b);

    CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Byte b);

    CARAPI Put(
        /* [in] */ ArrayOf<Byte>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 byteCount);

    CARAPI PutChar(
        /* [in] */ Char32 value);

    CARAPI PutChar(
        /* [in] */ Int32 index,
        /* [in] */ Char32 value);

    CARAPI PutDouble(
        /* [in] */ Double value);

    CARAPI PutDouble(
        /* [in] */ Int32 index,
        /* [in] */ Double value);

    CARAPI PutFloat(
        /* [in] */ Float value);

    CARAPI PutFloat(
        /* [in] */ Int32 index,
        /* [in] */ Float value);

    CARAPI PutInt16(
        /* [in] */ Int16 value);

    CARAPI PutInt16(
        /* [in] */ Int32 index,
        /* [in] */ Int16 value);

    CARAPI PutInt32(
        /* [in] */ Int32 value);

    CARAPI PutInt32(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value);

    CARAPI PutInt64(
        /* [in] */ Int64 value);

    CARAPI PutInt64(
        /* [in] */ Int32 index,
        /* [in] */ Int64 value);

    CARAPI AsCharBuffer(
        /* [out] */ ICharBuffer** buffer);

    CARAPI AsDoubleBuffer(
        /* [out] */ IDoubleBuffer** buffer);

    CARAPI AsFloatBuffer(
        /* [out] */ IFloatBuffer** buffer);

    CARAPI AsInt16Buffer(
        /* [out] */ IInt16Buffer** buffer);

    CARAPI AsInt32Buffer(
        /* [out] */ IInt32Buffer** buffer);

    CARAPI AsInt64Buffer(
        /* [out] */ IInt64Buffer** buffer);

    CARAPI GetChars(
        /* [in] */ ArrayOf<Char32>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 charCount);

    CARAPI GetDoubles(
        /* [in] */ ArrayOf<Double>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 doubleCount);

    CARAPI GetFloats(
        /* [in] */ ArrayOf<Float>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 floatCount);

    CARAPI GetInt32s(
        /* [in] */ ArrayOf<Int32>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 intCount);

    CARAPI GetInt64s(
        /* [in] */ ArrayOf<Int64>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 longCount);

    CARAPI GetInt16s(
        /* [in] */ ArrayOf<Int16>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 shortCount);

    CARAPI PutChars(
        /* [in] */ ArrayOf<Char32>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 charCount);

    CARAPI PutDoubles(
        /* [in] */ ArrayOf<Double>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 doubleCount);

    CARAPI PutFloats(
        /* [in] */ ArrayOf<Float>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 floatCount);

    CARAPI PutInt32s(
        /* [in] */ ArrayOf<Int32>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 intCount);

    CARAPI PutInt64s(
        /* [in] */ ArrayOf<Int64>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 longCount);

    CARAPI PutInt16s(
        /* [in] */ ArrayOf<Int16>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 shortCount);
public:
    /**
     * These fields are non-private for NioUtils.unsafeArray.
     */
    AutoPtr<ArrayOf<Byte> > mBackingArray;

    Int32 mArrayOffset;

private:
    static CARAPI Copy(
        /* [in] */ ByteArrayBuffer* other,
        /* [in] */ Int32 markOfOther,
        /* [in] */ Boolean isReadOnly,
        /* [out] */ IByteBuffer** bab);

    Boolean mIsReadOnly;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_BYTEARRAYBUFFER_H__
