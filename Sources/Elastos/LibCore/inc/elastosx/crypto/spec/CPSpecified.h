
#ifndef __ELASTOSX_CRYPTO_SPEC_CPSPECIFIED_H__
#define __ELASTOSX_CRYPTO_SPEC_CPSPECIFIED_H__

#include "_Elastosx_Crypto_Spec_CPSpecified.h"
#include <elastos/core/Object.h>
#include "PSource.h"

namespace Elastosx {
namespace Crypto {
namespace Spec {

CarClass(CPSpecified)
    , public IPSpecified
    , public PSource
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CPSpecified();

    CARAPI constructor();

    /**
     * Creates a new instance of <code>PSpecified</code> with the specified
     * parameter <code>P</code>.
     *
     * @param p
     *            the parameter <code>P</code>.
     * @throws NullPointerException
     *             if <code>p</code> is null.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * p);

    /**
     * Returns a copy of the value of the parameter <code>P</code>.
     *
     * @return a copy of the value of the parameter <code>P</code>
     */
    CARAPI GetValue(
        /* [out, callee] */ ArrayOf<Byte> ** result);

private:
    AutoPtr<ArrayOf<Byte> > mP;
};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_CPSPECIFIED_H__
