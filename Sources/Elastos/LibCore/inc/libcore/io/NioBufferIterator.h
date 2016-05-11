#ifndef __LIBCORE_IO_NIOBUFFERITERATOR_H__
#define __LIBCORE_IO_NIOBUFFERITERATOR_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Libcore {
namespace IO {

class NioBufferIterator
    : public Object
    , public IBufferIterator
{
public:
    CAR_INTERFACE_DECL()

    NioBufferIterator(
        /* [in] */ Int64 address,
        /* [in] */ Int32 size,
        /* [in] */ Boolean swap);

    /**
     * Seeks to the absolute position {offset}, measured in bytes from the start.
     */
    CARAPI Seek(
        /* [in] */ Int32 offset);

    /**
     * Skips forwards or backwards {byteCount} bytes from the current position.
     */
    CARAPI Skip(
        /* [in] */ Int32 byteCount);

    /**
     * Copies {byteCount} bytes from the current position into {dst}, starting at
     * {dstOffset}, and advances the current position {byteCount} bytes.
     */
    CARAPI ReadByteArray(
        /* [in] */ ArrayOf<Byte>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 byteCount);

    /**
     * Returns the byte at the current position, and advances the current position one byte.
     */
    CARAPI ReadByte(
        /* [out] */ Byte* result);

    /**
     * Returns the 32-bit int at the current position, and advances the current position four bytes.
     */
    CARAPI ReadInt32(
        /* [out] */ Int32* result);

    /**
     * Copies {intCount} 32-bit ints from the current position into {dst}, starting at
     * {dstOffset}, and advances the current position {4 * intCount} bytes.
     */
    CARAPI ReadInt32Array(
        /* [in] */ ArrayOf<Int32>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 intCount);

    /**
     * Returns the 16-bit short at the current position, and advances the current position two bytes.
     */
    CARAPI ReadInt16(
        /* [out] */ Int16* result);
private:
    Int64 mAddress;
    Int32 mSize;
    Boolean mSwap;
    Int32 mPosition;
};

} // namespace IO
} // namespace Libcore
#endif
