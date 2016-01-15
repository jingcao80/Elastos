#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_BITWISEOUTPUTSTREAM_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_BITWISEOUTPUTSTREAM_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

class BitwiseOutputStream
    : public Object
    , public IBitwiseOutputStream
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Create object from hint at desired size.
     *
     * @param startingLength initial internal byte array length in bytes
     */
    CARAPI constructor(
        /* [in] */ Int32 startingLength);

    /**
     * Return byte array containing accumulated data, sized to just fit.
     *
     * @return newly allocated byte array
     */
    CARAPI ToByteArray(
        /* [out, callee] */ ArrayOf<Byte>** buf);

    /**
     * Write some data and increment the current position.
     *
     * The 8-bit limit on access to bitwise streams is intentional to
     * avoid endianness issues.
     *
     * @param bits the amount of data to write (gte 0, lte 8)
     * @param data to write, will be masked to expose only bits param from lsb
     */
    CARAPI Write(
        /* [in] */ Int32 bits,
        /* [in] */ Int32 data);

    /**
     * Write data in bulk from a byte array and increment the current position.
     *
     * @param bits the amount of data to write
     * @param arr the byte array containing data to be written
     */
    CARAPI WriteByteArray(
        /* [in] */ Int32 bits,
        /* [in] */ ArrayOf<Byte>* arr);

    /**
     * Increment the current position, implicitly writing zeros.
     *
     * @param bits the amount by which to increment the position
     */
    CARAPI Skip(
        /* [in] */ Int32 bits);

private:
    /**
     * Allocate a new internal buffer, if needed.
     *
     * @param bits additional bits to be accommodated
     */
    CARAPI_(void) PossExpand(
        /* [in] */ Int32 bits);

private:
    // The byte array being written to, which will be grown as needed.
    AutoPtr<ArrayOf<Byte> > mBuf;

    // The current position offset, in bits, from the msb in byte 0.
    Int32 mPos;

    // The last bit offset, given the current buf length.
    Int32 mEnd;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_BITWISEOUTPUTSTREAM_H__
