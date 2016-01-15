
#ifndef __ELASTOS_IO_PIPEDINPUTSTREAM_H__
#define __ELASTOS_IO_PIPEDINPUTSTREAM_H__

#include "InputStream.h"

using Elastos::Core::IThread;

namespace Elastos {
namespace IO {

class PipedInputStream
    : public InputStream
    , public IPipedInputStream
{
public:
    CAR_INTERFACE_DECL()

    PipedInputStream(): mIn(-1){}

    /**
     * Constructs a new unconnected {@code PipedInputStream}. The resulting
     * stream must be connected to a {@link PipedOutputStream} before data may
     * be read from it.
     */
    CARAPI constructor(){return NOERROR;}

    /**
     * Constructs a new {@code PipedInputStream} connected to the
     * {@link PipedOutputStream} {@code out}. Any data written to the output
     * stream can be read from the this input stream.
     *
     * @param out
     *            the piped output stream to connect to.
     * @throws IOException
     *             if this stream or {@code out} are already connected.
     */
    CARAPI constructor(
        /* [in] */ IPipedOutputStream* ipos);

    /**
     * Constructs a new unconnected {@code PipedInputStream} with the given
     * buffer size. The resulting stream must be connected to a
     * {@code PipedOutputStream} before data may be read from it.
     *
     * @param pipeSize the size of the buffer in bytes.
     * @throws IllegalArgumentException if pipeSize is less than or equal to zero.
     * @since 1.6
     */
    CARAPI constructor(
        /* [in] */ Int32 pipeSize);

    /**
     * Constructs a new {@code PipedInputStream} connected to the given {@code PipedOutputStream},
     * with the given buffer size. Any data written to the output stream can be read from this
     * input stream.
     *
     * @param out the {@code PipedOutputStream} to connect to.
     * @param pipeSize the size of the buffer in bytes.
     * @throws IOException if an I/O error occurs.
     * @throws IllegalArgumentException if pipeSize is less than or equal to zero.
     * @since 1.6
     */
    CARAPI constructor(
        /* [in] */ IPipedOutputStream* ipos,
        /* [in] */ Int32 pipeSize);

    /**
     * {@inheritDoc}
     *
     * <p>Unlike most streams, {@code PipedInputStream} returns 0 rather than throwing
     * {@code IOException} if the stream has been closed. Unconnected and broken pipes also
     * return 0.
     *
     * @throws IOException if an I/O error occurs
     */
    // @Override
    CARAPI Available(
        /* [out] */ Int32* rev);

    /**
     * Closes this stream. This implementation releases the buffer used for the
     * pipe and notifies all threads waiting to read or write.
     *
     * @throws IOException
     *             if an error occurs while closing this stream.
     */
    // @Override
    CARAPI Close();

    /**
     * Connects this {@code PipedInputStream} to a {@link PipedOutputStream}.
     * Any data written to the output stream becomes readable in this input
     * stream.
     *
     * @param src
     *            the source output stream.
     * @throws IOException
     *             if either stream is already connected.
     */
    CARAPI Connect(
        /* [in] */ IPipedOutputStream* src);

    /**
     * Establishes the connection to the PipedOutputStream.
     *
     * @throws IOException
     *             If this Reader is already connected.
     */
     CARAPI EstablishConnection();

    /**
     * Reads a single byte from this stream and returns it as an integer in the
     * range from 0 to 255. Returns -1 if the end of this stream has been
     * reached. If there is no data in the pipe, this method blocks until data
     * is available, the end of the stream is detected or an exception is
     * thrown.
     * <p>
     * Separate threads should be used to read from a {@code PipedInputStream}
     * and to write to the connected {@link PipedOutputStream}. If the same
     * thread is used, a deadlock may occur.
     *
     * @return the byte read or -1 if the end of the source stream has been
     *         reached.
     * @throws IOException
     *             if this stream is closed or not connected to an output
     *             stream, or if the thread writing to the connected output
     *             stream is no longer alive.
     */
    // @Override
    CARAPI Read(
        /* [out] */ Int32* rev);

    /**
     * Reads up to {@code byteCount} bytes from this stream and stores them in the
     * byte array {@code bytes} starting at {@code byteOffset}. Blocks until at
     * least one byte has been read, the end of the stream is detected or an
     * exception is thrown.
     *
     * <p>Separate threads should be used to read from a {@code PipedInputStream}
     * and to write to the connected {@link PipedOutputStream}. If the same
     * thread is used, a deadlock may occur.
     *
     * <p>Returns the number of bytes actually read or -1 if the end of the stream
     * has been reached.
     *
     * @throws IndexOutOfBoundsException
     *     if {@code byteOffset < 0 || byteCount < 0 || byteOffset + byteCount > bytes.length}.
     * @throws InterruptedIOException
     *             if the thread reading from this stream is interrupted.
     * @throws IOException
     *             if this stream is closed or not connected to an output
     *             stream, or if the thread writing to the connected output
     *             stream is no longer alive.
     * @throws NullPointerException
     *             if {@code bytes} is {@code null}.
     */
    // @Override
    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* rev);

    CARAPI Done();

    CARAPI IsConnected(
        /* [out] */ Boolean* rev);

    /**
     * Receives a byte and stores it in this stream's {@code buffer}. This
     * method is called by {@link PipedOutputStream#write(int)}. The least
     * significant byte of the integer {@code oneByte} is stored at index
     * {@code in} in the {@code buffer}.
     * <p>
     * This method blocks as long as {@code buffer} is full.
     *
     * @param oneByte
     *            the byte to store in this pipe.
     * @throws InterruptedIOException
     *             if the {@code buffer} is full and the thread that has called
     *             this method is interrupted.
     * @throws IOException
     *             if this stream is closed or the thread that has last read
     *             from this stream is no longer alive.
     */
    CARAPI Receive(
        /* [in] */ Int32 oneByte);

protected:
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
    AutoPtr<ArrayOf<Byte> > mBuffer;

    /**
     * The index in {@code buffer} where the next byte will be written.
     */
    Int32 mIn;

    /**
     * The index in {@code buffer} where the next byte will be read.
     */
    Int32 mOut;

    /**
     * The size of the default pipe in bytes.
     */
    static const Int32 PIPE_SIZE;

private:
    AutoPtr<IThread> mLastReader;

    AutoPtr<IThread> mLastWriter;

    Boolean mIsClosed;

    /**
     * Indicates if this pipe is connected.
     */
    Boolean mIsConnected;

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_PIPEDINPUTSTREAM_H__