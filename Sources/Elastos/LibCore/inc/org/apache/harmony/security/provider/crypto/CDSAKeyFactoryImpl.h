
#ifndef __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CDSAKEYFACTORYIMPL_H__
#define __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CDSAKEYFACTORYIMPL_H__

#include "_Org_Apache_Harmony_Security_Provider_Crypto_CDSAKeyFactoryImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

CarClass(CDSAKeyFactoryImpl)
{
public:
    CAR_OBJECT_DECL()

    CARAPI EngineGeneratePublic(
        /* [in] */ Elastos::Security::Spec::IKeySpec * pKeySpec,
        /* [out] */ Elastos::Security::IPublicKey ** ppPubKey);

    CARAPI EngineGeneratePrivate(
        /* [in] */ Elastos::Security::Spec::IKeySpec * pKeySpec,
        /* [out] */ Elastos::Security::IPrivateKey ** ppPriKey);

    CARAPI EngineGetKeySpec(
        /* [in] */ Elastos::Security::IKey * pKey,
        /* [in] */ const ClassID & keySpec,
        /* [out] */ Elastos::Security::Spec::IKeySpec ** ppRetkeySpec);

    CARAPI EngineTranslateKey(
        /* [in] */ Elastos::Security::IKey * pKey,
        /* [out] */ Elastos::Security::IKey ** ppTranslatedKey);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CDSAKEYFACTORYIMPL_H__
