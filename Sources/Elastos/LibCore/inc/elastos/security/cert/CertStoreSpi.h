
#ifndef __ELASTOS_SECURITY_CERT_CERTSTORESPI_H__
#define __ELASTOS_SECURITY_CERT_CERTSTORESPI_H__

#include "Elastos.CoreLibrary.Security.h"
#include "core/Object.h"

namespace Elastos {
namespace Security {
namespace Cert {

/**
 * The <i>Service Provider Interface</i> (<b>SPI</b>) for the {@code
 * CertPathValidator} class to be implemented by security providers.
 */
class CertStoreSpi
    : public Object
    , public ICertStoreSpi
{
public:
    CAR_INTERFACE_DECL()

    CertStoreSpi();
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CERTSTORESPI_H__
