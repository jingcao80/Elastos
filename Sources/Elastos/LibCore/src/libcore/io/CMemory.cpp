
#include "CMemory.h"
#include "Memory.h"

namespace Libcore {
namespace IO {

CAR_INTERFACE_IMPL(CMemory, Singleton, IMemory)

CAR_SINGLETON_IMPL(CMemory)

ECode CMemory::UnsafeBulkGet(
    /* [out] */ Byte* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ ArrayOf<Byte>* src,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 sizeofElements,
    /* [in] */ Boolean swap)
{
    VALIDATE_NOT_NULL(src)
    return Memory::UnsafeBulkGet(dst, dstOffset, byteCount,
            src, srcOffset, sizeofElements, swap);
}

ECode CMemory::UnsafeBulkPut(
    /* [out] */ ArrayOf<Byte>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Byte* src,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 sizeofElements,
    /* [in] */ Boolean swap)
{
    return Memory::UnsafeBulkPut(dst, dstOffset, byteCount,
            src, srcOffset, sizeofElements, swap);
}

ECode CMemory::PeekInt32(
    /* [in] */ ArrayOf<Byte>* src,
    /* [in] */ Int32 offset,
    /* [in] */ ByteOrder order,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    VALIDATE_NOT_NULL(src)
    *result = Memory::PeekInt32(src, offset, order);
    return NOERROR;
}

ECode CMemory::PeekInt64(
    /* [in] */ ArrayOf<Byte>* src,
    /* [in] */ Int32 offset,
    /* [in] */ ByteOrder order,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    VALIDATE_NOT_NULL(src)
    *result = Memory::PeekInt64(src, offset, order);
    return NOERROR;
}

ECode CMemory::PeekInt16(
    /* [in] */ ArrayOf<Byte>* src,
    /* [in] */ Int32 offset,
    /* [in] */ ByteOrder order,
    /* [out] */ Int16* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    VALIDATE_NOT_NULL(src)
    *result = Memory::PeekInt16(src, offset, order);
    return NOERROR;
}

ECode CMemory::PokeInt32(
    /* [out] */ ArrayOf<Byte>* dst,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 value,
    /* [in] */ ByteOrder order)
{
    return Memory::PokeInt32(dst, offset, value, order);
}

ECode CMemory::PokeInt64(
    /* [out] */ ArrayOf<Byte>* dst,
    /* [in] */ Int32 offset,
    /* [in] */ Int64 value,
    /* [in] */ ByteOrder order)
{
    return Memory::PokeInt64(dst, offset, value, order);
}

ECode CMemory::PokeInt16(
    /* [out] */ ArrayOf<Byte>* dst,
    /* [in] */ Int32 offset,
    /* [in] */ Int16 value,
    /* [in] */ ByteOrder order)
{
    return Memory::PokeInt16(dst, offset, value, order);
}

ECode CMemory::Memmove(
    /* [out] */ ArrayOf<Byte>* dstObject,
    /* [in] */ Int32 dstOffset,
    /* [in] */ ArrayOf<Byte>* srcObject,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int64 byteCount)
{
    return Memory::Memmove(dstObject, dstOffset, srcObject, srcOffset, byteCount);
}

ECode CMemory::PeekByte(
    /* [in] */ Int32 address,
    /* [out] */ Byte* value)
{
    VALIDATE_NOT_NULL(value);
    *value = Memory::PeekByte(address);
    return NOERROR;
}

ECode CMemory::PeekInt32(
    /* [in] */ Int32 address,
    /* [in] */ Boolean swap,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = Memory::PeekInt32(address, swap);
    return NOERROR;
}

ECode CMemory::PeekInt64(
    /* [in] */ Int32 address,
    /* [in] */ Boolean swap,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);
    *value = Memory::PeekInt64(address, swap);
    return NOERROR;
}

ECode CMemory::PeekInt16(
    /* [in] */ Int32 address,
    /* [in] */ Boolean swap,
    /* [out] */ Int16* value)
{
    VALIDATE_NOT_NULL(value);
    *value = Memory::PeekInt16(address, swap);
    return NOERROR;
}

ECode CMemory::PeekByteArray(
    /* [in] */ Int32 address,
    /* [out] */ ArrayOf<Byte>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 byteCount)
{
    return Memory::PeekByteArray(address, dst, dstOffset, byteCount);
}

ECode CMemory::PeekCharArray(
    /* [in] */ Int32 address,
    /* [out] */ ArrayOf<Char32>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 charCount,
    /* [in] */ Boolean swap)
{
    return Memory::PeekCharArray(address, dst, dstOffset, charCount, swap);
}

ECode CMemory::PeekDoubleArray(
    /* [in] */ Int32 address,
    /* [out] */ ArrayOf<Double>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 doubleCount,
    /* [in] */ Boolean swap)
{
    return Memory::PeekDoubleArray(address, dst, dstOffset, doubleCount, swap);
}

ECode CMemory::PeekFloatArray(
    /* [in] */ Int32 address,
    /* [out] */ ArrayOf<Float>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 floatCount,
    /* [in] */ Boolean swap)
{
    return Memory::PeekFloatArray(address, dst, dstOffset, floatCount, swap);
}

ECode CMemory::PeekInt32Array(
    /* [in] */ Int32 address,
    /* [out] */ ArrayOf<Int32>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 int32Count,
    /* [in] */ Boolean swap)
{
    return Memory::PeekInt32Array(address, dst, dstOffset, int32Count, swap);
}

ECode CMemory::PeekInt64Array(
    /* [in] */ Int32 address,
    /* [out] */ ArrayOf<Int64>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 int64Count,
    /* [in] */ Boolean swap)
{
    return Memory::PeekInt64Array(address, dst, dstOffset, int64Count, swap);
}

ECode CMemory::PeekInt16Array(
    /* [in] */ Int32 address,
    /* [out] */ ArrayOf<Int16>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 int16Count,
    /* [in] */ Boolean swap)
{
    return Memory::PeekInt16Array(address, dst, dstOffset, int16Count, swap);
}

ECode CMemory::PokeByte(
    /* [in] */ Int32 address,
    /* [in] */ Byte value)
{
    return Memory::PokeByte(address, value);
}

ECode CMemory::PokeInt32(
    /* [in] */ Int32 address,
    /* [in] */ Int32 value,
    /* [in] */ Boolean swap)
{
    return Memory::PokeInt32(address, value, swap);
}

ECode CMemory::PokeInt64(
    /* [in] */ Int32 address,
    /* [in] */ Int64 value,
    /* [in] */ Boolean swap)
{
    return Memory::PokeInt64(address, value, swap);
}

ECode CMemory::PokeInt16(
    /* [in] */ Int32 address,
    /* [in] */ Int16 value,
    /* [in] */ Boolean swap)
{
    return Memory::PokeInt16(address, value, swap);
}

ECode CMemory::PokeByteArray(
    /* [in] */ Int32 address,
    /* [out] */ ArrayOf<Byte>* src,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    VALIDATE_NOT_NULL(src)
    return Memory::PokeByteArray(address, src, offset, count);
}

ECode CMemory::PokeCharArray(
    /* [in] */ Int32 address,
    /* [out] */ ArrayOf<Char32>* src,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ Boolean swap)
{
    VALIDATE_NOT_NULL(src)
    return Memory::PokeCharArray(address, src, offset, count, swap);
}

ECode CMemory::PokeDoubleArray(
    /* [in] */ Int32 address,
    /* [out] */ ArrayOf<Double>* src,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ Boolean swap)
{
    VALIDATE_NOT_NULL(src)
    return Memory::PokeDoubleArray(address, src, offset, count, swap);
}

ECode CMemory::PokeFloatArray(
    /* [in] */ Int32 address,
    /* [out] */ ArrayOf<Float>* src,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ Boolean swap)
{
    VALIDATE_NOT_NULL(src)
    return Memory::PokeFloatArray(address, src, offset, count, swap);
}

ECode CMemory::PokeInt32Array(
    /* [in] */ Int32 address,
    /* [out] */ ArrayOf<Int32>* src,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ Boolean swap)
{
    VALIDATE_NOT_NULL(src)
    return Memory::PokeInt32Array(address, src, offset, count, swap);
}

ECode CMemory::PokeInt64Array(
    /* [in] */ Int32 address,
    /* [out] */ ArrayOf<Int64>* src,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ Boolean swap)
{
    VALIDATE_NOT_NULL(src)
    return Memory::PokeInt64Array(address, src, offset, count, swap);
}

ECode CMemory::PokeInt16Array(
    /* [in] */ Int32 address,
    /* [out] */ ArrayOf<Int16>* src,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ Boolean swap)
{
    VALIDATE_NOT_NULL(src)
    return Memory::PokeInt16Array(address, src, offset, count, swap);
}

} // namespace IO
} // namespace Libcore
