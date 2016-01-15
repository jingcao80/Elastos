
#ifndef __ELASTOS_DROID_NET_HTTP_X509TRUSTMANAGEREXTENSIONS_H__
#define __ELASTOS_DROID_NET_HTTP_X509TRUSTMANAGEREXTENSIONS_H__

#include "Elastos.Droid.Net.h"
#include "_Elastos.Droid.Core.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Security::Cert::IX509Certificate;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;
using Elastosx::Net::Ssl::IX509TrustManager;

using Org::Conscrypt::ITrustManagerImpl;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * X509TrustManager wrapper exposing Android-added features.
 * <p>
 * The checkServerTrusted method allows callers to perform additional
 * verification of certificate chains after they have been successfully verified
 * by the platform.
 * </p>
 */
class X509TrustManagerExtensions
    : public Object
    , public IX509TrustManagerExtensions
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs a new X509TrustManagerExtensions wrapper.
     *
     * @param tm A {@link X509TrustManager} as returned by TrustManagerFactory.getInstance();
     * @throws IllegalArgumentException If tm is an unsupported TrustManager type.
     */
    CARAPI constructor(
        /* [in] */ IX509TrustManager* tm);

    /**
     * Verifies the given certificate chain.
     *
     * <p>See {@link X509TrustManager#checkServerTrusted(X509Certificate[], String)} for a
     * description of the chain and authType parameters. The final parameter, host, should be the
     * hostname of the server.</p>
     *
     * @throws CertificateException if the chain does not verify correctly.
     * @return the properly ordered chain used for verification as a list of X509Certificates.
     */
    CARAPI CheckServerTrusted(
        /* [in] */ ArrayOf<IX509Certificate*>* chain,
        /* [in] */ const String& authType,
        /* [in] */ const String& host,
        /* [out] */ IList** result);

    /**
     * Checks whether a CA certificate is added by an user.
     *
     * <p>Since {@link X509TrustManager#checkServerTrusted} allows its parameter {@code chain} to
     * chain up to user-added CA certificates, this method can be used to perform additional
     * policies for user-added CA certificates.
     *
     * @return {@code true} to indicate that the certificate was added by the user, {@code false}
     * otherwise.
     */
    CARAPI IsUserAddedCertificate(
        /* [in] */ IX509Certificate* cert,
        /* [out] */ Boolean* result);

public:
    AutoPtr<ITrustManagerImpl> mDelegate;
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_X509TRUSTMANAGEREXTENSIONS_H__
