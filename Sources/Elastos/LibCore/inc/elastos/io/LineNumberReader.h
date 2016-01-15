
#ifndef __ELASTOS_IO_LINENUMBERREADER_H__
#define __ELASTOS_IO_LINENUMBERREADER_H__

#include "BufferedReader.h"

namespace Elastos {
namespace IO {

class LineNumberReader
    : public BufferedReader
    , public ILineNumberReader
{
public:
    CAR_INTERFACE_DECL()

protected:
    LineNumberReader();

    ~LineNumberReader();

public:
    /**
     * Constructs a new LineNumberReader on the Reader {@code in}. The internal
     * buffer gets the default size (8 KB).
     *
     * @param in
     *            the Reader that is buffered.
     */
    CARAPI constructor(
        /* [in] */ IReader* in);
    /**
     * Constructs a new LineNumberReader on the Reader {@code in}. The size of
     * the internal buffer is specified by the parameter {@code size}.
     *
     * @param in
     *            the Reader that is buffered.
     * @param size
     *            the size of the buffer to allocate.
     * @throws IllegalArgumentException
     *             if {@code size <= 0}.
     */
    CARAPI constructor(
        /* [in] */ IReader* in,
        /* [in] */ Int32 size);

    /**
     * Returns the current line number for this reader. Numbering starts at 0.
     *
     * @return the current line number.
     */
    virtual CARAPI GetLineNumber(
        /* [out] */ Int32* lineNumber);

    /**
     * Sets a mark position in this reader. The parameter {@code readlimit}
     * indicates how many characters can be read before the mark is invalidated.
     * Sending {@code reset()} will reposition this reader back to the marked
     * position, provided that {@code readlimit} has not been surpassed. The
     * line number associated with this marked position is also stored so that
     * it can be restored when {@code reset()} is called.
     *
     * @param readlimit
     *            the number of characters that can be read from this stream
     *            before the mark is invalidated.
     * @throws IOException
     *             if an error occurs while setting the mark in this reader.
     * @see #markSupported()
     * @see #reset()
     */
    CARAPI Mark(
        /* [in] */ Int32 readLimit);

    /**
     * Reads a single character from the source reader and returns it as an
     * integer with the two higher-order bytes set to 0. Returns -1 if the end
     * of the source reader has been reached.
     * <p>
     * The line number count is incremented if a line terminator is encountered.
     * Recognized line terminator sequences are {@code '\r'}, {@code '\n'} and
     * {@code "\r\n"}. Line terminator sequences are always translated into
     * {@code '\n'}.
     *
     * @return the character read or -1 if the end of the source reader has been
     *         reached.
     * @throws IOException
     *             if the reader is closed or another IOException occurs.
     */
    CARAPI Read(
        /* [out] */ Int32* value);

    /**
     * Reads up to {@code count} characters from the source reader and stores
     * them in the character array {@code buffer} starting at {@code offset}.
     * Returns the number of characters actually read or -1 if no characters
     * have been read and the end of this reader has been reached.
     *
     * <p>The line number count is incremented if a line terminator is encountered.
     * Recognized line terminator sequences are {@code '\r'}, {@code '\n'} and
     * {@code "\r\n"}.
     *
     * @throws IOException
     *             if this reader is closed or another IOException occurs.
     */
    // @Override
    CARAPI Read(
        /* [in] */ ArrayOf<Char32>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32* number);

    /**
     * Returns the next line of text available from this reader. A line is
     * represented by 0 or more characters followed by {@code '\r'},
     * {@code '\n'}, {@code "\r\n"} or the end of the stream. The returned
     * string does not include the newline sequence.
     *
     * @return the contents of the line or {@code null} if no characters have
     *         been read before the end of the stream has been reached.
     * @throws IOException
     *             if this reader is closed or another IOException occurs.
     */
    CARAPI ReadLine(
        /* [out] */ String* result);

    /**
     * Resets this reader to the last marked location. It also resets the line
     * count to what is was when this reader was marked. This implementation
     * resets the source reader.
     *
     * @throws IOException
     *             if this reader is already closed, no mark has been set or the
     *             mark is no longer valid because more than {@code readlimit}
     *             bytes have been read since setting the mark.
     * @see #mark(int)
     * @see #markSupported()
     */
    CARAPI Reset();

    /**
     * Sets the line number of this reader to the specified {@code lineNumber}.
     * Note that this may have side effects on the line number associated with
     * the last marked position.
     *
     * @param lineNumber
     *            the new line number value.
     * @see #mark(int)
     * @see #reset()
     */
    virtual CARAPI SetLineNumber(
        /* [in] */ Int32 lineNumber);

    /**
     * Skips {@code count} number of characters in this reader. Subsequent
     * {@code read()}'s will not return these characters unless {@code reset()}
     * is used. This implementation skips {@code count} number of characters in
     * the source reader and increments the line number count whenever line
     * terminator sequences are skipped.
     *
     * @param count
     *            the number of characters to skip.
     * @return the number of characters actually skipped.
     * @throws IllegalArgumentException
     *             if {@code count < 0}.
     * @throws IOException
     *             if this reader is closed or another IOException occurs.
     * @see #mark(int)
     * @see #read()
     * @see #reset()
     */
    CARAPI Skip(
        /* [in] */ Int64 count,
        /* [out] */ Int64* number);

private:
    Int32 mLineNumber;

    Int32 mMarkedLineNumber;

    Boolean mLastWasCR;

    Boolean mMarkedLastWasCR;
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_LINENUMBERREADER_H__
