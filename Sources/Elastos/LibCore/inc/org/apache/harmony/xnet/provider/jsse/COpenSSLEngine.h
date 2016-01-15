
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLENGINE_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLENGINE_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_COpenSSLEngine.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(COpenSSLEngine)
{
public:
    CARAPI GetPrivateKeyById(
        /* [in] */ const String& id,
        /* [out] */ Elastos::Security::IPrivateKey ** ppKey);

    CARAPI Equals(
        /* [in] */ IInterface * pObj,
        /* [out] */ Boolean * pIsEqual);

    CARAPI GetHashCode(
        /* [out] */ Int32 * pHashCode);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLENGINE_H__
