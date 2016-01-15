
#ifndef __ELASTOSX_CRYPTO_SPEC_CDHPUBLICKEYSPEC_H__
#define __ELASTOSX_CRYPTO_SPEC_CDHPUBLICKEYSPEC_H__

#include "_Elastosx_Crypto_Spec_CDHPublicKeySpec.h"
#include <elastos/core/Object.h>

using Elastos::Security::Spec::IKeySpec;
using Elastos::Security::Spec::EIID_IKeySpec;
using Elastos::Math::IBigInteger;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CarClass(CDHPublicKeySpec)
    , public Object
    , public IDHPublicKeySpec
    , public IKeySpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CDHPublicKeySpec();

    /**
     * Creates a new <code>DHPublicKeySpec</code> instance with the specified
     * <i>public value</i> <code>y</code>, the <i>prime modulus</i>
     * <code>p</code> and the <i>base generator</i> <code>g</code>.
     *
     * @param y
     *            the public value.
     * @param p
     *            the prime modulus.
     * @param g
     *            the base generator.
     */
    CARAPI constructor(
        /* [in] */ IBigInteger * y,
        /* [in] */ IBigInteger * p,
        /* [in] */ IBigInteger * g);

    /**
     * Returns the <i>public value</i> <code>y</code>.
     *
     * @return the public value <code>y</code>.
     */
    CARAPI GetY(
        /* [out] */ IBigInteger ** result);

    /**
     * Returns the <i>prime modulus</i> <code>p</code>.
     *
     * @return the prime modulus <code>p</code>.
     */
    CARAPI GetP(
        /* [out] */ IBigInteger ** result);

    /**
     * Returns the <i>base generator</i> <code>g</code>;
     *
     * @return the base generator <code>g</code>;
     */
    CARAPI GetG(
        /* [out] */ IBigInteger ** result);


private:
    AutoPtr<IBigInteger> mY;
    AutoPtr<IBigInteger> mP;
    AutoPtr<IBigInteger> mG;
};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_CDHPUBLICKEYSPEC_H__
