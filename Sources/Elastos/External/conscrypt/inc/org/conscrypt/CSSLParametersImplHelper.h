
#ifndef __ORG_CONSCRYPT_CSSLPARAMETERSIMPLHELPER_H__
#define __ORG_CONSCRYPT_CSSLPARAMETERSIMPLHELPER_H__

#include "_Org_Conscrypt_CSSLParametersImplHelper.h"
#include <elastos/core/Singleton.h>

using Elastosx::Net::Ssl::IX509TrustManager;

namespace Org {
namespace Conscrypt {

CarClass(CSSLParametersImplHelper)
    , public Singleton
    , public ISSLParametersImplHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Gets the default X.509 trust manager.
     *
     * TODO: Move this to a published API under dalvik.system.
     */
    CARAPI GetDefaultX509TrustManager(
        /* [out] */ IX509TrustManager** result);

    /**
     * Similar to getServerKeyType, but returns value given TLS
     * ClientCertificateType byte values from a CertificateRequest
     * message for use with X509KeyManager.chooseClientAlias or
     * X509ExtendedKeyManager.chooseEngineClientAlias.
     */
    CARAPI GetClientKeyType(
        /* [in] */ Byte keyType,
        /* [out] */ String* result);
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_CSSLPARAMETERSIMPLHELPER_H__
