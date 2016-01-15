
#ifndef __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CCHECKEDINPUTSTREAM_H__
#define __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CCHECKEDINPUTSTREAM_H__

#include "_Elastos_Utility_Zip_CCheckedInputStream.h"
#include "FilterInputStream.h"

using Elastos::IO::FilterInputStream;

namespace Elastos {
namespace Utility {
namespace Zip {

CarClass(CCheckedInputStream)
    , public FilterInputStream
    , public ICheckedInputStream
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Constructs a new {@code CheckedInputStream} on {@code InputStream}
     * {@code is}. The checksum will be calculated using the algorithm
     * implemented by {@code csum}.
     *
     * <p><strong>Warning:</strong> passing a null source creates an invalid
     * {@code CheckedInputStream}. All operations on such a stream will fail.
     *
     * @param is
     *            the input stream to calculate checksum from.
     * @param csum
     *            an entity implementing the checksum algorithm.
     */
    CARAPI constructor(
        /* [in] */ IInputStream* is,
        /* [in] */ IChecksum* csum);

    /**
     * Reads one byte of data from the underlying input stream and updates the
     * checksum with the byte data.
     *
     * @return {@code -1} at the end of the stream, a single byte value
     *         otherwise.
     * @throws IOException
     *             if an {@code IOException} occurs.
     */
    //@Override
    CARAPI Read(
        /* [out] */ Int32* value);

    /**
     * Reads up to n bytes of data from the underlying input stream, storing it
     * into {@code buf}, starting at offset {@code off}. The checksum is
     * updated with the bytes read.
     *
     * @param buf
     *            the byte array in which to store the bytes read.
     * @param off
     *            the initial position in {@code buf} to store the bytes read
     *            from this stream.
     * @param nbytes
     *            the maximum number of bytes to store in {@code buf}.
     * @return the number of bytes actually read or {@code -1} if arrived at the
     *         end of the filtered stream while reading the data.
     * @throws IOException
     *             if this stream is closed or some I/O error occurs.
     */
    //@Override
    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 off,
        /* [in] */ Int32 nbytes,
        /* [out] */ Int32* number);

    /**
     * Returns the checksum calculated on the stream read so far.
     *
     * @return the updated checksum.
     */
    CARAPI GetChecksum(
        /* [out] */ IChecksum** checksum);

    /**
     * Skip up to n bytes of data on the underlying input stream. Any skipped
     * bytes are added to the running checksum value.
     *
     * @param byteCount
     *            the number of bytes to skip.
     * @throws IOException
     *             if this stream is closed or another I/O error occurs.
     * @return the number of bytes skipped.
     */
    //@Override
    CARAPI Skip(
        /* [in] */ Int64 byteCount,
        /* [out] */ Int64* number);

    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [out] */ Int32* number);

private:
    AutoPtr<IChecksum> mCheck;
};

} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CCHECKEDINPUTSTREAM_H__
