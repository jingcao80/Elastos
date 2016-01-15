
#ifndef __ELASTOSX_CRYPTO_EXEMPTIONMECHANISMSPI_H__
#define __ELASTOSX_CRYPTO_EXEMPTIONMECHANISMSPI_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Security::IKey;
using Elastos::Security::IAlgorithmParameters;
using Elastos::Security::Spec::IAlgorithmParameterSpec;

namespace Elastosx {
namespace Crypto {

class ExemptionMechanismSpi
    : public Object
    , public IExemptionMechanismSpi
{
public:
    CAR_INTERFACE_DECL()

    ExemptionMechanismSpi();

    virtual ~ExemptionMechanismSpi();

    /**
     * Creates a new {@code ExemptionMechanismSpi} instance.
     */
    CARAPI constructor();

    /**
     * Generates the result key blob for this exemption mechanism.
     *
     * @return the result key blob for this exemption mechanism.
     * @throws ExemptionMechanismException
     *             if error(s) occur during generation.
     */
    virtual CARAPI EngineGenExemptionBlob(
        /* [out, callee] */ ArrayOf<Byte> ** blob) = 0;

    /**
     * Generates the result key blob for this exemption mechanism and stores it
     * into the {@code output} buffer at offset {@code outputOffset}.
     *
     * @param output
     *            the output buffer for the result key blob.
     * @param outputOffset
     *            the offset in the output buffer to start.
     * @return the number of bytes written to the {@code output} buffer.
     * @throws ShortBufferException
     *             if the provided buffer is too small for the result key blob.
     * @throws ExemptionMechanismException
     *             if error(s) occur during generation.
     */
    virtual CARAPI EngineGenExemptionBlob(
        /* [in] */ ArrayOf<Byte> * output,
        /* [in] */ Int32 outputOffset,
        /* [out] */ Int32 * value) = 0;

    /**
     * Returns the size in bytes for the output buffer needed to hold the output
     * of the next {@link #engineGenExemptionBlob} call, given the specified
     * {@code inputLen} (in bytes).
     *
     * @param inputLen
     *            the specified input length (in bytes).
     * @return the size in bytes for the output buffer.
     */
    virtual CARAPI EngineGetOutputSize(
        /* [in] */ Int32 inputLen,
        /* [out] */ Int32 * value) = 0;

    /**
     * Initializes this {@code ExemptionMechanism} instance with the specified
     * key.
     *
     * @param key
     *            the key to initialize this instance with.
     * @throws InvalidKeyException
     *             if the key cannot be used to initialize this mechanism.
     * @throws ExemptionMechanismException
     *             if error(s) occur during initialization.
     */
    virtual CARAPI EngineInit(
        /* [in] */ IKey * key) = 0;

    /**
     * Initializes this {@code ExemptionMechanism} instance with the specified
     * key and algorithm parameters.
     *
     * @param key
     *            the key to initialize this instance with.
     * @param params
     *            the parameters for this exemption mechanism algorithm.
     * @throws InvalidKeyException
     *             if the key cannot be used to initialize this mechanism.
     * @throws InvalidAlgorithmParameterException
     *             if the parameters cannot be used to initialize this
     *             mechanism.
     * @throws ExemptionMechanismException
     *             if error(s) occur during initialization.
     */
    virtual CARAPI EngineInit(
        /* [in] */ IKey * key,
        /* [in] */ IAlgorithmParameters * params) = 0;

    /**
     * Initializes this {@code ExemptionMechanism} instance with the specified
     * key and algorithm parameters.
     *
     * @param key
     *            the key to initialize this instance with.
     * @param params
     *            the parameters for this exemption mechanism algorithm.
     * @throws InvalidKeyException
     *             if the key cannot be used to initialize this mechanism.
     * @throws InvalidAlgorithmParameterException
     *             the the parameters cannot be used to initialize this
     *             mechanism.
     * @throws ExemptionMechanismException
     *             if error(s) occur during initialization.
     */
    virtual CARAPI EngineInit(
        /* [in] */ IKey * key,
        /* [in] */ IAlgorithmParameterSpec * params) = 0;

};

}
}

#endif // __ELASTOSX_CRYPTO_EXEMPTIONMECHANISMSPI_H__
