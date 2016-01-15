
#ifndef __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_INFLATERINPUTSTREAM_H__
#define __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_INFLATERINPUTSTREAM_H__

#include "Elastos.CoreLibrary.Utility.Zip.h"
#include "FilterInputStream.h"

using Elastos::IO::IInputStream;
using Elastos::IO::FilterInputStream;

namespace Elastos {
namespace Utility {
namespace Zip {

/**
 * This class provides an implementation of {@code FilterInputStream} that
 * uncompresses data that was compressed using the <i>DEFLATE</i> algorithm
 * (see <a href="http://www.gzip.org/algorithm.txt">specification</a>).
 * Basically it wraps the {@code Inflater} class and takes care of the
 * buffering.
 *
 * @see Inflater
 * @see DeflaterOutputStream
 */
class InflaterInputStream
    : public FilterInputStream
    , public IInflaterInputStream
{
protected:
    InflaterInputStream();

    virtual ~InflaterInputStream();

public:
    CAR_INTERFACE_DECL()

    /**
     * This is the most basic constructor. You only need to pass the {@code
     * InputStream} from which the compressed data is to be read from. Default
     * settings for the {@code Inflater} and internal buffer are be used. In
     * particular the Inflater expects a ZLIB header from the input stream.
     *
     * @param is
     *            the {@code InputStream} to read data from.
     */
    CARAPI constructor(
        /* [in] */ IInputStream* is);

    /**
     * This constructor lets you pass a specifically initialized Inflater,
     * for example one that expects no ZLIB header.
     *
     * @param is
     *            the {@code InputStream} to read data from.
     * @param inflater
     *            the specific {@code Inflater} for uncompressing data.
     */
    CARAPI constructor(
        /* [in] */ IInputStream* is,
        /* [in] */ IInflater* inflater);

    /**
     * This constructor lets you specify both the {@code Inflater} as well as
     * the internal buffer size to be used.
     *
     * @param is
     *            the {@code InputStream} to read data from.
     * @param inflater
     *            the specific {@code Inflater} for uncompressing data.
     * @param bsize
     *            the size to be used for the internal buffer.
     */
    CARAPI constructor(
        /* [in] */ IInputStream* is,
        /* [in] */ IInflater* inflater,
        /* [in] */ Int32 bsize);

    /**
     * Reads a single byte of decompressed data.
     *
     * @return the byte read.
     * @throws IOException
     *             if an error occurs reading the byte.
     */
    //@Override
    CARAPI Read(
        /* [out] */ Int32* value);

    /**
     * Reads up to {@code byteCount} bytes of decompressed data and stores it in
     * {@code buffer} starting at {@code offset}.
     *
     * @return Number of uncompressed bytes read
     */
    //@Override
    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* number);

    /**
     * Skips up to {@code byteCount} bytes of uncompressed data.
     *
     * @param byteCount the number of bytes to skip.
     * @return the number of uncompressed bytes skipped.
     * @throws IllegalArgumentException if {@code byteCount < 0}.
     * @throws IOException if an error occurs skipping.
     */
    //@Override
    CARAPI Skip(
        /* [in] */ Int64 byteCount,
        /* [out] */ Int64* number);

    /**
     * Returns 0 when when this stream has exhausted its input; and 1 otherwise.
     * A result of 1 does not guarantee that further bytes can be returned,
     * with or without blocking.
     *
     * <p>Although consistent with the RI, this behavior is inconsistent with
     * {@link InputStream#available()}, and violates the <a
     * href="http://en.wikipedia.org/wiki/Liskov_substitution_principle">Liskov
     * Substitution Principle</a>. This method should not be used.
     *
     * @return 0 if no further bytes are available. Otherwise returns 1,
     *         which suggests (but does not guarantee) that additional bytes are
     *         available.
     * @throws IOException if this stream is closed or an error occurs
     */
    //@Override
    CARAPI Available(
        /* [out] */ Int32* number);

    /**
     * Closes the input stream.
     *
     * @throws IOException
     *             If an error occurs closing the input stream.
     */
    //@Override
    CARAPI Close();

    /**
     * Marks the current position in the stream. This implementation overrides
     * the super type implementation to do nothing at all.
     *
     * @param readlimit
     *            of no use.
     */
    //@Override
    CARAPI Mark(
        /* [in] */ Int32 readLimit);

    /**
     * Reset the position of the stream to the last marked position. This
     * implementation overrides the supertype implementation and always throws
     * an {@link IOException IOException} when called.
     *
     * @throws IOException
     *             if the method is called
     */
    //@Override
    CARAPI Reset();

    /**
     * Returns whether the receiver implements {@code mark} semantics. This type
     * does not support {@code mark()}, so always responds {@code false}.
     *
     * @return false, always
     */
    //@Override
    CARAPI IsMarkSupported(
        /* [out] */ Boolean* supported);

    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [out] */ Int32* number);

protected:
    /**
     * Fills the input buffer with data to be decompressed.
     *
     * @throws IOException
     *             if an {@code IOException} occurs.
     */
    CARAPI Fill();

private:
    CARAPI CheckClosed();

public:
    static const Int32 BUF_SIZE = 512;

    Boolean mClosed;

    /**
     * True if this stream's last byte has been returned to the user. This
     * could be because the underlying stream has been exhausted, or if errors
     * were encountered while inflating that stream.
     */
    Boolean mEof;

    Int32 mNativeEndBufSize; // android-only

protected:
    /**
     * The inflater used for this stream.
     */
    AutoPtr<IInflater> mInf;

    /**
     * The input buffer used for decompression.
     */
    AutoPtr<ArrayOf<Byte> > mBuf;

    /**
     * The length of the buffer.
     */
    Int32 mLen;

    AutoPtr<IInputStream> mIn;
};

} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_INFLATERINPUTSTREAM_H__
