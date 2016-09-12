
#ifndef __ORG_CONSCRYPT_BYTEARRAY_H__
#define __ORG_CONSCRYPT_BYTEARRAY_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "_Org.Conscrypt.h"

namespace Org {
namespace Conscrypt {

/**
 * Byte array wrapper for hashtable use. Implements equals() and hashCode().
 */
class ByteArray
    : public IByteArray
    , public Object
{
public:
    CAR_INTERFACE_DECL()

    ByteArray(
        /* [in] */ ArrayOf<Byte>* bytes);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

private:
    AutoPtr<ArrayOf<Byte> > mBytes;
    Int32 mHashCode;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_BYTEARRAY_H__
