#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_BITWISEINPUTSTREAM_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_BITWISEINPUTSTREAM_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

class BitwiseInputStream
    : public Object
    , public IBitwiseInputStream
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Create object from byte array.
     *
     * @param buf a byte array containing data
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* buf);

    /**
     * Return the number of bit still available for reading.
     */
    CARAPI Available(
        /* [out] */ Int32* num);

    /**
     * Read some data and increment the current position.
     *
     * The 8-bit limit on access to bitwise streams is intentional to
     * avoid endianness issues.
     *
     * @param bits the amount of data to read (gte 0, lte 8)
     * @return byte of read data (possibly partially filled, from lsb)
     */
    CARAPI Read(
        /* [in] */ Int32 bits,
        /* [out] */ Int32* num);

    /**
     * Read data in bulk into a byte array and increment the current position.
     *
     * @param bits the amount of data to read
     * @return newly allocated byte array of read data
     */
    CARAPI ReadByteArray(
        /* [in] */ Int32 bits,
        /* [out, callee] */ ArrayOf<Byte>** buf);

    /**
     * Increment the current position and ignore contained data.
     *
     * @param bits the amount by which to increment the position
     */
    CARAPI Skip(
        /* [in] */ Int32 bits);

private:
    // The byte array being read from.
    AutoPtr<ArrayOf<Byte> > mBuf;

    // The current position offset, in bits, from the msb in byte 0.
    Int32 mPos;

    // The last valid bit offset.
    Int32 mEnd;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_BITWISEINPUTSTREAM_H__
