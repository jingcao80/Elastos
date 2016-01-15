
#ifndef __ELASTOS_IO_BYTEBUFFERASINT32BUFFER_H__
#define __ELASTOS_IO_BYTEBUFFERASINT32BUFFER_H__

#include "Int32Buffer.h"
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
class ByteBufferAsInt32Buffer
    : public Int32Buffer
{
private:
    ByteBufferAsInt32Buffer();

    CARAPI constructor(
        /* [in] */ ByteBuffer* byteBuffer);

public:
    static CARAPI_(AutoPtr<IInt32Buffer>) AsInt32Buffer(
        /* [in] */ ByteBuffer* byteBuffer);

    // @Override
    CARAPI AsReadOnlyBuffer(
        /* [out] */ IInt32Buffer** buffer);

    // @Override
    CARAPI Compact();

    // @Override
    CARAPI Duplicate(
        /* [out] */ IInt32Buffer** buffer);

    // @Override
    CARAPI Get(
        /* [out] */ Int32* value);

    // @Override
    CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

    // @Override
    CARAPI Get(
        /* [out] */ ArrayOf<Int32>* dst,
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

    CARAPI Put(
        /* [in] */ Int32 c);

    CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Int32 c);

    CARAPI Put(
        /* [in] */ ArrayOf<Int32>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 charCount);

    CARAPI Slice(
        /* [out] */ IInt32Buffer** buffer);

protected:
    CARAPI ProtectedArray(
        /* [out, callee] */ ArrayOf<Int32>** array);

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

#endif // __ELASTOS_IO_BYTEBUFFERASINT32BUFFER_H__
