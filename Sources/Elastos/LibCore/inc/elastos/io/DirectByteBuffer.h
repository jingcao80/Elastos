//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_IO_DIRECTBYTEBUFFER_H__
#define __ELASTOS_IO_DIRECTBYTEBUFFER_H__

#include "MappedByteBuffer.h"
#include "MemoryBlock.h"

namespace Elastos {
namespace IO {

/**
 * DirectByteBuffer, ReadWriteDirectByteBuffer and ReadOnlyDirectByteBuffer
 * compose the implementation of platform memory based byte buffers.
 * <p>
 * DirectByteBuffer implements all the shared readonly methods and is extended
 * by the other two classes.
 * </p>
 * <p>
 * All methods are marked final for runtime performance.
 * </p>
 *
 */
class DirectByteBuffer
    : public MappedByteBuffer
    , public IDirectByteBuffer
{
    friend class ReadWriteDirectByteBuffer;
    friend class ReadOnlyDirectByteBuffer;

public:
    CAR_INTERFACE_DECL()

    CARAPI GetPrimitiveArray(
        /* [out] */ Handle64* arrayHandle);

    CARAPI Get(
        /* [out] */ ArrayOf<Byte>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 byteCount);

    CARAPI GetChars(
        /* [out] */ ArrayOf<Char32>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 charCount);

    CARAPI GetDoubles(
        /* [out] */ ArrayOf<Double>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 doubleCount);

    CARAPI GetFloats(
        /* [out] */ ArrayOf<Float>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 floatCount);

    CARAPI GetInt32s(
        /* [out] */ ArrayOf<Int32>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 int32Count);

    CARAPI GetInt64s(
        /* [out] */ ArrayOf<Int64>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 int64Count);

    CARAPI GetInt16s(
        /* [out] */ ArrayOf<Int16>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 int16Count);

    CARAPI Get(
        /* [out] */ Byte* value);

    CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Byte* value);

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

    CARAPI Free();

    CARAPI ProtectedArray(
        /* [out, callee] */ ArrayOf<Byte>** array);

    CARAPI ProtectedArrayOffset(
        /* [out] */ Int32* offset);

    CARAPI ProtectedHasArray(
        /* [out] */ Boolean* hasArray);

    CARAPI IsReadOnly(
        /* [out] */ Boolean* value);

    virtual CARAPI Put(
        /* [in] */ Byte b);

    virtual CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Byte b);

    virtual CARAPI Put(
        /* [in] */ ArrayOf<Byte>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 byteCount);

    virtual CARAPI PutChar(
        /* [in] */ Char32 value);

    virtual CARAPI PutChar(
        /* [in] */ Int32 index,
        /* [in] */ Char32 value);

    virtual CARAPI PutDouble(
        /* [in] */ Double value);

    virtual CARAPI PutDouble(
        /* [in] */ Int32 index,
        /* [in] */ Double value);

    virtual CARAPI PutFloat(
        /* [in] */ Float value);

    virtual CARAPI PutFloat(
        /* [in] */ Int32 index,
        /* [in] */ Float value);

    virtual CARAPI PutInt16(
        /* [in] */ Int16 value);

    virtual CARAPI PutInt16(
        /* [in] */ Int32 index,
        /* [in] */ Int16 value);

    virtual CARAPI PutInt32(
        /* [in] */ Int32 value);

    virtual CARAPI PutInt32(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value);

    virtual CARAPI PutInt64(
        /* [in] */ Int64 value);

    virtual CARAPI PutInt64(
        /* [in] */ Int32 index,
        /* [in] */ Int64 value);

    virtual CARAPI Slice(
        /* [out] */ IByteBuffer** buffer);

    virtual CARAPI AsCharBuffer(
        /* [out] */ ICharBuffer** buffer);

    virtual CARAPI AsDoubleBuffer(
        /* [out] */ IDoubleBuffer** buffer);

    virtual CARAPI AsFloatBuffer(
        /* [out] */ IFloatBuffer** buffer);

    virtual CARAPI AsInt16Buffer(
        /* [out] */ IInt16Buffer** buffer);

    virtual CARAPI AsInt32Buffer(
        /* [out] */ IInt32Buffer** buffer);

    virtual CARAPI AsInt64Buffer(
        /* [out] */ IInt64Buffer** buffer);

    virtual CARAPI AsReadOnlyBuffer(
        /* [out] */ IByteBuffer** buffer);

    virtual CARAPI Compact();

    virtual CARAPI Duplicate(
        /* [out] */ IByteBuffer** buffer);

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
    DirectByteBuffer();

    CARAPI constructor(
        /* [in] */ Int64 address,
        /* [in] */ Int32 capacity);

    CARAPI constructor(
        /* [in] */ MemoryBlock* block,
        /* [in] */ Int32 capacity,
        /* [in] */ Int32 offset,
        /* [in] */ Boolean isReadOnly,
        /* [in] */ FileChannelMapMode mapMode);

private:
    CARAPI CheckIsAccessible();

    CARAPI CheckNotFreed();

    static CARAPI Copy(
        /* [in] */ DirectByteBuffer* other,
        /* [in] */ Int32 markOfOther,
        /* [in] */ Boolean isReadOnly,
        /* [out] */ DirectByteBuffer** db);

protected:
    // This is the offset into {@code Buffer.block} at which this buffer logically starts.
    // TODO: rewrite this so we set 'block' to an OffsetMemoryBlock?
    Int32 mOffset;

private:
    Boolean mIsReadOnly;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_DIRECTBYTEBUFFER_H__
