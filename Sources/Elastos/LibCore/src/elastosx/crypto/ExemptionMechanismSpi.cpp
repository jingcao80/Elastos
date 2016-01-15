#include "ExemptionMechanismSpi.h"

namespace Elastosx {
namespace Crypto {

CAR_INTERFACE_IMPL(ExemptionMechanismSpi, Object, IExemptionMechanismSpi)

ExemptionMechanismSpi::ExemptionMechanismSpi()
{
}

ExemptionMechanismSpi::~ExemptionMechanismSpi()
{
}

ECode ExemptionMechanismSpi::constructor()
{
    return NOERROR;
}

} // Crypto
} // Elastosx