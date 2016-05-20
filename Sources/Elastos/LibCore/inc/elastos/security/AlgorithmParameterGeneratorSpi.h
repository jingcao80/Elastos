
#ifndef __ELASTOS_SECURITY_ALGORITHMPARAMETERGENERATORSPI_H__
#define __ELASTOS_SECURITY_ALGORITHMPARAMETERGENERATORSPI_H__

#include "elastos/core/Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Security {

class CAlgorithmParameterGenerator;

/**
 * {@code AlgorithmParameterGeneratorSpi} is the Service Provider Interface
 * (SPI) definition for {@code AlgorithmParameterGenerator}.
 *
 * @see AlgorithmParameterGenerator
 */
class AlgorithmParameterGeneratorSpi
    : public Object
    , public IAlgorithmParameterGeneratorSpi
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs a new instance of {@code AlgorithmParameterGeneratorSpi} .
     */
    AlgorithmParameterGeneratorSpi();

    virtual ~AlgorithmParameterGeneratorSpi();

protected:
    /**
     * Initializes this {@code AlgorithmParameterGeneratorSpi} with the given
     * size and the given {@code SecureRandom}. The default parameter set
     * will be used.
     *
     * @param size
     *            the size (in number of bits).
     * @param random
     *            the source of randomness.
     */
    virtual CARAPI EngineInit(
        /* [in] */ Int32 size,
        /* [in] */ ISecureRandom* random) = 0;

    /**
     * Initializes this {@code AlgorithmParameterGeneratorSpi} with the given
     * {@code AlgorithmParameterSpec} and the given {@code SecureRandom}.
     *
     * @param genParamSpec
     *            the parameters to use.
     * @param random
     *            the source of randomness.
     * @throws InvalidAlgorithmParameterException
     *             if the specified parameters are not supported.
     */
    virtual CARAPI EngineInit(
        /* [in] */ IAlgorithmParameterSpec* genParamSpec,
        /* [in] */ ISecureRandom* random) /*throws InvalidAlgorithmParameterException*/ = 0;

    /**
     * Computes and returns {@code AlgorithmParameters} for this generator's
     * algorithm.
     *
     * @return {@code AlgorithmParameters} for this generator's algorithm.
     */
    virtual AutoPtr<IAlgorithmParameters> EngineGenerateParameters();

private:
    friend class CAlgorithmParameterGenerator;
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_ALGORITHMPARAMETERGENERATORSPI_H__
