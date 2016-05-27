
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLRSAPRIVATEKEY_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLRSAPRIVATEKEY_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_COpenSSLRSAPrivateKey.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(COpenSSLRSAPrivateKey)
    , public Object
    , public IOpenSSLRSAPrivateKey
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

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

    CARAPI constructor(
        /* [in] */ Elastos::Security::Spec::IRSAPrivateKeySpec * pRsaKeySpec);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLRSAPRIVATEKEY_H__
