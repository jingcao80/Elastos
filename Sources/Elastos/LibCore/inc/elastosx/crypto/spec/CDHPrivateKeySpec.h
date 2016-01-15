
#ifndef __ELASTOSX_CRYPTO_SPEC_CDHPRIVATEKEYSPEC_H__
#define __ELASTOSX_CRYPTO_SPEC_CDHPRIVATEKEYSPEC_H__

#include "_Elastosx_Crypto_Spec_CDHPrivateKeySpec.h"
#include <elastos/core/Object.h>

using Elastos::Security::Spec::IKeySpec;
using Elastos::Security::Spec::EIID_IKeySpec;
using Elastos::Math::IBigInteger;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CarClass(CDHPrivateKeySpec)
    , public Object
    , public IDHPrivateKeySpec
    , public IKeySpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CDHPrivateKeySpec();

    /**
     * Creates a new <code>DHPrivateKeySpec</code> with the specified <i>private
     * value</i> <code>x</code>. <i>prime modulus</i> <code>p</code> and <i>base
     * generator</i> <code>g</code>.
     *
     * @param x
     *            the private value.
     * @param p
     *            the prime modulus.
     * @param g
     *            the base generator.
     */
    CARAPI constructor(
        /* [in] */ IBigInteger * x,
        /* [in] */ IBigInteger * p,
        /* [in] */ IBigInteger * g);

    /**
     * Returns the <i>private value</i> <code>x</code>.
     *
     * @return the private value <code>x</code>.
     */
    CARAPI GetX(
        /* [out] */ IBigInteger ** result);

    /**
     * Returns the <i>prime modulus</i> <code>p</code>.
     *
     * @return the prime modulus <code>p</code>.
     */
    CARAPI GetP(
        /* [out] */ IBigInteger ** result);

    /**
     * Returns the <i>base generator</i> <code>g</code>.
     *
     * @return the base generator <code>g</code>.
     */
    CARAPI GetG(
        /* [out] */ IBigInteger ** result);


private:
    AutoPtr<IBigInteger> mX;
    AutoPtr<IBigInteger> mP;
    AutoPtr<IBigInteger> mG;
};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_CDHPRIVATEKEYSPEC_H__
