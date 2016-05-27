
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLRSAPRIVATECRTKEY_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLRSAPRIVATECRTKEY_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_COpenSSLRSAPrivateCrtKey.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(COpenSSLRSAPrivateCrtKey)
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetPrivateExponent(
        /* [out] */ Elastos::Math::IBigInteger ** ppPrivateExponent);

    CARAPI GetPkeyContext(
        /* [out] */ Int32 * pCtx);

    CARAPI GetPkeyAlias(
        /* [out] */ String * pAlias);

    CARAPI Equals(
        /* [in] */ IInterface * pObj,
        /* [out] */ Boolean * pIsEqual);

    CARAPI GetHashCode(
        /* [out] */ Int32 * pHashCode);

    CARAPI ToString(
        /* [out] */ String * pStr);

    CARAPI GetCrtCoefficient(
        /* [out] */ Elastos::Math::IBigInteger ** ppCrtCoefficient);

    CARAPI GetPrimeP(
        /* [out] */ Elastos::Math::IBigInteger ** ppPrimeP);

    CARAPI GetPrimeQ(
        /* [out] */ Elastos::Math::IBigInteger ** ppPrimeQ);

    CARAPI GetPrimeExponentP(
        /* [out] */ Elastos::Math::IBigInteger ** ppPrimeExponentP);

    CARAPI GetPrimeExponentQ(
        /* [out] */ Elastos::Math::IBigInteger ** ppPrimeExponentQ);

    CARAPI GetPublicExponent(
        /* [out] */ Elastos::Math::IBigInteger ** ppPublicExponent);

    CARAPI constructor(
        /* [in] */ Elastos::Security::Spec::IRSAPrivateCrtKeySpec * pRsaKeySpec);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLRSAPRIVATECRTKEY_H__
