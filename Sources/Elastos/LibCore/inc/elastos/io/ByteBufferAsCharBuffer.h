
#ifndef __ELASTOS_IO_BYTEBUFFERASCHARBUFFER_H__
#define __ELASTOS_IO_BYTEBUFFERASCHARBUFFER_H__

#include "CharBuffer.h"
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
class ByteBufferAsCharBuffer
    : public CharBuffer
{
private:
    ByteBufferAsCharBuffer();

    CARAPI constructor(
        /* [in] */ ByteBuffer* byteBuffer);

public:
    static CARAPI_(AutoPtr<ICharBuffer>) AsCharBuffer(
        /* [in] */ ByteBuffer* byteBuffer);

    // @Override
    CARAPI AsReadOnlyBuffer(
        /* [out] */ ICharBuffer** buffer);

    // @Override
    CARAPI Compact();

    // @Override
    CARAPI Duplicate(
        /* [out] */ ICharBuffer** buffer);

    // @Override
    CARAPI Get(
        /* [out] */ Char32* value);

    // @Override
    CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Char32* value);

    // @Override
    CARAPI Get(
        /* [out] */ ArrayOf<Char32>* dst,
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
        /* [in] */ Char32 c);

    CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Char32 c);


    CARAPI Put(
        /* [in] */ ArrayOf<Char32>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 charCount);

    CARAPI Slice(
        /* [out] */ ICharBuffer** buffer);

    CARAPI SubSequence(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ ICharSequence** csq);

protected:
    CARAPI ProtectedArray(
        /* [out, callee] */ ArrayOf<Char32>** array);

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

#endif // __ELASTOS_IO_BYTEBUFFERASCHARBUFFER_H__
