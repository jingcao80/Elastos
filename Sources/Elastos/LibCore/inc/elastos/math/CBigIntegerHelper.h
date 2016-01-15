#ifndef __ELASTOS_MATH_CBIGINTEGERHELPER_H__
#define __ELASTOS_MATH_CBIGINTEGERHELPER_H__

#include "_Elastos_Math_CBigIntegerHelper.h"
#include "Singleton.h"

using Elastos::Utility::IRandom;

namespace Elastos {
namespace Math {

CarClass(CBigIntegerHelper)
    , public Singleton
    , public IBigIntegerHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /** Returns a {@code BigInteger} whose value is equal to {@code value}. */
    CARAPI ValueOf(
        /* [in] */ Int64 value,
        /* [out] */ IBigInteger** result);

    CARAPI ProbablePrime(
        /* [in] */ Int32 bitLength,
        /* [in] */ IRandom* random,
        /* [out] */ IBigInteger** result);

    CARAPI GetMINUSOne(
        /* [out] */ IBigInteger** result);

    CARAPI GetZERO(
        /* [out] */ IBigInteger** result);

    CARAPI GetONE(
        /* [out] */ IBigInteger** result);

    CARAPI GetTEN(
        /* [out] */ IBigInteger** result);
};

} // namespace Math
} // namespace Elastos

#endif // __ELASTOS_MATH_CBIGINTEGERHELPER_H__
