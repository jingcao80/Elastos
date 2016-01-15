
#ifndef __ELASTOS_SECURITY_SPEC_CECPARAMETERSPEC_H__
#define __ELASTOS_SECURITY_SPEC_CECPARAMETERSPEC_H__

#include "_Elastos_Security_Spec_CECParameterSpec.h"

using Elastos::Math::IBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CECParameterSpec)
    , public Object
    , public IECParameterSpec
    , public IAlgorithmParameterSpec
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI GetCofactor(
        /* [out] */ Int32 *cofactor);

    CARAPI GetCurve(
        /* [out] */ IEllipticCurve **curve);

    CARAPI GetGenerator(
        /* [out] */ IECPoint **generator);

    CARAPI GetOrder(
        /* [out] */ IBigInteger **order);

    CARAPI constructor(
        /* [in] */ IEllipticCurve *curve,
        /* [in] */ IECPoint *generator,
        /* [in] */ IBigInteger *order,
        /* [in] */ Int32 cofactor);

    /**
     * Used to set the curve name if available.
     *
     * @hide
     */
    CARAPI SetCurveName(
        /* [in] */ const String& curveName);

    /**
     * Returns the name of the curve if this is a named curve. Returns
     * {@code null} if this is not known to be a named curve.
     *
     * @hide
     */
    CARAPI GetCurveName(
        /* [out] */ String* name);

private:
    // Elliptic curve for which this is parameter
    AutoPtr<IEllipticCurve> mCurve;
    // Distinguished point on the elliptic curve called generator or base point
    AutoPtr<IECPoint> mGenerator;
    // Order of the generator
    AutoPtr<IBigInteger> mOrder;
    // Cofactor
    Int32 mCofactor;
    // Name of curve if available.
    String mCurveName;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CECPARAMETERSPEC_H__
