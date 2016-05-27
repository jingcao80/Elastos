
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSERVERSESSIONCONTEXT_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSERVERSESSIONCONTEXT_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CServerSessionContext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CServerSessionContext)
    , public Object
    , public IServerSessionContext
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetIds(
        /* [out] */ Elastos::Utility::IEnumeration ** ppIds);

    CARAPI GetSession(
        /* [in] */ ArrayOf<Byte> * pSessionId,
        /* [out] */ Elastosx::Net::Ssl::ISSLSession ** ppSession);

    CARAPI GetSessionCacheSize(
        /* [out] */ Int32 * pSize);

    CARAPI GetSessionTimeout(
        /* [out] */ Int32 * pTimeout);

    CARAPI SetSessionCacheSize(
        /* [in] */ Int32 size);

    CARAPI SetSessionTimeout(
        /* [in] */ Int32 seconds);

    CARAPI SetPersistentCache(
        /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ISSLServerSessionCache * pPersistentCache);

    CARAPI constructor();

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSERVERSESSIONCONTEXT_H__
