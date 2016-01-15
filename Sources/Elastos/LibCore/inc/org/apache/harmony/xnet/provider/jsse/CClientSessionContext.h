
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCLIENTSESSIONCONTEXT_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCLIENTSESSIONCONTEXT_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CClientSessionContext.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CClientSessionContext)
{
public:
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

    CARAPI GetSize(
        /* [out] */ Int32 * pSize);

    CARAPI SetPersistentCache(
        /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ISSLClientSessionCache * pPersistentCache);

    CARAPI GetSessionEx(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [out] */ Elastosx::Net::Ssl::ISSLSession ** ppSession);

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

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCLIENTSESSIONCONTEXT_H__
