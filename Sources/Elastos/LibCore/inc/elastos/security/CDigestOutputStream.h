
#ifndef __ELASTOS_SECURITY_CDIGESTOUTPUTSTREAM_H__
#define __ELASTOS_SECURITY_CDIGESTOUTPUTSTREAM_H__

#include "_Elastos_Security_CDigestOutputStream.h"
#include <elastos/io/FilterOutputStream.h>

using Elastos::IO::FilterOutputStream;

namespace Elastos {
namespace Security {

/**
 * {@code DigestOutputStream} is a {@code FilterOutputStream} which maintains an
 * associated message digest.
 */
CarClass(CDigestOutputStream)
    , public FilterOutputStream
    , public IDigestOutputStream
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CDigestOutputStream();

    /**
     * Constructs a new instance of this {@code DigestOutputStream}, using the
     * given {@code stream} and the {@code digest}.
     *
     * @param stream
     *            the output stream.
     * @param digest
     *            the message digest.
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* stream,
        /* [in] */ IMessageDigest* digest);

    /**
     * Returns the message digest for this stream.
     *
     * @return the message digest for this stream.
     */
    CARAPI GetMessageDigest(
        /* [out] */ IMessageDigest** result);

    /**
     * Sets the message digest which this stream will use.
     *
     * @param digest
     *            the message digest which this stream will use.
     */
    CARAPI SetMessageDigest(
        /* [in] */ IMessageDigest* digest);

    /**
     * Writes the specified {@code Int32} to the stream. Updates the digest if
     * this function is {@link #on(Boolean)}.
     *
     * @param b
     *            the byte to be written.
     * @throws IOException
     *             if writing to the stream causes a {@code IOException}
     */
    // @Override
    CARAPI Write(
        /* [in] */ Int32 b) /*throws IOException*/;

    /**
     * Writes {@code len} bytes into the stream, starting from the specified
     * offset. Updates the digest if this function is {@link #on(Boolean)}.
     *
     * @param b
     *            the buffer to write to.
     * @param off
     *            the index of the first byte in {@code b} to write.
     * @param len
     *            the number of bytes in {@code b} to write.
     * @throws IOException
     *             if writing to the stream causes an {@code IOException}.
     */
    // @Override
    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len) /*throws IOException*/;

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
     * {@code DigestOutputStream} including the digest.
     *
     * @return a printable representation for this {@code DigestOutputStream}.
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

protected:
    /**
     * The message digest for this stream.
     */
    AutoPtr<IMessageDigest> mDigest;

private:
    // Indicates whether digest functionality is on or off
    Boolean mIsOn;
};

}
}

#endif // __ELASTOS_SECURITY_CDIGESTOUTPUTSTREAM_H__
