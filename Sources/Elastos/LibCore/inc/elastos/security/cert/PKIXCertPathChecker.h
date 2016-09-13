
#ifndef __ELASTOS_SECURITY_CERT_CERTIFICATEFACTORYSPI_H__
#define __ELASTOS_SECURITY_CERT_CERTIFICATEFACTORYSPI_H__

#include "Elastos.CoreLibrary.Security.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Core::ICloneable;

namespace Elastos {
namespace Security {
namespace Cert {

/**
 * This class defines the <i>Service Provider Interface</i> (<b>SPI</b>) for the
 * {@code CertificateFactory} class. This SPI must be implemented for each
 * certificate type a security provider wishes to support.
 */
class PKIXCertPathChecker
    : public Object
    , public IPKIXCertPathChecker
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs a new instance of this class.
     */
    PKIXCertPathChecker();
};

} // end Cert
} // end Security
} // end Elastos

#endif // __ELASTOS_SECURITY_CERT_CERTIFICATEFACTORYSPI_H__
