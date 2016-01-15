
#ifndef __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CGZIPOUTPUTSTREAM_H__
#define __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CGZIPOUTPUTSTREAM_H__

#include "_Elastos_Utility_Zip_CGZIPOutputStream.h"
#include "DeflaterOutputStream.h"
#include "CCRC32.h"

namespace Elastos {
namespace Utility {
namespace Zip {

/**
 * The {@code GZIPOutputStream} class is used to write data to a stream in the
 * GZIP storage format.
 */
CarClass(CGZIPOutputStream)
    , public DeflaterOutputStream
    , public IGZIPOutputStream
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CGZIPOutputStream();

    /**
     * Constructs a new {@code GZIPOutputStream} to write data in GZIP format to
     * the given stream.
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* os);

    /**
     * Constructs a new {@code GZIPOutputStream} to write data in GZIP format to
     * the given stream with the given flushing behavior (see {@link DeflaterOutputStream#flush}).
     * @since 1.7
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* os,
        /* [in] */ Boolean syncFlush);

    /**
     * Constructs a new {@code GZIPOutputStream} to write data in GZIP format to
     * the given stream with the given internal buffer size.
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* os,
        /* [in] */ Int32 size);

    /**
     * Constructs a new {@code GZIPOutputStream} to write data in GZIP format to
     * the given stream with the given internal buffer size and
     * flushing behavior (see {@link DeflaterOutputStream#flush}).
     * @since 1.7
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* os,
        /* [in] */ Int32 size,
        /* [in] */ Boolean syncFlush);

    /**
     * Indicates to the stream that all data has been written out, and any GZIP
     * terminal data can now be written.
     *
     * @throws IOException
     *             if an {@code IOException} occurs.
     */
    //@Override
    CARAPI Finish();

    /**
     * Write up to nbytes of data from the given buffer, starting at offset off,
     * to the underlying stream in GZIP format.
     */
    //@Override
    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 off,
        /* [in] */ Int32 nbytes);

private:
    CARAPI_(Int64) WriteInt64(
        /* [in] */ Int64 i);

    CARAPI_(Int32) WriteInt32(
        /* [in] */ Int32 i);

protected:
    /**
     * The checksum algorithm used when treating uncompressed data.
     */
    AutoPtr<CCRC32> mCrc;
};

} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CGZIPOUTPUTSTREAM_H__
