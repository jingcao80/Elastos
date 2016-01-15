
#ifndef __ELASTOS_IO_PUSHBACKINPUTSTREAM_H__
#define __ELASTOS_IO_PUSHBACKINPUTSTREAM_H__

#include "FilterInputStream.h"

namespace Elastos {
namespace IO {

class PushbackInputStream
    : public FilterInputStream
    , public IPushbackInputStream
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs a new {@code PushbackInputStream} with the specified input
     * stream as source. The size of the pushback buffer is set to the default
     * value of 1 byte.
     *
     * <p><strong>Warning:</strong> passing a null source creates an invalid
     * {@code PushbackInputStream}. All read operations on such a stream will
     * fail.
     *
     * @param in
     *            the source input stream.
     */
    CARAPI constructor(
        /* [in] */ IInputStream* is);

    /**
     * Constructs a new {@code PushbackInputStream} with {@code in} as source
     * input stream. The size of the pushback buffer is set to {@code size}.
     *
     * <p><strong>Warning:</strong> passing a null source creates an invalid
     * {@code PushbackInputStream}. All read operations on such a stream will
     * fail.
     *
     * @param in
     *            the source input stream.
     * @param size
     *            the size of the pushback buffer.
     * @throws IllegalArgumentException
     *             if {@code size} is negative.
     */
    CARAPI constructor(
        /* [in] */ IInputStream* is,
        /* [in] */ Int32 size);

    CARAPI Available(
        /* [out] */ Int32* number);

    /**
     * Closes this stream. This implementation closes the source stream
     * and releases the pushback buffer.
     *
     * @throws IOException
     *             if an error occurs while closing this stream.
     */
    CARAPI Close();

    /**
     * Indicates whether this stream supports the {@code mark(int)} and
     * {@code reset()} methods. {@code PushbackInputStream} does not support
     * them, so it returns {@code false}.
     *
     * @return always {@code false}.
     * @see #mark(int)
     * @see #reset()
     */
    CARAPI IsMarkSupported(
        /* [out] */ Boolean* supported);

    /**
     * Reads a single byte from this stream and returns it as an integer in the
     * range from 0 to 255. If the pushback buffer does not contain any
     * available bytes then a byte from the source input stream is returned.
     * Blocks until one byte has been read, the end of the source stream is
     * detected or an exception is thrown.
     *
     * @return the byte read or -1 if the end of the source stream has been
     *         reached.
     * @throws IOException
     *             if this stream is closed or an I/O error occurs while reading
     *             from this stream.
     */
    CARAPI Read(
        /* [out] */ Int32* value);

    /**
     * Reads up to {@code byteCount} bytes from this stream and stores them in
     * the byte array {@code buffer} starting at {@code byteOffset}. Bytes are read
     * from the pushback buffer first, then from the source stream if more bytes
     * are required. Blocks until {@code byteCount} bytes have been read, the end of
     * the source stream is detected or an exception is thrown. Returns the number of bytes read,
     * or -1 if the end of the source stream has been reached.
     *
     * @throws IndexOutOfBoundsException
     *     if {@code byteOffset < 0 || byteCount < 0 || byteOffset + byteCount > buffer.length}.
     * @throws IOException
     *             if this stream is closed or another I/O error occurs while
     *             reading from this stream.
     * @throws NullPointerException
     *             if {@code buffer == null}.
     */
    // @Override
    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* number);

    /**
     * Skips {@code count} bytes in this stream. This implementation skips bytes
     * in the pushback buffer first and then in the source stream if necessary.
     *
     * @param count
     *            the number of bytes to skip.
     * @return the number of bytes actually skipped.
     * @throws IOException
     *             if this stream is closed or another I/O error occurs.
     */
    CARAPI Skip(
        /* [in] */ Int64 count,
        /* [out] */ Int64* number);

    /**
     * Pushes the specified byte {@code oneByte} back to this stream. Only the
     * least significant byte of the integer {@code oneByte} is pushed back.
     * This is done in such a way that the next byte read from this stream is
     * {@code (byte) oneByte}.
     * <p>
     * If this stream's internal pushback buffer cannot store the byte, an
     * {@code IOException} is thrown.
     *
     * @param oneByte
     *            the byte to push back to this stream.
     * @throws IOException
     *             if this stream is closed or the internal pushback buffer is
     *             full.
     */
    CARAPI Unread(
        /* [in] */ Int32 oneByte);

    /**
     * Pushes all the bytes in {@code buffer} back to this stream. The bytes are
     * pushed back in such a way that the next byte read from this stream is
     * buffer[0], then buffer[1] and so on.
     * <p>
     * If this stream's internal pushback buffer cannot store the entire
     * contents of {@code buffer}, an {@code IOException} is thrown. Parts of
     * {@code buffer} may have already been copied to the pushback buffer when
     * the exception is thrown.
     *
     * @param buffer
     *            the buffer containing the bytes to push back to this stream.
     * @throws IOException
     *             if the free space in the internal pushback buffer is not
     *             sufficient to store the contents of {@code buffer}.
     */
    CARAPI Unread(
        /* [in] */ ArrayOf<Byte>* buffer);

    /**
     * Pushes a subset of the bytes in {@code buffer} back to this stream. The
     * subset is defined by the start position {@code offset} within
     * {@code buffer} and the number of bytes specified by {@code length}. The
     * bytes are pushed back in such a way that the next byte read from this
     * stream is {@code buffer[offset]}, then {@code buffer[1]} and so on.
     * <p>
     * If this stream's internal pushback buffer cannot store the selected
     * subset of {@code buffer}, an {@code IOException} is thrown. Parts of
     * {@code buffer} may have already been copied to the pushback buffer when
     * the exception is thrown.
     *
     * @param buffer
     *            the buffer containing the bytes to push back to this stream.
     * @param offset
     *            the index of the first byte in {@code buffer} to push back.
     * @param length
     *            the number of bytes to push back.
     * @throws IndexOutOfBoundsException
     *             if {@code offset < 0} or {@code length < 0}, or if
     *             {@code offset + length} is greater than the length of
     *             {@code buffer}.
     * @throws IOException
     *             if the free space in the internal pushback buffer is not
     *             sufficient to store the selected contents of {@code buffer}.
     */
    CARAPI Unread(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    /**
     * Marks the current position in this stream. Setting a mark is not
     * supported in this class; this implementation does nothing.
     *
     * @param readlimit
     *            the number of bytes that can be read from this stream before
     *            the mark is invalidated; this parameter is ignored.
     */
    CARAPI Mark(
        /* [in] */ Int32 readLimit);

    /**
     * Resets this stream to the last marked position. Resetting the stream is
     * not supported in this class; this implementation always throws an
     * {@code IOException}.
     *
     * @throws IOException
     *             if this method is called.
     */
    CARAPI Reset();

protected:
    PushbackInputStream();

    virtual ~PushbackInputStream();

protected:
    /**
     * The buffer that contains pushed-back bytes.
     */
    AutoPtr<ArrayOf<Byte> > mBuf;

    /**
     * The current position within {@code buf}. A value equal to
     * {@code buf.length} indicates that no bytes are available. A value of 0
     * indicates that the buffer is full.
     */
    Int32 mPos;
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_PUSHBACKINPUTSTREAM_H__
