
#ifndef __ELASTOS_SECURITY_SPEC_CPSSPARAMETERSPEC_H__
#define __ELASTOS_SECURITY_SPEC_CPSSPARAMETERSPEC_H__

#include "_Elastos_Security_Spec_CPSSParameterSpec.h"
#include "core/Object.h"

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CPSSParameterSpec)
    , public Object
    , public IPSSParameterSpec
    , public IAlgorithmParameterSpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    static CARAPI_(const AutoPtr<IPSSParameterSpec>) InitStatic();

    CARAPI GetSaltLength(
        /* [out] */ Int32 *saltLength);

    CARAPI GetDigestAlgorithm(
        /* [out] */ String *digestAlgo);

    CARAPI GetMGFAlgorithm(
        /* [out] */ String *mgfaAlgo);

    CARAPI GetMGFParameters(
        /* [out] */ IAlgorithmParameterSpec **mgfParams);

    CARAPI GetTrailerField(
        /* [out] */ Int32 *trailerField);

    CARAPI constructor(
        /* [in] */ Int32 saltLen);

    CARAPI constructor(
        /* [in] */ const String& mdName,
        /* [in] */ const String& mgfName,
        /* [in] */ IAlgorithmParameterSpec *mgfSpec,
        /* [in] */ Int32 saltLen,
        /* [in] */ Int32 trailerField);

    static const AutoPtr<IPSSParameterSpec> DEFAULT;

private:
    /**
     * The default parameter specification. It specifies the following parameters:
     * <ul>
     * <li>message digest: {@code "SHA-1"}</li>
     * <li>mask generation function (<i>mgf</i>): {@code "MGF1"}</li>
     * <li>parameters for the <i>mgf</i>: {@link MGF1ParameterSpec#SHA1}</li>
     * <li>salt length: {@code 20}</li>
     * <li>trailer field: {@code -1}</li>
     * </ul>
     */

    // Message digest algorithm name
    String mMdName;
    // Mask generation function algorithm name
    String mMgfName;
    // Mask generation function parameters
    AutoPtr<IAlgorithmParameterSpec> mMgfSpec;
    // Trailer field value
    Int32 mTrailerField;
    // Salt length in bits
    Int32 mSaltLen;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CPSSPARAMETERSPEC_H__
