
#ifndef __ELASTOS_IO_PIPEDREADER_H__
#define __ELASTOS_IO_PIPEDREADER_H__

#include "Reader.h"

using Elastos::Core::IThread;

namespace Elastos {
namespace IO {

class PipedReader
    : public Reader
    , public IPipedReader
{
public:
    CAR_INTERFACE_DECL()

    PipedReader() : mIn(-1) {}

    // Add additionally
    CARAPI IsClosed(
        /* [out] */ Boolean* isClosed);

    // Add additionally
    CARAPI IsConnected(
        /* [out] */ Boolean* isConnected);

    /**
     * Constructs a new unconnected {@code PipedReader}. The resulting reader
     * must be connected to a {@code PipedWriter} before data may be read from
     * it.
     */
    CARAPI constructor();

    /**
     * Constructs a new {@code PipedReader} connected to the {@link PipedWriter}
     * {@code out}. Any data written to the writer can be read from the this
     * reader.
     *
     * @param out
     *            the {@code PipedWriter} to connect to.
     * @throws IOException
     *             if {@code out} is already connected.
     */
    CARAPI constructor(
        /* [in] */ IPipedWriter* out);

    /**
     * Constructs a new unconnected {@code PipedReader} with the given buffer size.
     * The resulting reader must be connected to a {@code PipedWriter} before
     * data may be read from it.
     *
     * @param pipeSize the size of the buffer in chars.
     * @throws IllegalArgumentException if pipeSize is less than or equal to zero.
     * @since 1.6
     */
    CARAPI constructor(
        /* [in] */ Int32 pipeSize);

    /**
     * Constructs a new {@code PipedReader} connected to the given {@code PipedWriter},
     * with the given buffer size. Any data written to the writer can be read from
     * this reader.
     *
     * @param out the {@code PipedWriter} to connect to.
     * @param pipeSize the size of the buffer in chars.
     * @throws IOException if an I/O error occurs
     * @throws IllegalArgumentException if pipeSize is less than or equal to zero.
     * @since 1.6
     */
    CARAPI constructor(
        /* [in] */ IPipedWriter* out,
        /* [in] */ Int32 pipeSize);

    /**
     * Closes this reader. This implementation releases the buffer used for
     * the pipe and notifies all threads waiting to read or write.
     *
     * @throws IOException
     *             if an error occurs while closing this reader.
     */
    // @Override
    CARAPI Close();

    CARAPI Connect(
        /* [in] */ IPipedWriter* src);

    CARAPI EstablishConnection();

    /**
     * Reads a single character from this reader and returns it as an integer
     * with the two higher-order bytes set to 0. Returns -1 if the end of the
     * reader has been reached. If there is no data in the pipe, this method
     * blocks until data is available, the end of the reader is detected or an
     * exception is thrown.
     * <p>
     * Separate threads should be used to read from a {@code PipedReader} and to
     * write to the connected {@link PipedWriter}. If the same thread is used,
     * a deadlock may occur.
     *
     * @return the character read or -1 if the end of the reader has been
     *         reached.
     * @throws IOException
     *             if this reader is closed or some other I/O error occurs.
     */
    // @Override
    CARAPI Read(
        /* [out] */ Int32* rev);

    /**
     * Reads up to {@code count} characters from this reader and stores them
     * in the character array {@code buffer} starting at {@code offset}. If
     * there is no data in the pipe, this method blocks until at least one byte
     * has been read, the end of the reader is detected or an exception is
     * thrown.
     *
     * <p>Separate threads should be used to read from a {@code PipedReader} and to
     * write to the connected {@link PipedWriter}. If the same thread is used, a
     * deadlock may occur.
     *
     * <p>Returns the number of characters read or -1 if the end of the reader has
     * been reached.
     *
     * @throws IndexOutOfBoundsException
     *     if {@code offset < 0 || count < 0 || offset + count > buffer.length}.
     * @throws InterruptedIOException
     *             if the thread reading from this reader is interrupted.
     * @throws IOException
     *             if this reader is closed or not connected to a writer, or if
     *             the thread writing to the connected writer is no longer
     *             alive.
     */
    // @Override
     CARAPI Read(
        /* [in] */ ArrayOf<Char32>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [out] */ Int32* rev);

    /**
     * Indicates whether this reader is ready to be read without blocking.
     * Returns {@code true} if this reader will not block when {@code read} is
     * called, {@code false} if unknown or blocking will occur. This
     * implementation returns {@code true} if the internal buffer contains
     * characters that can be read.
     *
     * @return always {@code false}.
     * @throws IOException
     *             if this reader is closed or not connected, or if some other
     *             I/O error occurs.
     * @see #read()
     * @see #read(char[], int, int)
     */
    // @Override
    CARAPI Ready(
        /* [out] */ Boolean* rev);

    /**
     * Receives a char and stores it into the PipedReader. This called by
     * PipedWriter.write() when writes occur.
     * <P>
     * If the buffer is full and the thread sending #receive is interrupted, the
     * InterruptedIOException will be thrown.
     *
     * @param oneChar
     *            the char to store into the pipe.
     *
     * @throws IOException
     *             If the stream is already closed or another IOException
     *             occurs.
     */
     CARAPI Receive(
        /* [in] */ Char32 oneChar);

    /**
     * Receives a char array and stores it into the PipedReader. This called by
     * PipedWriter.write() when writes occur.
     * <P>
     * If the buffer is full and the thread sending #receive is interrupted, the
     * InterruptedIOException will be thrown.
     *
     * @throws IOException
     *             If the stream is already closed or another IOException
     *             occurs.
     */
    CARAPI Receive(
        /* [in] */ ArrayOf<Char32>* chars,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    CARAPI Done();

private:
    Boolean mIsClosed;

    AutoPtr<IThread> mLastReader;

    AutoPtr<IThread> mLastWriter;

    /**
     * The circular buffer through which data is passed. Data is read from the
     * range {@code [out, in)} and written to the range {@code [in, out)}.
     * Data in the buffer is either sequential: <pre>
     *     { - - - X X X X X X X - - - - - }
     *             ^             ^
     *             |             |
     *            out           in</pre>
     * ...or wrapped around the buffer's end: <pre>
     *     { X X X X - - - - - - - - X X X }
     *               ^               ^
     *               |               |
     *              in              out</pre>
     * When the buffer is empty, {@code in == -1}. Reading when the buffer is
     * empty will block until data is available. When the buffer is full,
     * {@code in == out}. Writing when the buffer is full will block until free
     * space is available.
     */
    AutoPtr<ArrayOf<Char32> > mBuffer;

    /**
     * The index in {@code buffer} where the next character will be written.
     */
    Int32 mIn;

    /**
     * The index in {@code buffer} where the next character will be read.
     */
    Int32 mOut;

    /**
     * The size of the default pipe in characters
     */
    static const Int32 PIPE_SIZE;

    /**
     * Indicates if this pipe is connected
     */
    Boolean mIsConnected;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_PIPEDREADER_H__