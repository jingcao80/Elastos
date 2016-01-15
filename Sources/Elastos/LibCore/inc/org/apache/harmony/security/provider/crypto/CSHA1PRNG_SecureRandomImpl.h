
#ifndef __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CSHA1PRNG_SECURERANDOMIMPL_H__
#define __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CSHA1PRNG_SECURERANDOMIMPL_H__

#include "_Org_Apache_Harmony_Security_Provider_Crypto_CSHA1PRNG_SecureRandomImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

CarClass(CSHA1PRNG_SecureRandomImpl)
{
public:
    CARAPI EngineSetSeed(
        /* [in] */ ArrayOf<Byte> * pSeed);

    CARAPI EngineNextBytes(
        /* [in] */ ArrayOf<Byte> * pBytes);

    CARAPI EngineGenerateSeed(
        /* [in] */ Int32 numBytes,
        /* [out, callee] */ ArrayOf<Byte> ** ppSeed);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CSHA1PRNG_SECURERANDOMIMPL_H__
