
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCERTPINMANAGER_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCERTPINMANAGER_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CCertPinManager.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CCertPinManager)
    , public Object
    , public ICertPinManager
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI ChainIsNotPinned(
        /* [in] */ const String& hostname,
        /* [in] */ Elastos::Utility::IList * pChain,
        /* [out] */ Boolean * pIsNotPinned);

    CARAPI constructor(
        /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ITrustedCertificateStore * pStore);

    CARAPI constructor(
        /* [in] */ const String& path,
        /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ITrustedCertificateStore * pStore);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCERTPINMANAGER_H__
