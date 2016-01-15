#include "SecretKeyFactorySpi.h"

namespace Elastosx {
namespace Crypto {

CAR_INTERFACE_IMPL(SecretKeyFactorySpi, Object, ISecretKeyFactorySpi)

SecretKeyFactorySpi::SecretKeyFactorySpi()
{
}

SecretKeyFactorySpi::~SecretKeyFactorySpi()
{
}

ECode SecretKeyFactorySpi::constructor()
{
    return NOERROR;
}


} // Crypto
} // Elastosx