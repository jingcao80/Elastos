
#ifndef __ELASTOS_SECURITY_SECURERANDOMSPI_H__
#define __ELASTOS_SECURITY_SECURERANDOMSPI_H__

#include "Elastos.CoreLibrary.Security.h"
#include "Object.h"

using Elastos::IO::ISerializable;

namespace Elastos {
namespace Security {

/**
 * {@code SecureRandomSpi} is the <i>Service Provider Interface</i> (<b>SPI</b>) definition
 * for {@link SecureRandom}.
 *
 * @see SecureRandom
 */
class SecureRandomSpi
    : public Object
    , public ISecureRandomSpi
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    virtual ~SecureRandomSpi() = 0;
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_SECURERANDOMSPI_H__
