
#ifndef __ELASTOS_DROID_UTILITY_BASE64INPUTSTREAM_H__
#define __ELASTOS_DROID_UTILITY_BASE64INPUTSTREAM_H__

#include <elastos/io/FilterInputStream.h>
#include "elastos/droid/utility/CBase64.h"

using Elastos::IO::IInputStream;
using Elastos::IO::FilterInputStream;

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * An OutputStream that does Base64 encoding on the data written to
 * it, writing the resulting data to another OutputStream.
 */
class Base64InputStream
    : public FilterInputStream
    , public IBase64InputStream
{
public:
    CAR_INTERFACE_DECL()

    Base64InputStream();

    /**
     * An InputStream that performs Base64 decoding on the data read
     * from the wrapped stream.
     *
     * @param in the InputStream to read the source data from
     * @param flags bit flags for controlling the decoder; see the
     *        constants in {@link Base64}
     */
    CARAPI constructor(
        /* [in] */ IInputStream* inputStream,
        /* [in] */ Int32 flags);

    /**
     * Performs Base64 encoding or decoding on the data read from the
     * wrapped InputStream.
     *
     * @param in the InputStream to read the source data from
     * @param flags bit flags for controlling the decoder; see the
     *        constants in {@link Base64}
     * @param encode true to encode, false to decode
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IInputStream* inputStream,
        /* [in] */ Int32 flags,
        /* [in] */ Boolean encode);

    CARAPI IsMarkSupported(
        /* [out] */ Boolean* supported);

    CARAPI Mark(
        /* [in] */ Int32 readLimit);

    CARAPI Reset();

    CARAPI Close();

    CARAPI Available(
        /* [out] */ Int32* number);

    CARAPI Skip(
        /* [in] */ Int64 byteCount,
        /* [out] */ Int64* number);

    CARAPI Read(
        /* [out] */ Int32* value);

    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* number);

private:
    /**
     * Read data from the input stream into inputBuffer, then
     * decode/encode it into the empty coder.output, and reset the
     * outputStart and outputEnd pointers.
     */
    CARAPI Refill();

private:
    AutoPtr<CBase64::Coder> mCoder;

    static AutoPtr<ArrayOf<Byte> > EMPTY;// = new byte[0];

    static const Int32 BUFFER_SIZE;// = 2048;

    Boolean mEof;
    AutoPtr<ArrayOf<Byte> > mInputBuffer;
    Int32 mOutputStart;
    Int32 mOutputEnd;
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_BASE64INPUTSTREAM_H__
