
#ifndef __ELASTOS_SECURITY_CERT_CERTPATHVALIDATORSPI_H__
#define __ELASTOS_SECURITY_CERT_CERTPATHVALIDATORSPI_H__

#include "Elastos.CoreLibrary.Security.h"
#include "core/Object.h"

namespace Elastos {
namespace Security {
namespace Cert {

/**
 * The <i>Service Provider Interface</i> (<b>SPI</b>) for the {@code
 * CertPathValidator} class to be implemented by security providers.
 */
class CertPathValidatorSpi
    : public Object
    , public ICertPathValidatorSpi
{
public:
    CAR_INTERFACE_DECL()

    CertPathValidatorSpi();
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CERTPATHVALIDATORSPI_H__
