
#ifndef __ELASTOS_IO_PIPEDOUTPUTSTREAM_H__
#define __ELASTOS_IO_PIPEDOUTPUTSTREAM_H__

#include "OutputStream.h"

namespace Elastos {
namespace IO {

class PipedOutputStream
    : public OutputStream
    , public IPipedOutputStream
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs a new unconnected {@code PipedOutputStream}. The resulting
     * stream must be connected to a {@link PipedInputStream} before data can be
     * written to it.
     */
    CARAPI constructor();

    /**
     * Constructs a new {@code PipedOutputStream} connected to the
     * {@link PipedInputStream} {@code target}. Any data written to this stream
     * can be read from the target stream.
     *
     * @param target
     *            the piped input stream to connect to.
     * @throws IOException
     *             if this stream or {@code target} are already connected.
     */
    CARAPI constructor(
        /* [in] */ IPipedInputStream* target);

    CARAPI Close();

    CARAPI Connect(
        /* [in] */ IPipedInputStream* stream);

    CARAPI Flush();

    /**
     * Writes a single byte to this stream. Only the least significant byte of
     * the integer {@code oneByte} is written. The written byte can then be read
     * from the connected input stream.
     * <p>
     * Separate threads should be used to write to a {@code PipedOutputStream}
     * and to read from the connected {@link PipedInputStream}. If the same
     * thread is used, a deadlock may occur.
     *
     * @param oneByte
     *            the byte to write.
     * @throws InterruptedIOException
     *             if the pipe is full and the current thread is interrupted
     *             waiting for space to write data. This case is not currently
     *             handled correctly.
     * @throws IOException
     *             if this stream is not connected, if the target stream is
     *             closed or if the thread reading from the target stream is no
     *             longer alive. This case is currently not handled correctly.
     */
    // @Override
    CARAPI Write(
        /* [in] */ Int32 oneByte);

private:
    /**
     * The destination PipedInputStream
     */
    AutoPtr<IPipedInputStream> mTarget;

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_PIPEDOUTPUTSTREAM_H__