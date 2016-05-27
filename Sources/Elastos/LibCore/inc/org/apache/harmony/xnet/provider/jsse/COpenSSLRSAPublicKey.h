
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLRSAPUBLICKEY_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLRSAPUBLICKEY_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_COpenSSLRSAPublicKey.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(COpenSSLRSAPublicKey)
    , public Object
    , public IOpenSSLRSAPublicKey
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetPublicExponent(
        /* [out] */ Elastos::Math::IBigInteger ** ppPublicExponent);

    CARAPI Equals(
        /* [in] */ IInterface * pObj,
        /* [out] */ Boolean * pIsEqual);

    CARAPI GetHashCode(
        /* [out] */ Int32 * pHashCode);

    CARAPI ToString(
        /* [out] */ String * pStr);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLRSAPUBLICKEY_H__
