
#ifndef __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_FILTEROUTPUTSTREAM_H__
#define __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_FILTEROUTPUTSTREAM_H__

#include "Elastos.CoreLibrary.Utility.Zip.h"
#include "FilterOutputStream.h"

using Elastos::IO::IOutputStream;
using Elastos::IO::FilterOutputStream;

namespace Elastos {
namespace Utility {
namespace Zip {

/**
 * An {@code OutputStream} filter to decompress data. Callers write
 * compressed data in the "deflate" format, and uncompressed data is
 * written to the underlying stream.
 * @since 1.6
 */
class InflaterOutputStream
    : public FilterOutputStream
    , public IInflaterOutputStream
{
protected:
    InflaterOutputStream();

    virtual ~InflaterOutputStream();

public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs an {@code InflaterOutputStream} with a new {@code Inflater} and an
     * implementation-defined default internal buffer size. {@code out} is a destination
     * for uncompressed data, and compressed data will be written to this stream.
     *
     * @param out the destination {@code OutputStream}
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* out);

    /**
     * Constructs an {@code InflaterOutputStream} with the given {@code Inflater} and an
     * implementation-defined default internal buffer size. {@code out} is a destination
     * for uncompressed data, and compressed data will be written to this stream.
     *
     * @param out the destination {@code OutputStream}
     * @param inf the {@code Inflater} to be used for decompression
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* out,
        /* [in] */ IInflater* inf);

    /**
     * Constructs an {@code InflaterOutputStream} with the given {@code Inflater} and
     * given internal buffer size. {@code out} is a destination
     * for uncompressed data, and compressed data will be written to this stream.
     *
     * @param out the destination {@code OutputStream}
     * @param inf the {@code Inflater} to be used for decompression
     * @param bufferSize the length in bytes of the internal buffer
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* out,
        /* [in] */ IInflater* inf,
        /* [in] */ Int32 bufferSize);

    /**
     * Writes remaining data into the output stream and closes the underlying
     * output stream.
     */
    //@Override
    CARAPI Close();

    //@Override
    CARAPI Flush();

    /**
     * Finishes writing current uncompressed data into the InflaterOutputStream
     * without closing it.
     *
     * @throws IOException if an I/O error occurs, or the stream has been closed
     */
    CARAPI Finish();

    /**
     * Writes a byte to the decompressing output stream. {@code b} should be a byte of
     * compressed input. The corresponding uncompressed data will be written to the underlying
     * stream.
     *
     * @param b the byte
     * @throws IOException if an I/O error occurs, or the stream has been closed
     * @throws ZipException if a zip exception occurs.
     */
    //@Override
    CARAPI Write(
        /* [in] */ Int32 b);

    /**
     * Writes bytes to the decompressing output stream. {@code b} should be an array of
     * compressed input. The corresponding uncompressed data will be written to the underlying
     * stream.
     *
     * @throws IOException if an I/O error occurs, or the stream has been closed
     * @throws ZipException if a zip exception occurs.
     * @throws NullPointerException if {@code b == null}.
     * @throws IndexOutOfBoundsException if {@code off < 0 || len < 0 || off + len > b.length}
     */
    //@Override
    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount);

    //inherit frome super class
    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer);

private:
    CARAPI Write();

    CARAPI CheckClosed();

protected:
    AutoPtr<IInflater> mInf;
    AutoPtr<ArrayOf<Byte> > mBuf;

private:
    static const Int32 DEFAULT_BUFFER_SIZE = 1024;

    Boolean mClosed;
};

} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_FILTEROUTPUTSTREAM_H__
