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

#ifndef __ELASTOS_IO_FLOATARRAYBUFFER_H__
#define __ELASTOS_IO_FLOATARRAYBUFFER_H__

#include "FloatBuffer.h"

namespace Elastos {
namespace IO {
/**
 * FloatArrayBuffer, ReadWriteFloatArrayBuffer and ReadOnlyFloatArrayBuffer
 * compose the implementation of array based float buffers.
 * <p>
 * FloatArrayBuffer implements all the shared readonly methods and is extended
 * by the other two classes.
 * </p>
 * <p>
 * All methods are marked final for runtime performance.
 * </p>
 *
 */
class FloatArrayBuffer
    : public FloatBuffer
{
public:
    FloatArrayBuffer();

    CARAPI constructor(
        /* [in] */ ArrayOf<Float>* array);

    CARAPI constructor(
        /* [in] */ Int32 capacity,
        /* [in] */ ArrayOf<Float>* backingArray,
        /* [in] */ Int32 offset,
        /* [in] */ Boolean isReadOnly);

    CARAPI GetPrimitiveArray(
        /* [out] */ HANDLE* arrayHandle);

    CARAPI Get(
        /* [out] */ Float* value);

    CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Float* value);

    CARAPI Get(
        /* [out] */ ArrayOf<Float>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 floatCount);

    CARAPI IsDirect(
        /* [out] */ Boolean* isDirect);

    CARAPI GetOrder(
        /* [out] */ ByteOrder* byteOrder);

    CARAPI AsReadOnlyBuffer(
        /* [out] */ IFloatBuffer** buffer);

    CARAPI Compact();

    CARAPI Duplicate(
        /* [out] */ IFloatBuffer** buffer);

    CARAPI ProtectedArray(
        /* [out, callee] */ ArrayOf<Float>** array);

    CARAPI ProtectedArrayOffset(
        /* [out] */ Int32* offset);

    CARAPI ProtectedHasArray(
        /* [out] */ Boolean* hasArray);

    CARAPI Put(
        /* [in] */ Float d);

    CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Float d);
    CARAPI Put(
        /* [in] */ const ArrayOf<Float>& src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 doubleCount);

    CARAPI Slice(
        /* [out] */ IFloatBuffer** buffer);

    CARAPI IsReadOnly(
        /* [out] */ Boolean* value);

private:
    static CARAPI Copy(
        /* [in] */ FloatArrayBuffer* other,
        /* [in] */ Int32 markOfOther,
        /* [in] */ Boolean isReadOnly,
        /* [out] */ FloatArrayBuffer** fab);

public:
    AutoPtr< ArrayOf<Float> > mBackingArray;
    Int32 mArrayOffset;
    Boolean mIsReadOnly;

private:
    AutoPtr< ArrayOf<Float> > mArrayTemp;
};
} // namespace IO
} // namespace Elastos
#endif // __ELASTOS_IO_FLOATARRAYBUFFER_H__
