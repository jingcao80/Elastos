
#ifndef __ELASTOSX_CRYPTO_CCIPHEROUTPUTSTREAM_H__
#define __ELASTOSX_CRYPTO_CCIPHEROUTPUTSTREAM_H__

#include "_Elastosx_Crypto_CCipherOutputStream.h"
#include "FilterOutputStream.h"

using Elastos::IO::IOutputStream;
using Elastos::IO::FilterOutputStream;

namespace Elastosx {
namespace Crypto {

CarClass(CCipherOutputStream)
    , public FilterOutputStream
    , public ICipherOutputStream
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CCipherOutputStream();

    /**
     * Creates a new {@code CipherOutputStream} instance for an {@code
     * OutputStream} and a {@code Cipher}.
     *
     * @param os
     *            the output stream to write data to.
     * @param c
     *            the cipher to process the data with.
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* os,
        /* [in] */ ICipher* c);

    /**
     * Creates a new {@code CipherOutputStream} instance for an {@code
     * OutputStream} without a cipher.
     * <p>
     * A {@code NullCipher} is created to process the data.
     *
     * @param os
     *            the output stream to write the data to.
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* os);

    /**
     * Writes the single byte to this cipher output stream.
     *
     * @param b
     *            the byte to write.
     * @throws IOException
     *             if an error occurs.
     */
    CARAPI Write(
        /* [in] */ Int32 b);

    /**
     * Writes the {@code len} bytes from buffer {@code b} starting at offset
     * {@code off} to this cipher output stream.
     *
     * @param b
     *            the buffer.
     * @param off
     *            the offset to start at.
     * @param len
     *            the number of bytes.
     * @throws IOException
     *             if an error occurs.
     */
    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len);

    /**
     * Flushes this cipher output stream.
     *
     * @throws IOException
     *             if an error occurs
     */
    CARAPI Flush();

    /**
     * Close this cipher output stream.
     * <p>
     * On the underlying cipher {@code doFinal} will be invoked, and any
     * buffered bytes from the cipher are also written out, and the cipher is
     * reset to its initial state. The underlying output stream is also closed.
     *
     * @throws IOException
     *             if an error occurs.
     */
    CARAPI Close();

private:
    AutoPtr<ICipher> mCipher;
};

}
}

#endif // __ELASTOSX_CRYPTO_CCIPHEROUTPUTSTREAM_H__
