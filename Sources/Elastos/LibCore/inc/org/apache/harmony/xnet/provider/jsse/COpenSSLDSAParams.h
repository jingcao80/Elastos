
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLDSAPARAMS_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLDSAPARAMS_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_COpenSSLDSAParams.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(COpenSSLDSAParams)
{
public:
    CARAPI GetG(
        /* [out] */ Elastos::Math::IBigInteger ** ppBase);

    CARAPI GetP(
        /* [out] */ Elastos::Math::IBigInteger ** ppPrime);

    CARAPI GetQ(
        /* [out] */ Elastos::Math::IBigInteger ** ppSubprime);

    CARAPI GetY(
        /* [out] */ Elastos::Math::IBigInteger ** ppY);

    CARAPI GetX(
        /* [out] */ Elastos::Math::IBigInteger ** ppX);

    CARAPI Equals(
        /* [in] */ IInterface * pObj,
        /* [out] */ Boolean * pIsEqual);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLDSAPARAMS_H__
