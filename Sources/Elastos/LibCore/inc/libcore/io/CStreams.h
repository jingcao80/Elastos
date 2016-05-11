#ifndef __LIBCORE_IO_ELASTOS_IO_CSTREAMS_H__
#define __LIBCORE_IO_ELASTOS_IO_CSTREAMS_H__

#include "_Libcore_IO_CStreams.h"
#include "elastos/core/Singleton.h"

using Elastos::Core::Singleton;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IReader;

namespace Libcore {
namespace IO {

CarClass(CStreams)
    , public Singleton
    , public IStreams
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Implements InputStream.read(int) in terms of InputStream.read(byte[], int, int).
     * InputStream assumes that you implement InputStream.read(int) and provides default
     * implementations of the others, but often the opposite is more efficient.
     */
    CARAPI ReadSingleByte(
        /* [in] */ IInputStream* in,
        /* [out] */ Int32* singleByte);

    /**
     * Implements OutputStream.write(int) in terms of OutputStream.write(byte[], int, int).
     * OutputStream assumes that you implement OutputStream.write(int) and provides default
     * implementations of the others, but often the opposite is more efficient.
     */
    CARAPI WriteSingleByte(
        /* [in] */ IOutputStream* out,
        /* [in] */ Int32 b);

    /**
     * Fills 'dst' with bytes from 'in', throwing EOFException if insufficient bytes are available.
     */
    CARAPI ReadFully(
        /* [in] */ IInputStream* in,
        /* [out, callee] */ ArrayOf<Byte>** dst);

    /**
     * Reads exactly 'byteCount' bytes from 'in' (into 'dst' at offset 'offset'), and throws
     * EOFException if insufficient bytes are available.
     *
     * Used to implement {@link java.io.DataInputStream#readFully(byte[], int, int)}.
     */
    CARAPI ReadFully(
        /* [in] */ IInputStream* in,
        /* [in] */ ArrayOf<Byte>* dst,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount);

    /**
     * Returns a byte[] containing the remainder of 'in', closing it when done.
     */
    CARAPI ReadFullyCloseWhenDone(
        /* [in] */ IInputStream* in,
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    /**
     * Returns a byte[] containing the remainder of 'in'.
     */
    CARAPI ReadFullyNoClose(
        /* [in] */ IInputStream* in,
        /* [out, callee] */ ArrayOf<Byte>** byteArray);

    /**
     * Returns the remainder of 'reader' as a string, closing it when done.
     */
    CARAPI ReadFully(
        /* [in] */ IReader* reader,
        /* [out] */ String* result);

    CARAPI SkipAll(
        /* [in] */ IInputStream* in);

    /**
     * Call {@code in.read()} repeatedly until either the stream is exhausted or
     * {@code byteCount} bytes have been read.
     *
     * <p>This method reuses the skip buffer but is careful to never use it at
     * the same time that another stream is using it. Otherwise streams that use
     * the caller's buffer for consistency checks like CRC could be clobbered by
     * other threads. A thread-local buffer is also insufficient because some
     * streams may call other streams in their skip() method, also clobbering the
     * buffer.
     */
    CARAPI SkipByReading(
        /* [in] */ IInputStream* in,
        /* [in] */ Int64 byteCount,
        /* [out] */ Int64* result);

    /**
     * Copies all of the bytes from {@code in} to {@code out}. Neither stream is closed.
     * Returns the total number of bytes transferred.
     */
    CARAPI Copy(
        /* [in] */ IInputStream* in,
        /* [in] */ IOutputStream* out,
        /* [out] */ Int32* number);

    /**
     * Returns the ASCII characters up to but not including the next "\r\n", or
     * "\n".
     *
     * @throws java.io.EOFException if the stream is exhausted before the next newline
     *     character.
     */
    CARAPI ReadAsciiLine(
        /* [in] */ IInputStream* in,
        /* [out] */ String* characters);

private:
    static AutoPtr<ArrayOf<Byte> > mSkipBuffer;
};

} // namespace IO
} // namespace Libcore

#endif //__LIBCORE_IO_ELASTOS_IO_CSTREAMS_H__
