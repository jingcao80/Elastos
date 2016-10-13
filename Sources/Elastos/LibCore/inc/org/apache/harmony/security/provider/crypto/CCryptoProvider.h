
#ifndef __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CCRYPTOPROVIDER_H__
#define __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CCRYPTOPROVIDER_H__

#include "_Org_Apache_Harmony_Security_Provider_Crypto_CCryptoProvider.h"
#include "Provider.h"

using Elastos::Security::Provider;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

/**
 * Implementation of Provider for SecureRandom. The implementation supports the
 * "SHA1PRNG" algorithm described in JavaTM Cryptography Architecture, API
 * Specification & Reference
 */
CarClass(CCryptoProvider)
    , public Elastos::Security::Provider
{
public:
    CAR_OBJECT_DECL()

    /**
     * Creates a Provider and puts parameters
     */
    CARAPI constructor();

private:
    // private static final long serialVersionUID = 7991202868423459598L;
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CCRYPTOPROVIDER_H__
