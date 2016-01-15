
#ifndef __ELASTOS_DROID_NET_HTTP_CCERTIFICATECHAINVALIDATORHELPER_H__
#define __ELASTOS_DROID_NET_HTTP_CCERTIFICATECHAINVALIDATORHELPER_H__

#include "_Elastos_Droid_Net_Http_CCertificateChainValidatorHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::IArrayOf;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * Class responsible for all server certificate validation functionality
 *
 * {@hide}
 */
CarClass(CCertificateChainValidatorHelper)
    , public Singleton
    , public ICertificateChainValidatorHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * @return The singleton instance of the certificates chain validator
     */
    CARAPI GetInstance(
        /* [out] */ ICertificateChainValidator** result);

    /**
     * Similar to doHandshakeAndValidateServerCertificates but exposed to JNI for use
     * by Chromium HTTPS stack to validate the cert chain.
     * @param certChain The bytes for certificates in ASN.1 DER encoded certificates format.
     * @param domain The full website hostname and domain
     * @param authType The authentication type for the cert chain
     * @return An SSL error object if there is an error and null otherwise
     */
    CARAPI VerifyServerCertificates(
        /* [in] */ ArrayOf<IArrayOf*>* certChain,
        /* [in] */ const String& domain,
        /* [in] */ const String& authType,
        /* [out] */ ISslError** result);

    /**
     * Handles updates to credential storage.
     */
    CARAPI HandleTrustStorageUpdate();
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_HTTP_CCERTIFICATECHAINVALIDATORHELPER_H__

