
#ifndef __ELASTOS_SECURITY_CERT_CERTPATHBUILDERSPI_H__
#define __ELASTOS_SECURITY_CERT_CERTPATHBUILDERSPI_H__

#include "Elastos.CoreLibrary.Security.h"
#include "core/Object.h"

namespace Elastos {
namespace Security {
namespace Cert {

class CertPathBuilderSpi
    : public Object
    , public ICertPathBuilderSpi
{
public:
    CAR_INTERFACE_DECL()

    CertPathBuilderSpi();
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CERTPATHBUILDERSPI_H__
