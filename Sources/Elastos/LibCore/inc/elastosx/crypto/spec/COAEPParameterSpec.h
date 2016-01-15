
#ifndef __ELASTOSX_CRYPTO_SPEC_COAEPPARAMETERSPEC_H__
#define __ELASTOSX_CRYPTO_SPEC_COAEPPARAMETERSPEC_H__

#include "_Elastosx_Crypto_Spec_COAEPParameterSpec.h"
#include <elastos/core/Object.h>

using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastos::Security::Spec::EIID_IAlgorithmParameterSpec;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CarClass(COAEPParameterSpec)
    , public Object
    , public IOAEPParameterSpec
    , public IAlgorithmParameterSpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    COAEPParameterSpec();

    CARAPI constructor();

    /**
     * Creates a new <code>OAEPParameterSpec</code> instance with the specified
     * <i>message digest</i> algorithm name, <i>mask generation function</i>
     * (<i>mgf</i>) algorithm name, <i>parameters</i> for the <i>mgf</i>
     * algorithm and the <i>source of the label <code>L</code></i>.
     *
     * @param mdName
     *            the message digest algorithm name.
     * @param mgfName
     *            the mask generation function algorithm name.
     * @param mgfSpec
     *            the algorithm parameter specification for the mask generation
     *            function algorithm.
     * @param pSrc
     *            the source of the label <code>L</code>.
     * @throws NullPointerException
     *             if one of <code>mdName</code>, <code>mgfName</code> or
     *             <code>pSrc</code> is null.
     */
    CARAPI constructor(
        /* [in] */ const String& mdName,
        /* [in] */ const String& mgfName,
        /* [in] */ IAlgorithmParameterSpec * mgfSpec,
        /* [in] */ IPSource * pSrc);

    /**
     * Returns the algorithm name of the <i>message digest</i>.
     *
     * @return the algorithm name of the message digest.
     */
    CARAPI GetDigestAlgorithm(
        /* [out] */ String * result);

    /**
     * Returns the algorithm name of the <i>mask generation function</i>.
     *
     * @return the algorithm name of the mask generation function.
     */
    CARAPI GetMGFAlgorithm(
        /* [out] */ String * result);

    /**
     * Returns the algorithm parameter specification for the mask generation
     * function algorithm.
     *
     * @return the algorithm parameter specification for the mask generation
     *         function algorithm.
     */
    CARAPI GetMGFParameters(
        /* [out] */ IAlgorithmParameterSpec ** result);

    /**
     * Returns the source of the label <code>L</code>.
     *
     * @return the source of the label <code>L</code>.
     */
    CARAPI GetPSource(
        /* [out] */ IPSource ** result);

private:
    String mMdName;
    String mMgfName;
    AutoPtr<IAlgorithmParameterSpec> mMgfSpec;
    AutoPtr<IPSource> mPSrc;
};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_COAEPPARAMETERSPEC_H__
