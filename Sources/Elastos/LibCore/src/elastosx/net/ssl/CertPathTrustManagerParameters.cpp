
#include "Elastos.CoreLibrary.Security.h"
#include "CertPathTrustManagerParameters.h"

using Elastos::Core::ICloneable;

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL_2(CertPathTrustManagerParameters, Object, ICertPathTrustManagerParameters, IManagerFactoryParameters)

ECode CertPathTrustManagerParameters::constructor(
    /* [in] */ ICertPathParameters* parameters)
{
    AutoPtr<IInterface> newDate;
    ICloneable::Probe(parameters)->Clone((IInterface**)&newDate);
    mParam = ICertPathParameters::Probe(newDate);
    return NOERROR;
}

ECode CertPathTrustManagerParameters::GetParameters(
    /* [out] */ ICertPathParameters** parameters)
{
    VALIDATE_NOT_NULL(parameters)

    AutoPtr<IInterface> newDate;
    ICloneable::Probe(mParam)->Clone((IInterface**)&newDate);
    *parameters = ICertPathParameters::Probe(newDate);
    REFCOUNT_ADD(*parameters);
    return NOERROR;
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
