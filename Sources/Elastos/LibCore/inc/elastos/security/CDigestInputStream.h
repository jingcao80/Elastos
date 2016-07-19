
#ifndef __ELASTOS_SECURITY_CDIGESTINPUTSTREAM_H__
#define __ELASTOS_SECURITY_CDIGESTINPUTSTREAM_H__

#include "_Elastos_Security_CDigestInputStream.h"
#include <elastos/io/FilterInputStream.h>

using Elastos::IO::FilterInputStream;

namespace Elastos {
namespace Security {

CarClass(CDigestInputStream)
    , public FilterInputStream
    , public IDigestInputStream
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CDigestInputStream();

    /**
     * Constructs a new instance of this {@code DigestInputStream}, using the
     * given {@code stream} and the {@code digest}.
     *
     * <p><strong>Warning:</strong> passing a null source creates an invalid
     * {@code DigestInputStream}. All operations on such a stream will fail.
     *
     * @param stream
     *            the input stream.
     * @param digest
     *            the message digest.
     */
    CARAPI constructor(
        /* [in] */ IInputStream* stream,
        /* [in] */ IMessageDigest* digest);

    /**
     * Returns the message digest for this stream.
     *
     * @return the message digest for this stream.
     */
    CARAPI GetMessageDigest(
        /* [out] */ IMessageDigest** digest);

    /**
     * Sets the message digest which this stream will use.
     *
     * @param digest
     *            the message digest which this stream will use.
     */
    CARAPI SetMessageDigest(
        /* [in] */ IMessageDigest* digest);

    /**
     * Reads the next byte and returns it as an {@code int}. Updates the digest
     * for the byte if this function is {@link #on(boolean)}.
     * <p>
     * This operation is blocking.
     *
     * @return the byte which was read or -1 at end of stream.
     * @throws IOException
     *             if reading the source stream causes an {@code IOException}.
     */
    CARAPI Read(
        /* [out] */ Int32* byteRead);

    /**
     * Reads up to {@code byteCount} bytes into {@code buffer}, starting at
     * {@code byteOffset}. Updates the digest if this function is
     * {@link #on(boolean)}.
     *
     * <p>This operation is blocking.
     *
     * <p>Returns the number of bytes actually read or -1 if the end of the
     * filtered stream has been reached while reading.
     *
     * @throws IOException
     *             if reading the source stream causes an {@code IOException}
     */
    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* bytesRead);

    /**
     * Enables or disables the digest function (default is on).
     *
     * @param on
     *            {@code true} if the digest should be computed, {@code false}
     *            otherwise.
     * @see MessageDigest
     */
    CARAPI On(
        /* [in] */ Boolean on);

    /**
     * Returns a string containing a concise, human-readable description of this
     * {@code DigestInputStream} including the digest.
     *
     * @return a printable representation for this {@code DigestInputStream}.
     */
    CARAPI ToString(
        /* [out] */ String* str);

private:
    /**
     * The message digest for this stream.
     */
    AutoPtr<IMessageDigest> mDigest;

    // Indicates whether digest functionality is on or off
    Boolean mIsOn;
};

}
}

#endif // __ELASTOS_SECURITY_CDIGESTINPUTSTREAM_H__
