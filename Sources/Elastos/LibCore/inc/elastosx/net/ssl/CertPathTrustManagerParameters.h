
#ifndef __ELASTOSX_NET_SSL_CERTPATHTRUSTMANAGERPARAMETERS_H__
#define __ELASTOSX_NET_SSL_CERTPATHTRUSTMANAGERPARAMETERS_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Security.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Security::Cert::ICertPathParameters;
using Elastosx::Net::Ssl::IManagerFactoryParameters;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * Certification path parameters to provide to certification path
 * based {@link TrustManager}.
 *
 * @since 1.5
 */
class CertPathTrustManagerParameters
    : public Object
    , public ICertPathTrustManagerParameters
    , public IManagerFactoryParameters
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a new {@code CertPathTrustManagerParameters} with the specified
     * certification path parameters.
     *
     * @param parameters
     *            the certification path parameters.
     */
    CARAPI constructor(
        /* [in] */ ICertPathParameters* parameters);

    /**
     * Returns a copy of the certification path parameters.
     *
     * @return a copy of the certification path parameters.
     */
    CARAPI GetParameters(
        /* [out] */ ICertPathParameters** parameters);

private:
    /*const*/ AutoPtr<ICertPathParameters> mParam;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_CERTPATHTRUSTMANAGERPARAMETERS_H__
