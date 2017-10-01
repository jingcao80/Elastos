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

#ifndef __LIBCORE_IO_CMEMORY_H__
#define __LIBCORE_IO_CMEMORY_H__

#include "_Libcore_IO_CMemory.h"
#include "Singleton.h"

using Elastos::Core::Singleton;
using Elastos::IO::ByteOrder;

namespace Libcore {
namespace IO {

CarClass(CMemory)
    , public Singleton
    , public IMemory
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Used to optimize nio heap buffer bulk get operations. 'dst' must be a primitive array.
     * 'dstOffset' is measured in units of 'sizeofElements' bytes.
     */
    CARAPI UnsafeBulkGet(
        /* [out] */ Byte* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 byteCount,
        /* [in] */ ArrayOf<Byte>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 sizeofElements,
        /* [in] */ Boolean swap);

    /**
     * Used to optimize nio heap buffer bulk put operations. 'src' must be a primitive array.
     * 'srcOffset' is measured in units of 'sizeofElements' bytes.
     */
    CARAPI UnsafeBulkPut(
        /* [out] */ ArrayOf<Byte>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 byteCount,
        /* [in] */ Byte* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 sizeofElements,
        /* [in] */ Boolean swap);

    CARAPI PeekInt32(
        /* [in] */ ArrayOf<Byte>* src,
        /* [in] */ Int32 offset,
        /* [in] */ ByteOrder order,
        /* [out] */ Int32* result);

    CARAPI PeekInt64(
        /* [in] */ ArrayOf<Byte>* src,
        /* [in] */ Int32 offset,
        /* [in] */ ByteOrder order,
        /* [out] */ Int64* result);

    CARAPI PeekInt16(
        /* [in] */ ArrayOf<Byte>* src,
        /* [in] */ Int32 offset,
        /* [in] */ ByteOrder order,
        /* [out] */ Int16* result);

    CARAPI PokeInt32(
        /* [out] */ ArrayOf<Byte>* dst,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 value,
        /* [in] */ ByteOrder order);

    CARAPI PokeInt64(
        /* [out] */ ArrayOf<Byte>* dst,
        /* [in] */ Int32 offset,
        /* [in] */ Int64 value,
        /* [in] */ ByteOrder order);

    CARAPI PokeInt16(
        /* [out] */ ArrayOf<Byte>* dst,
        /* [in] */ Int32 offset,
        /* [in] */ Int16 value,
        /* [in] */ ByteOrder order);

    /**
     * Copies 'byteCount' bytes from the source to the destination. The objects are either
     * instances of DirectByteBuffer or byte[]. The offsets in the byte[] case must include
     * the Buffer.arrayOffset if the array came from a Buffer.array call. We could make this
     * private and provide the four type-safe variants, but then ByteBuffer.put(ByteBuffer)
     * would need to work out which to call based on whether the source and destination buffers
     * are direct or not.
     *
     * @hide make type-safe before making public?
     */
    CARAPI Memmove(
        /* [out] */ ArrayOf<Byte>* dstObject,
        /* [in] */ Int32 dstOffset,
        /* [in] */ ArrayOf<Byte>* srcObject,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int64 byteCount);

    CARAPI PeekByte(
        /* [in] */ Int32 address,
        /* [out] */ Byte* value);

    CARAPI PeekInt32(
        /* [in] */ Int32 address,
        /* [in] */ Boolean swap,
        /* [out] */ Int32* value);

    CARAPI PeekInt64(
        /* [in] */ Int32 address,
        /* [in] */ Boolean swap,
        /* [out] */ Int64* value);

    CARAPI PeekInt16(
        /* [in] */ Int32 address,
        /* [in] */ Boolean swap,
        /* [out] */ Int16* value);

    CARAPI PeekByteArray(
        /* [in] */ Int32 address,
        /* [out] */ ArrayOf<Byte>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 byteCount);

    CARAPI PeekCharArray(
        /* [in] */ Int32 address,
        /* [out] */ ArrayOf<Char32>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 charCount,
        /* [in] */ Boolean swap);

    CARAPI PeekDoubleArray(
        /* [in] */ Int32 address,
        /* [out] */ ArrayOf<Double>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 doubleCount,
        /* [in] */ Boolean swap);

    CARAPI PeekFloatArray(
        /* [in] */ Int32 address,
        /* [out] */ ArrayOf<Float>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 floatCount,
        /* [in] */ Boolean swap);

    CARAPI PeekInt32Array(
        /* [in] */ Int32 address,
        /* [out] */ ArrayOf<Int32>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 int32Count,
        /* [in] */ Boolean swap);

    CARAPI PeekInt64Array(
        /* [in] */ Int32 address,
        /* [out] */ ArrayOf<Int64>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 int64Count,
        /* [in] */ Boolean swap);

    CARAPI PeekInt16Array(
        /* [in] */ Int32 address,
        /* [out] */ ArrayOf<Int16>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 int16Count,
        /* [in] */ Boolean swap);

    CARAPI PokeByte(
        /* [in] */ Int32 address,
        /* [in] */ Byte value);

    CARAPI PokeInt32(
        /* [in] */ Int32 address,
        /* [in] */ Int32 value,
        /* [in] */ Boolean swap);

    CARAPI PokeInt64(
        /* [in] */ Int32 address,
        /* [in] */ Int64 value,
        /* [in] */ Boolean swap);

    CARAPI PokeInt16(
        /* [in] */ Int32 address,
        /* [in] */ Int16 value,
        /* [in] */ Boolean swap);

    CARAPI PokeByteArray(
        /* [in] */ Int32 address,
        /* [out] */ ArrayOf<Byte>* src,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    CARAPI PokeCharArray(
        /* [in] */ Int32 address,
        /* [out] */ ArrayOf<Char32>* src,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ Boolean swap);

    CARAPI PokeDoubleArray(
        /* [in] */ Int32 address,
        /* [out] */ ArrayOf<Double>* src,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ Boolean swap);

    CARAPI PokeFloatArray(
        /* [in] */ Int32 address,
        /* [out] */ ArrayOf<Float>* src,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ Boolean swap);

    CARAPI PokeInt32Array(
        /* [in] */ Int32 address,
        /* [out] */ ArrayOf<Int32>* src,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ Boolean swap);

    CARAPI PokeInt64Array(
        /* [in] */ Int32 address,
        /* [out] */ ArrayOf<Int64>* src,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ Boolean swap);

    CARAPI PokeInt16Array(
        /* [in] */ Int32 address,
        /* [out] */ ArrayOf<Int16>* src,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ Boolean swap);
};

} // namespace IO
} // namespace Libcore

#endif //__LIBCORE_IO_CMEMORY_H__
