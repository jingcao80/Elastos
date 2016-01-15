
#ifndef __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CINFLATER_H__
#define __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CINFLATER_H__

#include "_Elastos_Utility_Zip_CInflater.h"
#include "NativeZipStream.h"
#include "Object.h"

using Elastos::Core::ICloseGuard;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Utility {
namespace Zip {

/**
 * This class decompresses data that was compressed using the <i>DEFLATE</i>
 * algorithm (see <a href="http://www.gzip.org/algorithm.txt">specification</a>).
 *
 * <p>It is usually more convenient to use {@link InflaterInputStream}.
 *
 * <p>To decompress an in-memory {@code byte[]} to another in-memory {@code byte[]} manually:
 * <pre>
 *     byte[] compressedBytes = ...
 *     int decompressedByteCount = ... // From your format's metadata.
 *     Inflater inflater = new Inflater();
 *     inflater.setInput(compressedBytes, 0, compressedBytes.length);
 *     byte[] decompressedBytes = new byte[decompressedByteCount];
 *     if (inflater.inflate(decompressedBytes) != decompressedByteCount) {
 *         throw new AssertionError();
 *     }
 *     inflater.end();
 * </pre>
 * <p>In situations where you don't have all the input in one array (or have so much
 * input that you want to feed it to the inflater in chunks), it's possible to call
 * {@link #setInput} repeatedly, but you're much better off using {@link InflaterInputStream}
 * to handle all this for you.
 *
 * <p>If you don't know how big the decompressed data will be, you can call {@link #inflate}
 * repeatedly on a temporary buffer, copying the bytes to a {@link java.io.ByteArrayOutputStream},
 * but this is probably another sign you'd be better off using {@link InflaterInputStream}.
 */
CarClass(CInflater)
    , public Object
    , public IInflater
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CInflater();

    ~CInflater();

    /**
     * This constructor creates an inflater that expects a header from the input
     * stream. Use {@code Inflater(boolean)} if the input comes without a ZLIB
     * header.
     */
    CARAPI constructor();

    /**
     * This constructor allows to create an inflater that expects no header from
     * the input stream.
     *
     * @param noHeader
     *            {@code true} indicates that no ZLIB header comes with the
     *            input.
     */
    CARAPI constructor(
        /* [in] */ Boolean noHeader);

    /**
     * Releases resources associated with this {@code Inflater}. Any unused
     * input or output is discarded. This method should be called explicitly in
     * order to free native resources as soon as possible. After {@code end()} is
     * called, other methods will typically throw {@code IllegalStateException}.
     */
    // synchronized
    CARAPI End();

    /**
     * Indicates if the {@code Inflater} has inflated the entire deflated
     * stream. If deflated bytes remain and {@code needsInput()} returns {@code
     * true} this method will return {@code false}. This method should be
     * called after all deflated input is supplied to the {@code Inflater}.
     *
     * @return {@code true} if all input has been inflated, {@code false}
     *         otherwise.
     */
    // synchronized
    CARAPI Finished(
        /* [out] */ Boolean* finished);

    /**
     * Returns the {@link Adler32} checksum of the bytes inflated so far, or the
     * checksum of the preset dictionary if {@link #needsDictionary} returns true.
     */
    // synchronized
    CARAPI GetAdler(
        /* [out] */ Int32* checksum);

    /**
     * Returns the total number of bytes read by the {@code Inflater}. This
     * method is the same as {@link #getTotalIn} except that it returns a
     * {@code long} value instead of an integer.
     */
    // synchronized
    CARAPI GetBytesRead(
        /* [out] */ Int64* number);

    /**
     * Returns a the total number of bytes written by this {@code Inflater}. This
     * method is the same as {@code getTotalOut} except it returns a
     * {@code long} value instead of an integer.
     */
    // synchronized
    CARAPI GetBytesWritten(
        /* [out] */ Int64* number);

    /**
     * Returns the number of bytes of current input remaining to be read by this
     * inflater.
     */
    // synchronized
    CARAPI GetRemaining(
        /* [out] */ Int32* number);

    /**
     * Returns the offset of the next byte to read in the underlying buffer.
     *
     * For internal use only.
     */
    CARAPI GetCurrentOffset(
        /* [out] */ Int32* offset);

    /**
     * Returns the total number of bytes of input read by this {@code Inflater}. This
     * method is limited to 32 bits; use {@link #getBytesRead} instead.
     */
    // synchronized
    CARAPI GetTotalIn(
        /* [out] */ Int32* number);

    /**
     * Returns the total number of bytes written to the output buffer by this {@code
     * Inflater}. The method is limited to 32 bits; use {@link #getBytesWritten} instead.
     */
    // synchronized
    CARAPI GetTotalOut(
        /* [out] */ Int32* number);

    /**
     * Inflates bytes from the current input and stores them in {@code buf}.
     *
     * @param buf
     *            the buffer where decompressed data bytes are written.
     * @return the number of bytes inflated.
     * @throws DataFormatException
     *             if the underlying stream is corrupted or was not compressed
     *             using a {@code Deflater}.
     */
    CARAPI Inflate(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [out] */ Int32* number);

    /**
     * Inflates up to {@code byteCount} bytes from the current input and stores them in
     * {@code buf} starting at {@code offset}.
     *
     * @throws DataFormatException
     *             if the underlying stream is corrupted or was not compressed
     *             using a {@code Deflater}.
     * @return the number of bytes inflated.
     */
    // synchronized
    CARAPI Inflate(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* number);

    /**
     * Returns true if the input bytes were compressed with a preset
     * dictionary. This method should be called if the first call to {@link #inflate} returns 0,
     * to determine whether a dictionary is required. If so, {@link #setDictionary}
     * should be called with the appropriate dictionary before calling {@code
     * inflate} again. Use {@link #getAdler} to determine which dictionary is required.
     */
    // synchronized
    CARAPI NeedsDictionary(
        /* [out] */ Boolean* result);

    /**
     * Returns true if {@link #setInput} must be called before inflation can continue.
     */
    // synchronized
    CARAPI NeedsInput(
        /* [out] */ Boolean* result);

    /**
     * Resets this {@code Inflater}. Should be called prior to inflating a new
     * set of data.
     */
    // synchronized
    CARAPI Reset();

    /**
     * Sets the preset dictionary to be used for inflation to {@code dictionary}.
     * See {@link #needsDictionary} for details.
     */
    CARAPI SetDictionary(
        /* [in] */ ArrayOf<Byte>* buf);

    /**
     * Sets the preset dictionary to be used for inflation to a subsequence of {@code dictionary}
     * starting at {@code offset} and continuing for {@code byteCount} bytes. See {@link
     * #needsDictionary} for details.
     */
    // synchronized
    CARAPI SetDictionary(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount);

    /**
     * Sets the current input to to be decompressed. This method should only be
     * called if {@link #needsInput} returns {@code true}.
     */
    CARAPI SetInput(
        /* [in] */ ArrayOf<Byte>* buf);

    /**
     * Sets the current input to to be decompressed. This method should only be
     * called if {@link #needsInput} returns {@code true}.
     */
    // synchronized
    CARAPI SetInput(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount);

    // synchronized
    /* package */ CARAPI_(Int32) SetFileInput(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int64 offset,
        /* [in] */ Int32 byteCount);

private:
    CARAPI CreateStream(
        /* [in] */ Boolean noHeader);

    CARAPI_(void) EndImpl(
        /* [in] */ NativeZipStream* stream);

    CARAPI_(Int32) GetAdlerImpl(
        /* [in] */ NativeZipStream* stream);

    CARAPI_(Int64) GetTotalInImpl(
        /* [in] */ NativeZipStream* stream);

    CARAPI_(Int64) GetTotalOutImpl(
        /* [in] */ NativeZipStream* stream);

    CARAPI InflateImpl(
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount,
        /* [out] */ ArrayOf<Byte>* buf,
        /* [in] */ NativeZipStream* stream,
        /* [out] */ Int32* result);

    CARAPI ResetImpl(
        /* [in] */ NativeZipStream* stream);

    CARAPI_(void) SetDictionaryImpl(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount,
        /* [in] */ NativeZipStream* stream);

    CARAPI_(void) SetInputImpl(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount,
        /* [in] */ NativeZipStream* stream);

    // BEGIN android-only
    CARAPI_(Int32) SetFileInputImpl(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int64 offset,
        /* [in] */ Int32 byteCount,
        /* [in] */ NativeZipStream* stream);

    CARAPI CheckOpen();

private:
    /* package */ Int32 mInLength;

    /* package */ Int32 mInRead; // Set by inflateImpl.
    Boolean mFinished; // Set by the inflateImpl native
    Boolean mNeedsDictionary; // Set by the inflateImpl native

    AutoPtr<NativeZipStream> mStreamHandle;

    AutoPtr<ICloseGuard> mGuard;
};

} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CINFLATER_H__
