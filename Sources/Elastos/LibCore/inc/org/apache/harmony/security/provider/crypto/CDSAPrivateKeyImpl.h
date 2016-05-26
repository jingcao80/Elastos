
#ifndef __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CDSAPRIVATEKEYIMPL_H__
#define __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CDSAPRIVATEKEYIMPL_H__

#include "_Org_Apache_Harmony_Security_Provider_Crypto_CDSAPrivateKeyImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

CarClass(CDSAPrivateKeyImpl)
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetAlgorithm(
        /* [out] */ String * pAlgorithm);

    CARAPI GetFormat(
        /* [out] */ String * pFormat);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncoded);

    CARAPI SetAlgorithm(
        /* [in] */ const String& algorithm);

    CARAPI SetEncoding(
        /* [in] */ ArrayOf<Byte> * pEncoding);

    CARAPI GetParams(
        /* [out] */ Elastos::Security::Interfaces::IDSAParams ** ppParams);

    CARAPI GetX(
        /* [out] */ Elastos::Math::IBigInteger ** ppX);

    CARAPI constructor(
        /* [in] */ Elastos::Security::Spec::IDSAPrivateKeySpec * pKeySpec);

    CARAPI constructor(
        /* [in] */ Elastos::Security::Spec::IPKCS8EncodedKeySpec * pKeySpec);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CDSAPRIVATEKEYIMPL_H__
