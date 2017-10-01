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

#ifndef __ELASTOS_IO_BYTEBUFFERASINT64BUFFER_H__
#define __ELASTOS_IO_BYTEBUFFERASINT64BUFFER_H__

#include "Int64Buffer.h"
#include "ByteBuffer.h"

namespace Elastos {
namespace IO {

/**
 * A buffer for bytes.
 * <p>
 * A byte buffer can be created in either one of the following ways:
 * <ul>
 * <li>{@link #allocate(int) Allocate} a new byte array and create a buffer
 * based on it;</li>
 * <li>{@link #allocateDirect(int) Allocate} a memory block and create a direct
 * buffer based on it;</li>
 * <li>{@link #wrap(byte[]) Wrap} an existing byte array to create a new
 * buffer.</li>
 * </ul>
 *
 */
class ByteBufferAsInt64Buffer
    : public Int64Buffer
{
private:
    ByteBufferAsInt64Buffer();

    CARAPI constructor(
        /* [in] */ ByteBuffer* byteBuffer);

public:
    static CARAPI_(AutoPtr<IInt64Buffer>) AsInt64Buffer(
        /* [in] */ IByteBuffer* byteBuffer);

    CARAPI GetPrimitiveArray(
        /* [out] */ Handle64* arrayHandle);

    // @Override
    CARAPI AsReadOnlyBuffer(
        /* [out] */ IInt64Buffer** buffer);

    // @Override
    CARAPI Compact();

    // @Override
    CARAPI Duplicate(
        /* [out] */ IInt64Buffer** buffer);

    // @Override
    CARAPI Get(
        /* [out] */ Int64* value);

    // @Override
    CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Int64* value);

    // @Override
    CARAPI Get(
        /* [out] */ ArrayOf<Int64>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 charCount);

    // @Override
    CARAPI IsDirect(
        /* [out] */ Boolean* rst);

    // @Override
    CARAPI IsReadOnly(
        /* [out] */ Boolean* rst);

    // @Override
    CARAPI GetOrder(
        /* [out] */ ByteOrder* byteOrder);

    using Int64Buffer::Put;

    CARAPI Put(
        /* [in] */ Int64 c);

    CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Int64 c);

    CARAPI Put(
        /* [in] */ ArrayOf<Int64>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 charCount);

    CARAPI Slice(
        /* [out] */ IInt64Buffer** buffer);

protected:
    CARAPI ProtectedArray(
        /* [out, callee] */ ArrayOf<Int64>** array);

    /**
     * Child class implements this method to realize {@code arrayOffset()}.
     *
     * @see #arrayOffset()
     */
    CARAPI ProtectedArrayOffset(
        /* [out] */ Int32* offset);

    /**
     * Child class implements this method to realize {@code hasArray()}.
     *
     * @see #hasArray()
     */
    CARAPI ProtectedHasArray(
        /* [out] */ Boolean* result);

private:
    AutoPtr<ByteBuffer> mByteBuffer;
    Int32 mCap;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_BYTEBUFFERASINT64BUFFER_H__
