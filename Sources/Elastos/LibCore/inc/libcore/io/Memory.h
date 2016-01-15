
#ifndef __LIBCORE_IO_MEMORY_H__
#define __LIBCORE_IO_MEMORY_H__

#include "Elastos.CoreLibrary.h"
#include "io/MappedByteBuffer.h"

using Elastos::IO::ByteOrder;
using Elastos::IO::MappedByteBuffer;

namespace Libcore {
namespace IO {

class Memory
{
public:
    /**
     * Used to optimize nio heap buffer bulk get operations. 'dst' must be a primitive array.
     * 'dstOffset' is measured in units of 'sizeofElements' bytes.
     */
    static CARAPI UnsafeBulkGet(
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
    static CARAPI UnsafeBulkPut(
        /* [out] */ ArrayOf<Byte>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 byteCount,
        /* [in] */ Byte* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 sizeofElements,
        /* [in] */ Boolean swap);

    static CARAPI_(Int32) PeekInt32(
        /* [in] */ ArrayOf<Byte>* src,
        /* [in] */ Int32 offset,
        /* [in] */ ByteOrder order);

    static CARAPI_(Int64) PeekInt64(
        /* [in] */ ArrayOf<Byte>* src,
        /* [in] */ Int32 offset,
        /* [in] */ ByteOrder order);

    static CARAPI_(Int16) PeekInt16(
        /* [in] */ ArrayOf<Byte>* src,
        /* [in] */ Int32 offset,
        /* [in] */ ByteOrder order);

    static CARAPI PokeInt32(
        /* [out] */ ArrayOf<Byte>* dst,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 value,
        /* [in] */ ByteOrder order);

    static CARAPI PokeInt64(
        /* [out] */ ArrayOf<Byte>* dst,
        /* [in] */ Int32 offset,
        /* [in] */ Int64 value,
        /* [in] */ ByteOrder order);

    static CARAPI PokeInt16(
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
    static CARAPI Memmove(
        /* [out] */ ArrayOf<Byte>* dstObject,
        /* [in] */ Int32 dstOffset,
        /* [in] */ ArrayOf<Byte>* srcObject,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int64 byteCount);

    static CARAPI Memmove(
        /* [in] */ MappedByteBuffer* dstObject,
        /* [in] */ Int32 dstOffset,
        /* [in] */ MappedByteBuffer* srcObject,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int64 byteCount);

    static CARAPI_(Byte) PeekByte(
        /* [in] */ Int64 address);

    static CARAPI_(Int32) PeekInt32(
        /* [in] */ Int64 address,
        /* [in] */ Boolean swap);

    static CARAPI_(Int64) PeekInt64(
        /* [in] */ Int64 address,
        /* [in] */ Boolean swap);

    static CARAPI_(Int16) PeekInt16(
        /* [in] */ Int64 address,
        /* [in] */ Boolean swap);

    static CARAPI PeekByteArray(
        /* [in] */ Int64 address,
        /* [out] */ ArrayOf<Byte>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 byteCount);

    static CARAPI PeekCharArray(
        /* [in] */ Int64 address,
        /* [out] */ ArrayOf<Char32>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 charCount,
        /* [in] */ Boolean swap);

    static CARAPI PeekDoubleArray(
        /* [in] */ Int64 address,
        /* [out] */ ArrayOf<Double>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 doubleCount,
        /* [in] */ Boolean swap);

    static CARAPI PeekFloatArray(
        /* [in] */ Int64 address,
        /* [out] */ ArrayOf<Float>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 floatCount,
        /* [in] */ Boolean swap);

    static CARAPI PeekInt32Array(
        /* [in] */ Int64 address,
        /* [out] */ ArrayOf<Int32>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 int32Count,
        /* [in] */ Boolean swap);

    static CARAPI PeekInt64Array(
        /* [in] */ Int64 address,
        /* [out] */ ArrayOf<Int64>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 int64Count,
        /* [in] */ Boolean swap);

    static CARAPI PeekInt16Array(
        /* [in] */ Int64 address,
        /* [out] */ ArrayOf<Int16>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 int16Count,
        /* [in] */ Boolean swap);

    static CARAPI PokeByte(
        /* [in] */ Int64 address,
        /* [in] */ Byte value);

    static CARAPI PokeInt32(
        /* [in] */ Int64 address,
        /* [in] */ Int32 value,
        /* [in] */ Boolean swap);

    static CARAPI PokeInt64(
        /* [in] */ Int64 address,
        /* [in] */ Int64 value,
        /* [in] */ Boolean swap);

    static CARAPI PokeInt16(
        /* [in] */ Int64 address,
        /* [in] */ Int16 value,
        /* [in] */ Boolean swap);

    static CARAPI PokeByteArray(
        /* [in] */ Int64 address,
        /* [out] */ ArrayOf<Byte>* src,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    static CARAPI PokeCharArray(
        /* [in] */ Int64 address,
        /* [out] */ ArrayOf<Char32>* src,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ Boolean swap);

    static CARAPI PokeDoubleArray(
        /* [in] */ Int64 address,
        /* [out] */ ArrayOf<Double>* src,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ Boolean swap);

    static CARAPI PokeFloatArray(
        /* [in] */ Int64 address,
        /* [out] */ ArrayOf<Float>* src,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ Boolean swap);

    static CARAPI PokeInt32Array(
        /* [in] */ Int64 address,
        /* [out] */ ArrayOf<Int32>* src,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ Boolean swap);

    static CARAPI PokeInt64Array(
        /* [in] */ Int64 address,
        /* [out] */ ArrayOf<Int64>* src,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ Boolean swap);

    static CARAPI PokeInt16Array(
        /* [in] */ Int64 address,
        /* [out] */ ArrayOf<Int16>* src,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ Boolean swap);

private:
    Memory();
    Memory(const Memory&);
};

} // namespace IO
} // namespace Libcore

#endif //__LIBCORE_IO_MEMORY_H__
