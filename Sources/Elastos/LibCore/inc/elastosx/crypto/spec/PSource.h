
#ifndef __ELASTOSX_CRYPTO_SPEC_PSOURCE_H__
#define __ELASTOSX_CRYPTO_SPEC_PSOURCE_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include <elastos/core/Object.h>

namespace Elastosx {
namespace Crypto {
namespace Spec {

class PSource
    : public Object
    , public IPSource
{
public:
    CAR_INTERFACE_DECL()

    PSource();

    /**
     * Creates a new <code>PSource</code> instance with the specified source
     * algorithm identifier.
     *
     * @param pSrcName
     *            the source algorithm identifier.
     * @throws NullPointerException
     *             if pSrcName is null.
     */
    CARAPI constructor(
        /* [in] */ const String& pSrcName);

    /**
     * Returns the source algorithm identifier.
     *
     * @return the source algorithm identifier.
     */
    CARAPI GetAlgorithm(
        /* [out] */ String * result);

private:
    String mPSrcName;
};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_PSOURCE_H__
