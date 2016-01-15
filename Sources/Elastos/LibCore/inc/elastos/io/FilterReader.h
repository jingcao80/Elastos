
#ifndef __ELASTOS_IO_FILTERREADER_H__
#define __ELASTOS_IO_FILTERREADER_H__

#include "Reader.h"

namespace Elastos {
namespace IO {

class FilterReader
    : public Reader
    , public IFilterReader
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs a new FilterReader on the Reader {@code in}.
     *
     * @param in
     *            The non-null Reader to filter reads on.
     */
    CARAPI constructor(
        /* [in] */ IReader* in);

    /**
     * Closes this reader. This implementation closes the filtered reader.
     *
     * @throws IOException
     *             if an error occurs while closing this reader.
     */
    CARAPI Close();

    /**
     * Sets a mark position in this reader. The parameter {@code readlimit}
     * indicates how many bytes can be read before the mark is invalidated.
     * Sending {@code reset()} will reposition this reader back to the marked
     * position, provided that {@code readlimit} has not been surpassed.
     * <p>
     * This implementation sets a mark in the filtered reader.
     *
     * @param readlimit
     *            the number of bytes that can be read from this reader before
     *            the mark is invalidated.
     * @throws IOException
     *             if an error occurs while marking this reader.
     * @see #markSupported()
     * @see #reset()
     */
    CARAPI Mark(
        /* [in] */ Int32 readLimit);

    /**
     * Indicates whether this reader supports {@code mark()} and {@code reset()}.
     * This implementation returns whether the filtered reader supports marking.
     *
     * @return {@code true} if {@code mark()} and {@code reset()} are supported
     *         by the filtered reader, {@code false} otherwise.
     * @see #mark(int)
     * @see #reset()
     * @see #skip(long)
     */
    CARAPI IsMarkSupported(
        /* [out] */ Boolean* supported);

    /**
     * Reads a single character from the filtered reader and returns it as an
     * integer with the two higher-order bytes set to 0. Returns -1 if the end
     * of the filtered reader has been reached.
     *
     * @return The character read or -1 if the end of the filtered reader has
     *         been reached.
     * @throws IOException
     *             if an error occurs while reading from this reader.
     */
    CARAPI Read(
        /* [out] */ Int32* value);

    /**
     * Reads up to {@code count} characters from the filtered reader and stores them
     * in the byte array {@code buffer} starting at {@code offset}. Returns the
     * number of characters actually read or -1 if no characters were read and
     * the end of the filtered reader was encountered.
     *
     * @throws IOException
     *             if an error occurs while reading from this reader.
     */
    // @Override
    CARAPI Read(
        /* [in] */ ArrayOf<Char32>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [out] */ Int32* number);

    /**
     * Indicates whether this reader is ready to be read without blocking. If
     * the result is {@code true}, the next {@code read()} will not block. If
     * the result is {@code false}, this reader may or may not block when
     * {@code read()} is sent.
     *
     * @return {@code true} if this reader will not block when {@code read()}
     *         is called, {@code false} if unknown or blocking will occur.
     * @throws IOException
     *             if the reader is closed or some other I/O error occurs.
     */
    CARAPI IsReady(
        /* [out] */ Boolean* ready);

    /**
     * Resets this reader's position to the last marked location. Invocations of
     * {@code read()} and {@code skip()} will occur from this new location. If
     * this reader was not marked, the behavior depends on the implementation of
     * {@code reset()} in the Reader subclass that is filtered by this reader.
     * The default behavior for Reader is to throw an {@code IOException}.
     *
     * @throws IOException
     *             if a problem occurred or the filtered reader does not support
     *             {@code mark()} and {@code reset()}.
     * @see #mark(int)
     * @see #markSupported()
     */
    CARAPI Reset();

    /**
     * Skips {@code count} characters in this reader. Subsequent {@code read()}'s
     * will not return these characters unless {@code reset()} is used. The
     * default implementation is to skip characters in the filtered reader.
     *
     * @param count
     *            the maximum number of characters to skip.
     * @return the number of characters actually skipped.
     * @throws IOException
     *             if the filtered reader is closed or some other I/O error
     *             occurs.
     * @see #mark(int)
     * @see #markSupported()
     * @see #reset()
     */
    CARAPI Skip(
        /* [in] */ Int64 count,
        /* [out] */ Int64* number);

protected:
    FilterReader();

    virtual ~FilterReader();

protected:
    /**
     * The target Reader which is being filtered.
     */
    AutoPtr<IReader> mIn;
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_FILTERREADER_H__
