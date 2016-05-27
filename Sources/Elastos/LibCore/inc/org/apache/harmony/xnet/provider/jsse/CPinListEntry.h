
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CPINLISTENTRY_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CPINLISTENTRY_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CPinListEntry.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CPinListEntry)
    , public Object
    , public IPinListEntry
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCommonName(
        /* [out] */ String * pCommonName);

    CARAPI GetEnforcing(
        /* [out] */ Boolean * pEnforcing);

    CARAPI ChainIsNotPinned(
        /* [in] */ Elastos::Utility::IList * pChain,
        /* [out] */ Boolean * pIsNotPinned);

    CARAPI constructor(
        /* [in] */ const String& entry,
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

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CPINLISTENTRY_H__
