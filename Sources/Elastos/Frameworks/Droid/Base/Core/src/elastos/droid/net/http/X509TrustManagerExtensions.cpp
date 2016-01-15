
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.h>
#include "elastos/droid/net/http/X509TrustManagerExtensions.h"
#include "elastos/droid/R.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::R;

using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;
using Elastosx::Net::Ssl::EIID_ITrustManager;
using Elastosx::Net::Ssl::ISSLParameters;
using Elastosx::Net::Ssl::ISSLSocket;
using Elastosx::Net::Ssl::ITrustManager;
using Elastosx::Net::Ssl::IX509TrustManager;

using Org::Conscrypt::ITrustManagerImpl;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL(X509TrustManagerExtensions, Object, IX509TrustManagerExtensions)

ECode X509TrustManagerExtensions::constructor(
    /* [in] */ IX509TrustManager* tm)
{
    VALIDATE_NOT_NULL(tm);

    AutoPtr<ITrustManagerImpl> impl;
    // TODO: Waiting for TrustManagerImpl
    assert(0);
    // impl = ITrustManagerImpl::Probe(tm);

    if (impl == NULL) {
            // throw new IllegalArgumentException("tm is an instance of " + tm.getClass().getName() +
            //         " which is not a supported type of X509TrustManager");
        Logger::E("X509TrustManagerExtensions", "tm is not an instance of X509TrustManager");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mDelegate = impl;
    return NOERROR;
}

ECode X509TrustManagerExtensions::CheckServerTrusted(
    /* [in] */ ArrayOf<IX509Certificate*>* chain,
    /* [in] */ const String& authType,
    /* [in] */ const String& host,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    // TODO: Waiting for TrustManagerImpl
    assert(0);
    // return mDelegate->CheckServerTrusted(chain, authType, host, result);
    return NOERROR;
}

ECode X509TrustManagerExtensions::IsUserAddedCertificate(
    /* [in] */ IX509Certificate* cert,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // TODO: Waiting for TrustManagerImpl
    assert(0);
    // return mDelegate->IsUserAddedCertificate(cert, result);
    return NOERROR;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
