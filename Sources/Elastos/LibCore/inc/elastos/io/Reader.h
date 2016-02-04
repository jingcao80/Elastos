
#ifndef __ELASTOS_IO_READER_H__
#define __ELASTOS_IO_READER_H__

#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace IO {

class Reader
    : public Object
    , public IReadable
    , public IReader
    , public ICloseable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ISynchronize* lock);

    /**
     * Sets a mark position in this reader. The parameter {@code readLimit}
     * indicates how many characters can be read before the mark is invalidated.
     * Calling {@code reset()} will reposition the reader back to the marked
     * position if {@code readLimit} has not been surpassed.
     * <p>
     * This default implementation simply throws an {@code IOException};
     * subclasses must provide their own implementation.
     *
     * @param readLimit
     *            the number of characters that can be read before the mark is
     *            invalidated.
     * @throws IllegalArgumentException
     *             if {@code readLimit < 0}.
     * @throws IOException
     *             if an error occurs while setting a mark in this reader.
     * @see #markSupported()
     * @see #reset()
     */
    virtual CARAPI Mark(
        /* [in] */ Int32 readLimit);

    /**
     * Indicates whether this reader supports the {@code mark()} and
     * {@code reset()} methods. This default implementation returns
     * {@code false}.
     *
     * @return always {@code false}.
     */
    virtual CARAPI IsMarkSupported(
        /* [out] */ Boolean* supported);

    /**
     * Reads a single character from this reader and returns it as an integer
     * with the two higher-order bytes set to 0. Returns -1 if the end of the
     * reader has been reached.
     *
     * @return the character read or -1 if the end of the reader has been
     *         reached.
     * @throws IOException
     *             if this reader is closed or some other I/O error occurs.
     */
    virtual CARAPI Read(
        /* [out] */ Int32* value);

    /**
     * Reads characters from this reader and stores them in the character array
     * {@code buffer} starting at offset 0. Returns the number of characters
     * actually read or -1 if the end of the reader has been reached.
     *
     * @throws IOException
     *             if this reader is closed or some other I/O error occurs.
     */
    virtual CARAPI Read(
        /* [out] */ ArrayOf<Char32>* buffer,
        /* [out] */ Int32* number);

    /**
     * Reads up to {@code count} characters from this reader and stores them
     * at {@code offset} in the character array {@code buffer}. Returns the number
     * of characters actually read or -1 if the end of the reader has been
     * reached.
     *
     * @throws IOException
     *             if this reader is closed or some other I/O error occurs.
     */
    virtual CARAPI Read(
        /* [out] */ ArrayOf<Char32>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32* number) = 0;

    /**
     * Indicates whether this reader is ready to be read without blocking.
     * Returns {@code true} if this reader will not block when {@code read} is
     * called, {@code false} if unknown or blocking will occur. This default
     * implementation always returns {@code false}.
     *
     * @return always {@code false}.
     * @throws IOException
     *             if this reader is closed or some other I/O error occurs.
     * @see #read()
     * @see #read(char[])
     * @see #read(char[], int, int)
     */
    virtual CARAPI IsReady(
        /* [out] */ Boolean* ready);

    /**
     * Resets this reader's position to the last {@code mark()} location.
     * Invocations of {@code read()} and {@code skip()} will occur from this new
     * location. If this reader has not been marked, the behavior of
     * {@code reset()} is implementation specific. This default
     * implementation throws an {@code IOException}.
     *
     * @throws IOException
     *             always thrown in this default implementation.
     * @see #mark(int)
     * @see #markSupported()
     */
    virtual CARAPI Reset();

    /**
     * Skips {@code amount} characters in this reader. Subsequent calls of
     * {@code read} methods will not return these characters unless {@code
     * reset()} is used. This method may perform multiple reads to read {@code
     * count} characters.
     *
     * @param count
     *            the maximum number of characters to skip.
     * @return the number of characters actually skipped.
     * @throws IllegalArgumentException
     *             if {@code amount < 0}.
     * @throws IOException
     *             if this reader is closed or some other I/O error occurs.
     * @see #mark(int)
     * @see #markSupported()
     * @see #reset()
     */
    virtual CARAPI Skip(
        /* [in] */ Int64 count,
        /* [out] */ Int64* number);

    /**
     * Reads characters and puts them into the {@code target} character buffer.
     *
     * @param target
     *            the destination character buffer.
     * @return the number of characters put into {@code target} or -1 if the end
     *         of this reader has been reached before a character has been read.
     * @throws IOException
     *             if any I/O error occurs while reading from this reader.
     * @throws NullPointerException
     *             if {@code target} is {@code null}.
     * @throws ReadOnlyBufferException
     *             if {@code target} is read-only.
     */
    virtual CARAPI Read(
        /* [in] */ ICharBuffer* target,
        /* [out] */ Int32* number);

protected:
    /**
     * Constructs a new {@code Reader} with {@code this} as the object used to
     * synchronize critical sections.
     */
    Reader();

    virtual ~Reader();


protected:
    ISynchronize* mLock;
    Boolean mIsStrongLock;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_READER_H__
