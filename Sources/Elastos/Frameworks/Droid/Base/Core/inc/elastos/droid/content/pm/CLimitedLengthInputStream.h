
#ifndef __ELASTOS_DROID_CONTENT_PM_CLIMITEDLENGTHINPUTSTREAM_H__
#define __ELASTOS_DROID_CONTENT_PM_CLIMITEDLENGTHINPUTSTREAM_H__

#include "_Elastos_Droid_Content_Pm_CLimitedLengthInputStream.h"
#include <elastos/io/FilterInputStream.h>

using Elastos::IO::IInputStream;
using Elastos::IO::FilterInputStream;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * A class that limits the amount of data that is read from an InputStream. When
 * the specified length is reached, the stream returns an EOF even if the
 * underlying stream still has more data.
 *
 * @hide
 */
CarClass(CLimitedLengthInputStream)
    , public FilterInputStream
    , public ILimitedLengthInputStream
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CLimitedLengthInputStream();

    virtual ~CLimitedLengthInputStream();

    /**
     * @param in underlying stream to wrap
     * @param offset offset into stream where data starts
     * @param length length of data at offset
     * @throws IOException if an error occurred with the underlying stream
     */
    CARAPI constructor(
        /* [in] */ IInputStream* inStream,
        /* [in] */ Int64 offset,
        /* [in] */ Int64 length);

    CARAPI Read(
        /* [out] */ Int32* value);

    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [out] */ Int32 * pNumber);

    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* number);

private:
    /**
     * The end of the stream where we don't want to allow more data to be read.
     */
    Int64 mEnd;

    /**
     * Current offset in the stream.
     */
    Int64 mOffset;

    /**
     * Constant for the maximum {@code long} value, 2<sup>63</sup>-1.
     */
    static const Int64 MAX_VALUE = 0x7FFFFFFFFFFFFFFFL;
};

}
}
}
}

#endif // __ELASTOS_DROID_CONTENT_PM_CLIMITEDLENGTHINPUTSTREAM_H__
