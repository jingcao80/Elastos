
#ifndef __ELASTOS_IO_CHARARRAYREADER_H__
#define __ELASTOS_IO_CHARARRAYREADER_H__

#include "Reader.h"

namespace Elastos {
namespace IO {

class CharArrayReader
    : public Reader
    , public ICharArrayReader
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ ArrayOf<Char32>* buf);

    CARAPI constructor(
        /* [in] */ ArrayOf<Char32>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    /**
     * This method closes this CharArrayReader. Once it is closed, you can no
     * longer read from it. Only the first invocation of this method has any
     * effect.
     */
    CARAPI Close();

    /**
     * Sets a mark position in this reader. The parameter {@code readLimit} is
     * ignored for CharArrayReaders. Calling {@code reset()} will reposition the
     * reader back to the marked position provided the mark has not been
     * invalidated.
     *
     * @param readLimit
     *            ignored for CharArrayReaders.
     * @throws IOException
     *             if this reader is closed.
     */
    CARAPI Mark(
        /* [in] */ Int32 readLimit);

    /**
     * Indicates whether this reader supports the {@code mark()} and
     * {@code reset()} methods.
     *
     * @return {@code true} for CharArrayReader.
     * @see #mark(int)
     * @see #reset()
     */
    CARAPI IsMarkSupported(
        /* [out] */ Boolean* number);

    /**
     * Reads a single character from this reader and returns it as an integer
     * with the two higher-order bytes set to 0. Returns -1 if no more
     * characters are available from this reader.
     *
     * @return the character read as an int or -1 if the end of the reader has
     *         been reached.
     * @throws IOException
     *             if this reader is closed.
     */
    CARAPI Read(
        /* [out] */ Int32* number);

    /**
     * Reads up to {@code count} characters from this CharArrayReader and
     * stores them at {@code offset} in the character array {@code buffer}.
     * Returns the number of characters actually read or -1 if the end of reader
     * was encountered.
     *
     * @throws IndexOutOfBoundsException
     * if {@code offset < 0 || count < 0 || offset + count > buffer.length}.
     * @throws IOException
     *             if this reader is closed.
     */
    CARAPI Read(
        /* [in] */ ArrayOf<Char32> * buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [out] */ Int32* number);

    /**
     * Indicates whether this reader is ready to be read without blocking.
     * Returns {@code true} if the next {@code read} will not block. Returns
     * {@code false} if this reader may or may not block when {@code read} is
     * called. The implementation in CharArrayReader always returns {@code true}
     * even when it has been closed.
     *
     * @return {@code true} if this reader will not block when {@code read} is
     *         called, {@code false} if unknown or blocking will occur.
     * @throws IOException
     *             if this reader is closed.
     */
    CARAPI IsReady(
        /* [out] */ Boolean* number);

    /**
     * Resets this reader's position to the last {@code mark()} location.
     * Invocations of {@code read()} and {@code skip()} will occur from this new
     * location. If this reader has not been marked, it is reset to the
     * beginning of the string.
     *
     * @throws IOException
     *             if this reader is closed.
     */
    CARAPI Reset();

    /**
     * Skips {@code count} number of characters in this reader. Subsequent
     * {@code read()}s will not return these characters unless {@code reset()}
     * is used. This method does nothing and returns 0 if {@code n} is negative.
     *
     * @param n
     *            the number of characters to skip.
     * @return the number of characters actually skipped.
     * @throws IOException
     *             if this reader is closed.
     */
    CARAPI Skip(
        /* [in] */Int64 n,
        /* [out] */ Int64* number);

protected:
    CharArrayReader();

    virtual ~CharArrayReader();

private:
    /**
     * Indicates whether this reader is open.
     *
     * @return {@code true} if the reader is open, {@code false} otherwise.
     */
    CARAPI_(Boolean) IsOpen();

    /**
     * Indicates whether this reader is closed.
     *
     * @return {@code true} if the reader is closed, {@code false} otherwise.
     */
    CARAPI_(Boolean) IsClosed();

    CARAPI CheckNotClosed();

protected:
    /**
     * The buffer for characters. UTF-8
     */
    AutoPtr<ArrayOf<Char32> > mBuf;

    /**
     * The current buffer position.
     */
    // In Bytes
    Int32 mPos;

    /**
     * The current mark position.
     */
    // In Bytes
    Int32 mMarkedPos;

    /**
     * The ending index of the buffer.
     */
    // In Bytes
    Int32 mCount;

};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CHARARRAYREADER_H__
