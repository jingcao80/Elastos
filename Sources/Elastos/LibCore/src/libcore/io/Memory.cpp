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

#include "Memory.h"
#include "coredef.h"
#include <byteswap.h>

using Elastos::IO::ByteOrder_BIG_ENDIAN;

namespace Libcore {
namespace IO {

#if defined(__arm__)
// 32-bit ARM has load/store alignment restrictions for longs.
#define LONG_ALIGNMENT_MASK 0x3
#define INT_ALIGNMENT_MASK 0x0
#define SHORT_ALIGNMENT_MASK 0x0
#elif defined(__mips__)
// MIPS has load/store alignment restrictions for longs, ints and shorts.
#define LONG_ALIGNMENT_MASK 0x7
#define INT_ALIGNMENT_MASK 0x3
#define SHORT_ALIGNMENT_MASK 0x1
#elif defined(__i386__)
// x86 can load anything at any alignment.
#define LONG_ALIGNMENT_MASK 0x0
#define INT_ALIGNMENT_MASK 0x0
#define SHORT_ALIGNMENT_MASK 0x0
#else
#error unknown load/store alignment restrictions for this architecture
#endif

// Use packed structures for access to unaligned data on targets with alignment restrictions.
// The compiler will generate appropriate code to access these structures without
// generating alignment exceptions.
template <typename T> static inline T get_unaligned(const T* address)
{
    struct unaligned { T v; } __attribute__ ((packed));
    const unaligned* p = reinterpret_cast<const unaligned*>(address);
    return p->v;
}

template <typename T> static inline void put_unaligned(T* address, T v)
{
    struct unaligned { T v; } __attribute__ ((packed));
    unaligned* p = reinterpret_cast<unaligned*>(address);
    p->v = v;
}

template <typename T> static T cast(Int32 address)
{
    return reinterpret_cast<T>(static_cast<uintptr_t>(address));
}

// Byte-swap 2 jshort values packed in a jint.
static inline Int32 bswap_2x16(Int32 v)
{
    // v is initially ABCD
#if defined(__mips__) && defined(__mips_isa_rev) && (__mips_isa_rev >= 2)
    __asm__ volatile ("wsbh %0, %0" : "+r" (v));  // v=BADC
#else
    v = bswap_32(v);                              // v=DCBA
    v = (v << 16) | ((v >> 16) & 0xffff);         // v=BADC
#endif
    return v;
}

static inline void SwapInt16s(Int16* dstShorts, const Int16* srcShorts, size_t count)
{
    // Do 32-bit swaps as long as possible...
    Int32* dst = reinterpret_cast<Int32*>(dstShorts);
    const Int32* src = reinterpret_cast<const Int32*>(srcShorts);

    if ((reinterpret_cast<uintptr_t>(dst) & INT_ALIGNMENT_MASK) == 0 &&
        (reinterpret_cast<uintptr_t>(src) & INT_ALIGNMENT_MASK) == 0) {
        for (size_t i = 0; i < count / 2; ++i) {
            Int32 v = *src++;
            *dst++ = bswap_2x16(v);
        }
        // ...with one last 16-bit swap if necessary.
        if ((count % 2) != 0) {
            Int16 v = *reinterpret_cast<const Int16*>(src);
            *reinterpret_cast<Int16*>(dst) = bswap_16(v);
        }
    }
    else {
        for (size_t i = 0; i < count / 2; ++i) {
            Int32 v = get_unaligned<Int32>(src++);
            put_unaligned<Int32>(dst++, bswap_2x16(v));
        }
        if ((count % 2) != 0) {
          Int16 v = get_unaligned<Int16>(reinterpret_cast<const Int16*>(src));
          put_unaligned<Int16>(reinterpret_cast<Int16*>(dst), bswap_16(v));
        }
    }
}

static inline void SwapInt32s(Int32* dstInts, const Int32* srcInts, size_t count)
{
    if ((reinterpret_cast<uintptr_t>(dstInts) & INT_ALIGNMENT_MASK) == 0 &&
        (reinterpret_cast<uintptr_t>(srcInts) & INT_ALIGNMENT_MASK) == 0) {
        for (size_t i = 0; i < count; ++i) {
            Int32 v = *srcInts++;
            *dstInts++ = bswap_32(v);
        }
    }
    else {
        for (size_t i = 0; i < count; ++i) {
            Int32 v = get_unaligned<int>(srcInts++);
            put_unaligned<Int32>(dstInts++, bswap_32(v));
        }
    }
}

static inline void SwapInt64s(Int64* dstInt64s, const Int64* srcInt64s, size_t count)
{
    Int32* dst = reinterpret_cast<Int32*>(dstInt64s);
    const Int32* src = reinterpret_cast<const Int32*>(srcInt64s);
    if ((reinterpret_cast<uintptr_t>(dstInt64s) & INT_ALIGNMENT_MASK) == 0 &&
        (reinterpret_cast<uintptr_t>(srcInt64s) & INT_ALIGNMENT_MASK) == 0) {
        for (size_t i = 0; i < count; ++i) {
          Int32 v1 = *src++;
          Int32 v2 = *src++;
          *dst++ = bswap_32(v2);
          *dst++ = bswap_32(v1);
        }
    }
    else {
        for (size_t i = 0; i < count; ++i) {
            Int32 v1 = get_unaligned<Int32>(src++);
            Int32 v2 = get_unaligned<Int32>(src++);
            put_unaligned<Int32>(dst++, bswap_32(v2));
            put_unaligned<Int32>(dst++, bswap_32(v1));
        }
    }
}

static void unsafeBulkCopy(Byte* dst, const Byte* src, Int32 byteCount,
    Int32 sizeofElement, Boolean swap)
{
    if (!swap) {
        memcpy(dst, src, byteCount);
        return;
    }

    if (sizeofElement == 2) {
        Int16* dstShorts = reinterpret_cast<Int16*>(dst);
        const Int16* srcShorts = reinterpret_cast<const Int16*>(src);
        SwapInt16s(dstShorts, srcShorts, byteCount / 2);
    }
    else if (sizeofElement == 4) {
        Int32* dstInts = reinterpret_cast<Int32*>(dst);
        const Int32* srcInts = reinterpret_cast<const Int32*>(src);
        SwapInt32s(dstInts, srcInts, byteCount / 4);
    }
    else if (sizeofElement == 8) {
        Int64* dstLongs = reinterpret_cast<Int64*>(dst);
        const Int64* srcLongs = reinterpret_cast<const Int64*>(src);
        SwapInt64s(dstLongs, srcLongs, byteCount / 8);
    }
}

ECode Memory::UnsafeBulkGet(
    /* [out] */ Byte* dstBytes,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ ArrayOf<Byte>* srcBytes,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 sizeofElements,
    /* [in] */ Boolean swap)
{
    VALIDATE_NOT_NULL(srcBytes)
    VALIDATE_NOT_NULL(dstBytes)

    Byte* dst = dstBytes + dstOffset * sizeofElements;
    const Byte* src = srcBytes->GetPayload() + srcOffset;
    unsafeBulkCopy(dst, src, byteCount, sizeofElements, swap);
    return NOERROR;
}

/**
 * Used to optimize nio heap buffer bulk put operations. 'src' must be a primitive array.
 * 'srcOffset' is measured in units of 'sizeofElements' bytes.
 */
ECode Memory::UnsafeBulkPut(
    /* [out] */ ArrayOf<Byte>* dstBytes,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Byte* srcBytes,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 sizeofElements,
    /* [in] */ Boolean swap)
{
    if (dstBytes == NULL || srcBytes == NULL) {
        return NOERROR;
    }
    Byte* dst = dstBytes->GetPayload() + dstOffset;
    const Byte* src = srcBytes + srcOffset * sizeofElements;
    unsafeBulkCopy(dst, src, byteCount, sizeofElements, swap);
    return NOERROR;
}

Int32 Memory::PeekInt32(
    /* [in] */ ArrayOf<Byte>* src,
    /* [in] */ Int32 offset,
    /* [in] */ ByteOrder order)
{
    assert(src);

    if (order == ByteOrder_BIG_ENDIAN) {
        return ((Int32)(((*src)[offset] & 0xff) << 24) |
                (((*src)[offset + 1] & 0xff) << 16) |
                (((*src)[offset + 2] & 0xff) << 8) |
                (((*src)[offset + 3] & 0xff) << 0));
    }
    else {
        return ((Int32)(((*src)[offset] & 0xff) << 0) |
                (((*src)[offset + 1] & 0xff) << 8) |
                (((*src)[offset + 2] & 0xff) << 16) |
                (((*src)[offset + 3] & 0xff) << 24));
    }
}

Int64 Memory::PeekInt64(
    /* [in] */ ArrayOf<Byte>* src,
    /* [in] */ Int32 offset,
    /* [in] */ ByteOrder order)
{
    assert(src);

    if (order == ByteOrder_BIG_ENDIAN) {
        Int32 h = (((*src)[offset] & 0xff) << 24) |
                (((*src)[offset + 1] & 0xff) << 16) |
                (((*src)[offset + 2] & 0xff) << 8) |
                (((*src)[offset + 3] & 0xff) << 0);
        Int32 l = (((*src)[offset + 4] & 0xff) << 24) |
                (((*src)[offset + 5] & 0xff) << 16) |
                (((*src)[offset + 6] & 0xff) << 8) |
                (((*src)[offset + 7] & 0xff) << 0);
        return (((Int64) h) << 32LL) | (((Int64) l) & 0xffffffffLL);
    }
    else {
        Int32 l = (((*src)[offset] & 0xff) << 0) |
                (((*src)[offset + 1] & 0xff) << 8) |
                (((*src)[offset + 2] & 0xff) << 16) |
                (((*src)[offset + 3] & 0xff) << 24);
        Int32 h = (((*src)[offset + 4] & 0xff) << 0) |
                (((*src)[offset + 5] & 0xff) << 8) |
                (((*src)[offset + 6] & 0xff) << 16) |
                (((*src)[offset + 7] & 0xff) << 24);
        return (((Int64) h) << 32LL) | (((Int64) l) & 0xffffffffLL);
    }
}

Int16 Memory::PeekInt16(
    /* [in] */ ArrayOf<Byte>* src,
    /* [in] */ Int32 offset,
    /* [in] */ ByteOrder order)
{
    assert(src);

    if (order == ByteOrder_BIG_ENDIAN) {
        return (Int16) (((*src)[offset] << 8) | ((*src)[offset + 1] & 0xff));
    }
    else {
        return (Int16) (((*src)[offset + 1] << 8) | ((*src)[offset] & 0xff));
    }
}

ECode Memory::PokeInt32(
    /* [out] */ ArrayOf<Byte>* dst,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 value,
    /* [in] */ ByteOrder order)
{
    VALIDATE_NOT_NULL(dst);
    if (order == ByteOrder_BIG_ENDIAN) {
        (*dst)[offset] = (Byte) ((value >> 24) & 0xff);
        (*dst)[++offset] = (Byte) ((value >> 16) & 0xff);
        (*dst)[++offset] = (Byte) ((value >> 8) & 0xff);
        (*dst)[++offset] = (Byte) ((value >> 0) & 0xff);
    }
    else {
        (*dst)[offset] = (Byte) ((value >> 0) & 0xff);
        (*dst)[++offset] = (Byte) ((value >> 8) & 0xff);
        (*dst)[++offset] = (Byte) ((value >> 16) & 0xff);
        (*dst)[++offset] = (Byte) ((value >> 24) & 0xff);
    }
    return NOERROR;
}

ECode Memory::PokeInt64(
    /* [out] */ ArrayOf<Byte>* dst,
    /* [in] */ Int32 offset,
    /* [in] */ Int64 value,
    /* [in] */ ByteOrder order)
{
    VALIDATE_NOT_NULL(dst);
    if (order == ByteOrder_BIG_ENDIAN) {
        Int32 i = (Int32) (value >> 32);
        (*dst)[offset] = (Byte) ((i >> 24) & 0xff);
        (*dst)[++offset] = (Byte) ((i >> 16) & 0xff);
        (*dst)[++offset] = (Byte) ((i >> 8) & 0xff);
        (*dst)[++offset] = (Byte) ((i >> 0) & 0xff);
        i = (Int32) value;
        (*dst)[++offset] = (Byte) ((i >> 24) & 0xff);
        (*dst)[++offset] = (Byte) ((i >> 16) & 0xff);
        (*dst)[++offset] = (Byte) ((i >> 8) & 0xff);
        (*dst)[++offset] = (Byte) ((i >> 0) & 0xff);
    }
    else {
        Int32 i = (Int32) value;
        (*dst)[offset] = (Byte) ((i >> 0) & 0xff);
        (*dst)[++offset] = (Byte) ((i >> 8) & 0xff);
        (*dst)[++offset] = (Byte) ((i >> 16) & 0xff);
        (*dst)[++offset] = (Byte) ((i >> 24) & 0xff);
        i = (Int32) (value >> 32);
        (*dst)[++offset] = (Byte) ((i >> 0) & 0xff);
        (*dst)[++offset] = (Byte) ((i >> 8) & 0xff);
        (*dst)[++offset] = (Byte) ((i >> 16) & 0xff);
        (*dst)[++offset] = (Byte) ((i >> 24) & 0xff);
    }
    return NOERROR;
}

ECode Memory::PokeInt16(
    /* [out] */ ArrayOf<Byte>* dst,
    /* [in] */ Int32 offset,
    /* [in] */ Int16 value,
    /* [in] */ ByteOrder order)
{
    VALIDATE_NOT_NULL(dst);
    if (order == ByteOrder_BIG_ENDIAN) {
        (*dst)[offset] = (Byte) ((value >> 8) & 0xff);
        (*dst)[++offset] = (Byte) ((value >> 0) & 0xff);
    }
    else {
        (*dst)[offset] = (Byte) ((value >> 0) & 0xff);
        (*dst)[++offset] = (Byte) ((value >> 8) & 0xff);
    }
    return NOERROR;
}

ECode Memory::Memmove(
    /* [out] */ ArrayOf<Byte>* dstBytes,
    /* [in] */ Int32 dstOffset,
    /* [in] */ ArrayOf<Byte>* srcBytes,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int64 byteCount)
{
    VALIDATE_NOT_NULL(srcBytes)
    VALIDATE_NOT_NULL(dstBytes)

    memmove(dstBytes->GetPayload() + dstOffset, srcBytes->GetPayload() + srcOffset, byteCount);
    return NOERROR;
}

ECode Memory::Memmove(
    /* [in] */ MappedByteBuffer* dstObj,
    /* [in] */ Int32 dstOffset,
    /* [in] */ MappedByteBuffer* srcObj,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int64 byteCount)
{
    VALIDATE_NOT_NULL(srcObj)
    VALIDATE_NOT_NULL(dstObj)

    memmove(dstObj->mBlock->GetRawAddress() + dstOffset,
        dstObj->mBlock->GetRawAddress() + srcOffset, byteCount);
    return NOERROR;
}

Byte Memory::PeekByte(
    /* [in] */ Int64 address)
{
    return *cast<const Byte*>(address);
}

Int32 Memory::PeekInt32(
    /* [in] */ Int64 address,
    /* [in] */ Boolean swap)
{
    Int32 result = *cast<const Int32*>(address);
    if (swap) {
        result = bswap_32(result);
    }
    return result;
}

Int64 Memory::PeekInt64(
    /* [in] */ Int64 address,
    /* [in] */ Boolean swap)
{
    Int64 result;
    const Int64* src = cast<const Int64*>(address);
    if ((address & LONG_ALIGNMENT_MASK) == 0) {
        result = *src;
    } else {
        result = get_unaligned<Int64>(src);
    }
    if (swap) {
        result = bswap_64(result);
    }
    return result;
}

Int16 Memory::PeekInt16(
    /* [in] */ Int64 address,
    /* [in] */ Boolean swap)
{
    Int16 result = *cast<const Int16*>(address);
    if (swap) {
        result = bswap_16(result);
    }
    return result;
}

// Implements the peekXArray methods:
// - For unswapped access, we just use the JNI SetXArrayRegion functions.
// - For swapped access, we use GetXArrayElements and our own copy-and-swap routines.
//   GetXArrayElements is disproportionately cheap on Dalvik because it doesn't copy (as opposed
//   to Hotspot, which always copies). The SWAP_FN copies and swaps in one pass, which is cheaper
//   than copying and then swapping in a second pass. Depending on future VM/GC changes, the
//   swapped case might need to be revisited.
#define PEEKER(SCALAR_TYPE, SWAP_TYPE, SWAP_FN) { \
    if (swap) { \
        if (dst == NULL) { \
            return NOERROR; \
        } \
        const SWAP_TYPE* src = cast<const SWAP_TYPE*>(address); \
        SWAP_FN(reinterpret_cast<SWAP_TYPE*>(dst->GetPayload()) + dstOffset, src, count); \
    } else { \
        const SCALAR_TYPE* src = cast<const SCALAR_TYPE*>(address); \
        dst->Copy(dstOffset, src, count); \
    } \
    return NOERROR; \
}

ECode Memory::PeekByteArray(
    /* [in] */ Int64 address,
    /* [out] */ ArrayOf<Byte>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 count)
{
    if (dst == NULL) {
        return NOERROR;
    }
    dst->Copy(dstOffset, cast<const Byte*>(address), count);
    return NOERROR;
}

ECode Memory::PeekCharArray(
    /* [in] */ Int64 address,
    /* [out] */ ArrayOf<Char32>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 count,
    /* [in] */ Boolean swap)
{
    PEEKER(Char32, Int32, SwapInt32s);
}

ECode Memory::PeekDoubleArray(
    /* [in] */ Int64 address,
    /* [out] */ ArrayOf<Double>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 count,
    /* [in] */ Boolean swap)
{
    PEEKER(Double, Int64, SwapInt64s);
}

ECode Memory::PeekFloatArray(
    /* [in] */ Int64 address,
    /* [out] */ ArrayOf<Float>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 count,
    /* [in] */ Boolean swap)
{
    PEEKER(Float, Int32, SwapInt32s);
}

ECode Memory::PeekInt32Array(
    /* [in] */ Int64 address,
    /* [out] */ ArrayOf<Int32>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 count,
    /* [in] */ Boolean swap)
{
    PEEKER(Int32, Int32, SwapInt32s);
}

ECode Memory::PeekInt64Array(
    /* [in] */ Int64 address,
    /* [out] */ ArrayOf<Int64>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 count,
    /* [in] */ Boolean swap)
{
    PEEKER(Int64, Int64, SwapInt64s);
}

ECode Memory::PeekInt16Array(
    /* [in] */ Int64 address,
    /* [out] */ ArrayOf<Int16>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 count,
    /* [in] */ Boolean swap)
{
    PEEKER(Int16, Int16, SwapInt16s);
}

ECode Memory::PokeByte(
    /* [in] */ Int64 dstAddress,
    /* [in] */ Byte value)
{
    *cast<Byte*>(dstAddress) = value;
    return NOERROR;
}

ECode Memory::PokeInt32(
    /* [in] */ Int64 dstAddress,
    /* [in] */ Int32 value,
    /* [in] */ Boolean swap)
{
    if (swap) {
        value = bswap_32(value);
    }
    *cast<Int32*>(dstAddress) = value;
    return NOERROR;
}

ECode Memory::PokeInt64(
    /* [in] */ Int64 dstAddress,
    /* [in] */ Int64 value,
    /* [in] */ Boolean swap)
{
    Int64* dst = cast<Int64*>(dstAddress);
    if (swap) {
        value = bswap_64(value);
    }
    if ((dstAddress & LONG_ALIGNMENT_MASK) == 0) {
        *dst = value;
    }
    else {
        put_unaligned<Int64>(dst, value);
    }
    return NOERROR;
}

ECode Memory::PokeInt16(
    /* [in] */ Int64 dstAddress,
    /* [in] */ Int16 value,
    /* [in] */ Boolean swap)
{
    if (swap) {
        value = bswap_16(value);
    }
    *cast<Int16*>(dstAddress) = value;
    return NOERROR;
}

ECode Memory::PokeByteArray(
    /* [in] */ Int64 dstAddress,
    /* [out] */ ArrayOf<Byte>* src,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    if (src == NULL) {
        return NOERROR;
    }
    src->Copy(offset, cast<Byte*>(dstAddress), count);
    return NOERROR;
}

// Implements the pokeXArray methods:
// - For unswapped access, we just use the JNI GetXArrayRegion functions.
// - For swapped access, we use GetXArrayElements and our own copy-and-swap routines.
//   GetXArrayElements is disproportionately cheap on Dalvik because it doesn't copy (as opposed
//   to Hotspot, which always copies). The SWAP_FN copies and swaps in one pass, which is cheaper
//   than copying and then swapping in a second pass. Depending on future VM/GC changes, the
//   swapped case might need to be revisited.
#define POKER(SCALAR_TYPE, SWAP_TYPE, SWAP_FN) { \
    if (swap) { \
        if (srcArray == NULL) { \
            return NOERROR; \
        } \
        const SWAP_TYPE* src = reinterpret_cast<const SWAP_TYPE*>(srcArray->GetPayload()) + srcOffset; \
        SWAP_FN(cast<SWAP_TYPE*>(dstAddress), src, count); \
    } else { \
        SCALAR_TYPE* pDst = cast<SCALAR_TYPE*>(dstAddress); \
        memcpy(pDst, srcArray->GetPayload(), count * sizeof(SCALAR_TYPE)); \
    } \
    return NOERROR; \
}

