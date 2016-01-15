
#ifndef __ELASTOS_IO_PUSHBACKREADER_H__
#define __ELASTOS_IO_PUSHBACKREADER_H__

#include "FilterReader.h"

namespace Elastos {
namespace IO {

class PushbackReader
    : public FilterReader
    , public IPushbackReader
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs a new {@code PushbackReader} with the specified reader as
     * source. The size of the pushback buffer is set to the default value of 1
     * character.
     *
     * @param in
     *            the source reader.
     */
    CARAPI constructor(
        /* [in] */ IReader* in);

    /**
     * Constructs a new {@code PushbackReader} with {@code in} as source reader.
     * The size of the pushback buffer is set to {@code size}.
     *
     * @param in
     *            the source reader.
     * @param size
     *            the size of the pushback buffer.
     * @throws IllegalArgumentException
     *             if {@code size} is negative.
     */
    CARAPI constructor(
        /* [in] */ IReader* in,
        /* [in] */ Int32 size);

    /**
     * Closes this reader. This implementation closes the source reader
     * and releases the pushback buffer.
     *
     * @throws IOException
     *             if an error occurs while closing this reader.
     */
    CARAPI Close();

    /**
     * Marks the current position in this stream. Setting a mark is not
     * supported in this class; this implementation always throws an
     * {@code IOException}.
     *
     * @param readAheadLimit
     *            the number of character that can be read from this reader
     *            before the mark is invalidated; this parameter is ignored.
     * @throws IOException
     *             if this method is called.
     */
    CARAPI Mark(
        /* [in] */ Int32 readLimit);

    /**
     * Indicates whether this reader supports the {@code mark(int)} and
     * {@code reset()} methods. {@code PushbackReader} does not support them, so
     * it returns {@code false}.
     *
     * @return always {@code false}.
     * @see #mark(int)
     * @see #reset()
     */
    CARAPI IsMarkSupported(
        /* [out] */ Boolean* supported);

    /**
     * Reads a single character from this reader and returns it as an integer
     * with the two higher-order bytes set to 0. Returns -1 if the end of the
     * reader has been reached. If the pushback buffer does not contain any
     * available characters then a character from the source reader is returned.
     * Blocks until one character has been read, the end of the source reader is
     * detected or an exception is thrown.
     *
     * @return the character read or -1 if the end of the source reader has been
     *         reached.
     * @throws IOException
     *             if this reader is closed or an I/O error occurs while reading
     *             from this reader.
     */
    CARAPI Read(
        /* [out] */ Int32* value);

    /**
     * Reads up to {@code count} characters from this reader and stores them in
     * character array {@code buffer} starting at {@code offset}. Characters are
     * read from the pushback buffer first, then from the source reader if more
     * bytes are required. Blocks until {@code count} characters have been read,
     * the end of the source reader is detected or an exception is thrown.
     * Returns the number of bytes read or -1 if the end of the source reader has been reached.
     *
     * @throws IndexOutOfBoundsException
     *     if {@code offset < 0 || count < 0 || offset + count > buffer.length}.
     * @throws IOException
     *             if this reader is closed or another I/O error occurs while
     *             reading from this reader.
     */
    // @Override
    CARAPI Read(
        /* [in] */ ArrayOf<Char32>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [out] */ Int32* number);

    /**
     * Indicates whether this reader is ready to be read without blocking.
     * Returns {@code true} if this reader will not block when {@code read} is
     * called, {@code false} if unknown or blocking will occur.
     *
     * @return {@code true} if the receiver will not block when
     *         {@code read()} is called, {@code false} if unknown
     *         or blocking will occur.
     * @throws IOException
     *             if this reader is closed or some other I/O error occurs.
     * @see #read()
     * @see #read(char[], int, int)
     */
    CARAPI IsReady(
        /* [out] */ Boolean* ready);

    /**
     * Resets this reader to the last marked position. Resetting the reader is
     * not supported in this class; this implementation always throws an
     * {@code IOException}.
     *
     * @throws IOException
     *             if this method is called.
     */
    CARAPI Reset();

    /**
     * Pushes the specified character {@code oneChar} back to this reader. This
     * is done in such a way that the next character read from this reader is
     * {@code (char) oneChar}.
     * <p>
     * If this reader's internal pushback buffer cannot store the character, an
     * {@code IOException} is thrown.
     *
     * @param oneChar
     *            the character to push back to this stream.
     * @throws IOException
     *             if this reader is closed or the internal pushback buffer is
     *             full.
     */
    CARAPI Unread(
        /* [in] */ Int32 oneChar);

    /**
     * Pushes all the characters in {@code buffer} back to this reader. The
     * characters are pushed back in such a way that the next character read
     * from this reader is buffer[0], then buffer[1] and so on.
     * <p>
     * If this reader's internal pushback buffer cannot store the entire
     * contents of {@code buffer}, an {@code IOException} is thrown. Parts of
     * {@code buffer} may have already been copied to the pushback buffer when
     * the exception is thrown.
     *
     * @param buffer
     *            the buffer containing the characters to push back to this
     *            reader.
     * @throws IOException
     *             if this reader is closed or the free space in the internal
     *             pushback buffer is not sufficient to store the contents of
     *             {@code buffer}.
     */
    CARAPI Unread(
        /* [in] */ ArrayOf<Char32>* buffer);

    /**
     * Pushes a subset of the characters in {@code buffer} back to this reader.
     * The subset is defined by the start position {@code offset} within
     * {@code buffer} and the number of characters specified by {@code length}.
     * The bytes are pushed back in such a way that the next byte read from this
     * stream is {@code buffer[offset]}, then {@code buffer[1]} and so on.
     * <p>
     * If this stream's internal pushback buffer cannot store the selected
     * subset of {@code buffer}, an {@code IOException} is thrown. Parts of
     * {@code buffer} may have already been copied to the pushback buffer when
     * the exception is thrown.
     *
     * @param buffer
     *            the buffer containing the characters to push back to this
     *            reader.
     * @param offset
     *            the index of the first byte in {@code buffer} to push back.
     * @param length
     *            the number of bytes to push back.
     * @throws IndexOutOfBoundsException
     *             if {@code offset < 0} or {@code count < 0}, or if
     *             {@code offset + count} is greater than the length of
     *             {@code buffer}.
     * @throws IOException
     *             if this reader is closed or the free space in the internal
     *             pushback buffer is not sufficient to store the selected
     *             contents of {@code buffer}.
     * @throws NullPointerException
     *             if {@code buffer} is {@code null}.
     */
    CARAPI Unread(
        /* [in] */ ArrayOf<Char32>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    /**
     * Skips {@code count} characters in this reader. This implementation skips
     * characters in the pushback buffer first and then in the source reader if
     * necessary.
     *
     * @param count
     *            the number of characters to skip.
     * @return the number of characters actually skipped.
     * @throws IllegalArgumentException if {@code count < 0}.
     * @throws IOException
     *             if this reader is closed or another I/O error occurs.
     */
    CARAPI Skip(
        /* [in] */ Int64 count,
        /* [out] */ Int64* number);

protected:
    PushbackReader();

    virtual ~PushbackReader();

private:
    CARAPI CheckNotClosed();

protected:
    /**
     * The {@code char} array containing the chars to read.
     */
    AutoPtr<ArrayOf<Char32> > mBuf;

    /**
     * The current position within the char array {@code buf}. A value
     * equal to buf.length indicates no chars available. A value of 0 indicates
     * the buffer is full.
     */
    Int32 mPos;
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_PUSHBACKREADER_H__
