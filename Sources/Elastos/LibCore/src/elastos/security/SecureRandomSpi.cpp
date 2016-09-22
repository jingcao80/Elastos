
#include "SecureRandomSpi.h"

using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Security {

CAR_INTERFACE_IMPL_2(SecureRandomSpi, Object, ISecureRandomSpi, ISerializable)
SecureRandomSpi::~SecureRandomSpi()
{}

} // namespace Security
} // namespace Elastos
