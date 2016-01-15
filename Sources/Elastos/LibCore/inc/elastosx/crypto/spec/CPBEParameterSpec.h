
#ifndef __ELASTOSX_CRYPTO_SPEC_CPBEPARAMETERSPEC_H__
#define __ELASTOSX_CRYPTO_SPEC_CPBEPARAMETERSPEC_H__

#include "_Elastosx_Crypto_Spec_CPBEParameterSpec.h"
#include <elastos/core/Object.h>

using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastos::Security::Spec::EIID_IAlgorithmParameterSpec;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CarClass(CPBEParameterSpec)
    , public Object
    , public IPBEParameterSpec
    , public IAlgorithmParameterSpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CPBEParameterSpec();

    /**
     * Creates a new <code>PBEParameterSpec</code> with the specified salt and
     * iteration count.
     *
     * @param salt
     *            the salt.
     * @param iterationCount
     *            the iteration count.
     * @throws NullPointerException
     *             if salt is null.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * salt,
        /* [in] */ Int32 iterationCount);

    /**
     * Returns a copy to the salt.
     *
     * @return a copy to the salt.
     */
    CARAPI GetSalt(
        /* [out, callee] */ ArrayOf<Byte> ** result);

    /**
     * Returns the iteration count.
     *
     * @return the iteration count.
     */
    CARAPI GetIterationCount(
        /* [out] */ Int32 * result);

private:
    Int32 mIterationCount;
    AutoPtr<ArrayOf<Byte> > mSalt;
};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_CPBEPARAMETERSPEC_H__