ECode Memory::PokeCharArray(
    /* [in] */ Int64 dstAddress,
    /* [out] */ ArrayOf<Char32>* srcArray,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 count,
    /* [in] */ Boolean swap)
{
    POKER(Char32, Int32, SwapInt32s);
}

ECode Memory::PokeDoubleArray(
    /* [in] */ Int64 dstAddress,
    /* [out] */ ArrayOf<Double>* srcArray,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 count,
    /* [in] */ Boolean swap)
{
    POKER(Double, Int64, SwapInt64s);
}

ECode Memory::PokeFloatArray(
    /* [in] */ Int64 dstAddress,
    /* [out] */ ArrayOf<Float>* srcArray,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 count,
    /* [in] */ Boolean swap)
{
    POKER(Float, Int32, SwapInt32s);
}

ECode Memory::PokeInt32Array(
    /* [in] */ Int64 dstAddress,
    /* [out] */ ArrayOf<Int32>* srcArray,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 count,
    /* [in] */ Boolean swap)
{
    POKER(Int32, Int32, SwapInt32s);
}

ECode Memory::PokeInt64Array(
    /* [in] */ Int64 dstAddress,
    /* [out] */ ArrayOf<Int64>* srcArray,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 count,
    /* [in] */ Boolean swap)
{
    POKER(Int64, Int64, SwapInt64s);
}

ECode Memory::PokeInt16Array(
    /* [in] */ Int64 dstAddress,
    /* [out] */ ArrayOf<Int16>* srcArray,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 count,
    /* [in] */ Boolean swap)
{
    POKER(Int16, Int16, SwapInt16s);
}

} // namespace IO
} // namespace Libcore
