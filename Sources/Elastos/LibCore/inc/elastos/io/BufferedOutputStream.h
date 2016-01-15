
#ifndef __ELASTOS_IO_BUFFEREDOUTPUTSTREAM_H__
#define __ELASTOS_IO_BUFFEREDOUTPUTSTREAM_H__

#include "FilterOutputStream.h"

namespace Elastos {
namespace IO {

class BufferedOutputStream
    : public FilterOutputStream
    , public IBufferedOutputStream
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IOutputStream* outs,
        /* [in] */ Int32 size);

    CARAPI constructor(
        /* [in] */ IOutputStream* outs);

    CARAPI Close();

    /**
     * Flushes this stream to ensure all pending data is written out to the
     * target stream. In addition, the target stream is flushed.
     *
     * @throws IOException
     *             if an error occurs attempting to flush this stream.
     */
    CARAPI Flush();

    /**
     * Writes one byte to this stream. Only the low order byte of the integer
     * {@code oneByte} is written. If there is room in the buffer, the byte is
     * copied into the buffer and the count incremented. Otherwise, the buffer
     * plus {@code oneByte} are written to the target stream, the target is
     * flushed, and the buffer is reset.
     *
     * @param oneByte
     *            the byte to be written.
     * @throws IOException
     *             if an error occurs attempting to write to this stream.
     */
    CARAPI Write(
        /* [in] */ Int32 oneByte);

    /**
     * Writes {@code count} bytes from the byte array {@code buffer} starting at
     * {@code offset} to this stream. If there is room in the buffer to hold the
     * bytes, they are copied in. If not, the buffered bytes plus the bytes in
     * {@code buffer} are written to the target stream, the target is flushed,
     * and the buffer is cleared.
     *
     * @param buffer
     *            the buffer to be written.
     * @param offset
     *            the start position in {@code buffer} from where to get bytes.
     * @param length
     *            the number of bytes from {@code buffer} to write to this
     *            stream.
     * @throws IndexOutOfBoundsException
     *             if {@code offset < 0} or {@code length < 0}, or if
     *             {@code offset + length} is greater than the size of
     *             {@code buffer}.
     * @throws IOException
     *             if an error occurs attempting to write to this stream.
     * @throws NullPointerException
     *             if {@code buffer} is {@code null}.
     * @throws ArrayIndexOutOfBoundsException
     *             If offset or count is outside of bounds.
     */
    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

protected:
    BufferedOutputStream();

    virtual ~BufferedOutputStream();

private:
    CARAPI FlushInternal();

    CARAPI CheckNotClosed();

protected:
    /**
     * The buffer containing the bytes to be written to the target stream.
     */
    AutoPtr<ArrayOf<Byte> > mBuf;

    /**
     * The total number of bytes inside the byte array {@code buf}.
     */
    Int32 mCount;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_BUFFEREDOUTPUTSTREAM_H__
