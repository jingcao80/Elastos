
#include "KeyPairGeneratorSpi.h"

namespace Elastos {
namespace Security {

CAR_INTERFACE_IMPL(KeyPairGeneratorSpi, Object, IKeyPairGeneratorSpi)
KeyPairGeneratorSpi::~KeyPairGeneratorSpi()
{}

ECode KeyPairGeneratorSpi::Initialize(
    /* [in] */ IAlgorithmParameterSpec* params,
    /* [in] */ ISecureRandom* random) /*throws InvalidAlgorithmParameterException*/
{
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

} // namespace Security
} // namespace Elastos
