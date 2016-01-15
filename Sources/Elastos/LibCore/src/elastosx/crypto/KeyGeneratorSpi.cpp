#include "KeyGeneratorSpi.h"

namespace Elastosx {
namespace Crypto {

CAR_INTERFACE_IMPL(KeyGeneratorSpi, Object, IKeyGeneratorSpi)

KeyGeneratorSpi::KeyGeneratorSpi()
{
}

KeyGeneratorSpi::~KeyGeneratorSpi()
{
}

ECode KeyGeneratorSpi::constructor()
{
    return NOERROR;
}

} // Crypto
} // Elastosx