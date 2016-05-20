
#ifndef __ELASTOS_SECURITY_CALGORITHMPARAMETERGENERATORHELPER_H__
#define __ELASTOS_SECURITY_CALGORITHMPARAMETERGENERATORHELPER_H__

#include "elastos/core/Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Security {

CarClass(CAlgorithmParameterGeneratorHelper)
    , public Singleton
    , public IAlgorithmParameterGeneratorHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    /**
     * Returns a new instance of {@code AlgorithmParameterGenerator} for the
     * specified algorithm.
     *
     * @param algorithm
     *            the name of the algorithm to use.
     * @return a new instance of {@code AlgorithmParameterGenerator} for the
     *         specified algorithm.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not available.
     * @throws NullPointerException
     *             if {@code algorithm} is {@code null}.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ IAlgorithmParameterGenerator** generator) /*throws NoSuchAlgorithmException*/;

    /**
     * Returns a new instance of {@code AlgorithmParameterGenerator} from the
     * specified provider for the specified algorithm.
     *
     * @param algorithm
     *            the name of the algorithm to use.
     * @param provider
     *            name of the provider of the {@code
     *            AlgorithmParameterGenerator}.
     * @return a new instance of {@code AlgorithmParameterGenerator} for the
     *         specified algorithm.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not available.
     * @throws NoSuchProviderException
     *             if the specified provider is not available.
     * @throws IllegalArgumentException if {@code provider == null || provider.isEmpty()}
     * @throws NullPointerException
     *             if {@code algorithm} is {@code null}.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ IAlgorithmParameterGenerator** generator) /*throws NoSuchAlgorithmException, NoSuchProviderException*/;

    /**
     * Returns a new instance of {@code AlgorithmParameterGenerator} from the
     * specified provider for the specified algorithm. The {@code provider}
     * supplied does not have to be registered.
     *
     * @param algorithm
     *            the name of the algorithm to use.
     * @param provider
     *            the provider of the {@code AlgorithmParameterGenerator}.
     * @return a new instance of {@code AlgorithmParameterGenerator} for the
     *         specified algorithm.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not available.
     * @throws NullPointerException
     *             if {@code algorithm} is {@code null}.
     * @throws IllegalArgumentException if {@code provider == null}
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [out] */ IAlgorithmParameterGenerator** generator) /*throws NoSuchAlgorithmException*/;
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_CALGORITHMPARAMETERGENERATORHELPER_H__
