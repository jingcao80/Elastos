
#ifndef __ELASTOS_SECURITY_KEYPAIRGENERATORSPI_H__
#define __ELASTOS_SECURITY_KEYPAIRGENERATORSPI_H__

#include "Elastos.CoreLibrary.Security.h"
#include <elastos/core/Object.h>

using Elastos::Security::Spec::IAlgorithmParameterSpec;

namespace Elastos {
namespace Security {

/**
 * {@code KeyPairGeneratorSpi} is the Service Provider Interface (SPI)
 * definition for {@link KeyPairGenerator}.
 *
 * @see KeyPairGenerator
 */
class ECO_PUBLIC KeyPairGeneratorSpi
    : public Object
    , public IKeyPairGeneratorSpi
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs a new instance of {@code KeyPairGeneratorSpi}.
     */
    KeyPairGeneratorSpi()
    {}

    virtual ~KeyPairGeneratorSpi() = 0;

    /**
     * Initializes this {@code KeyPairGeneratorSpi} with the given {@code
     * AlgorithmParameterSpec} and the given {@code SecureRandom}.
     *
     * @param params
     *            the parameters to use.
     * @param random
     *            the source of randomness.
     * @throws InvalidAlgorithmParameterException
     *             if the specified parameters are not supported.
     */
    CARAPI Initialize(
        /* [in] */ IAlgorithmParameterSpec* params,
        /* [in] */ ISecureRandom* random) /*throws InvalidAlgorithmParameterException*/;

    virtual CARAPI Initialize(
        /* [in] */ Int32 keysize,
        /* [in] */ ISecureRandom* random) = 0;
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_KEYPAIRGENERATORSPI_H__
