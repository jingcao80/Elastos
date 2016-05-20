
#ifndef __ELASTOS_SECURITY_CALGORITHMPARAMETERGENERATOR_H__
#define __ELASTOS_SECURITY_CALGORITHMPARAMETERGENERATOR_H__

#include "elastos/core/Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Security {

/**
 * {@code AlgorithmParameterGenerator} is an engine class which is capable of
 * generating parameters for the algorithm it was initialized with.
 */
CarClass(CAlgorithmParameterGenerator)
    , public Object
    , public IAlgorithmParameterGenerator
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Constructs a new instance of {@code AlgorithmParameterGenerator} with the
     * given arguments.
     *
     * @param paramGenSpi
     *            a concrete implementation, this engine instance delegates to.
     * @param provider
     *            the provider.
     * @param algorithm
     *            the name of the algorithm.
     */
    CARAPI constructor(
        /* [in] */ IAlgorithmParameterGeneratorSpi* paramGenSpi,
        /* [in] */ IProvider* provider,
        /* [in] */ const String& algorithm);

    /**
     * Returns the name of the algorithm.
     *
     * @return the name of the algorithm.
     */
    CARAPI GetAlgorithm(
        /* [out] */ String* algorithm);

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

    /**
     * Returns the provider associated with this {@code
     * AlgorithmParameterGenerator}.
     *
     * @return the provider associated with this {@code
     *         AlgorithmParameterGenerator}.
     */
    CARAPI GetProvider(
        /* [out] */ IProvider** provider);

    /**
     * Initializes this {@code AlgorithmParameterGenerator} with the given size.
     * The default parameter set and a default {@code SecureRandom} instance
     * will be used.
     *
     * @param size
     *            the size (in number of bits).
     */
    CARAPI Init(
        /* [in] */ Int32 size);

    /**
     * Initializes this {@code AlgorithmParameterGenerator} with the given size
     * and the given {@code SecureRandom}. The default parameter set will be
     * used.
     *
     * @param size
     *            the size (in number of bits).
     * @param random
     *            the source of randomness.
     */
    CARAPI Init(
        /* [in] */ Int32 size,
        /* [in] */ ISecureRandom* random);

    /**
     * Initializes this {@code AlgorithmParameterGenerator} with the given {@code
     * AlgorithmParameterSpec}. A default {@code SecureRandom} instance will be
     * used.
     *
     * @param genParamSpec
     *            the parameters to use.
     * @throws InvalidAlgorithmParameterException
     *             if the specified parameters are not supported.
     */
    CARAPI Init(
        /* [in] */ IAlgorithmParameterSpec* genParamSpec) /*throws InvalidAlgorithmParameterException*/;

    /**
     * Initializes this {@code AlgorithmParameterGenerator} with the given
     * {@code AlgorithmParameterSpec} and the given {@code SecureRandom}.
     *
     * @param genParamSpec
     *            the parameters to use.
     * @param random
     *            the source of randomness.
     * @throws InvalidAlgorithmParameterException
     *             if the specified parameters are not supported.
     */
    CARAPI Init(
        /* [in] */ IAlgorithmParameterSpec* genParamSpec,
        /* [in] */ ISecureRandom* random) /*throws InvalidAlgorithmParameterException*/;

    /**
     * Computes and returns {@code AlgorithmParameters} for this generator's
     * algorithm.
     *
     * @return {@code AlgorithmParameters} for this generator's algorithm.
     */
    CARAPI GenerateParameters(
        /* [out] */ IAlgorithmParameters** param);

private:
    static CARAPI_(Boolean) sInitStatic();

private:
    // Store spi service name
    static const String SERVICE;

    // Used to access common engine functionality
    static AutoPtr<IEngine> ENGINE = new Engine(SERVICE);

    // Store SecureRandom
    static AutoPtr<ISecureRandom> RANDOM = new SecureRandom();
    static Boolean sInit;

    // Store used provider
    AutoPtr<IProvider> mProvider;

    // Store used AlgorithmParameterGeneratorSpi implementation
    AutoPtr<IAlgorithmParameterGeneratorSpi> mSpiImpl;

    //Store used algorithm
    String mAlgorithm;
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_CALGORITHMPARAMETERGENERATOR_H__
