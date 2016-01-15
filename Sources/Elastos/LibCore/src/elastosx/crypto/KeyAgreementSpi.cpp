#include "KeyAgreementSpi.h"

namespace Elastosx {
namespace Crypto {

CAR_INTERFACE_IMPL(KeyAgreementSpi, Object, IKeyAgreementSpi)

KeyAgreementSpi::KeyAgreementSpi()
{
}

KeyAgreementSpi::~KeyAgreementSpi()
{
}

ECode KeyAgreementSpi::constructor()
{
    return NOERROR;
}

} // Crypto
} // Elastosx