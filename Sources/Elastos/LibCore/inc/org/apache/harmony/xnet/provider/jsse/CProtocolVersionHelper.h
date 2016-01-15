
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CPROTOCOLVERSIONHELPER_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CPROTOCOLVERSIONHELPER_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CProtocolVersionHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CProtocolVersionHelper)
{
public:
    CARAPI GetSupportedProtocols(
        /* [out, callee] */ ArrayOf<String> ** ppSupportedProtocols);

    CARAPI IsSupported(
        /* [in] */ ArrayOf<Byte> * pVer,
        /* [out] */ Boolean * pIsSupported);

    CARAPI GetByVersion(
        /* [in] */ ArrayOf<Byte> * pVer,
        /* [out] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IProtocolVersion ** ppVs);

    CARAPI IsSupportedEx(
        /* [in] */ const String& name,
        /* [out] */ Boolean * pIsSupported);

    CARAPI GetByName(
        /* [in] */ const String& name,
        /* [out] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IProtocolVersion ** ppVer);

    CARAPI GetLatestVersion(
        /* [in] */ ArrayOf<String> * pProtocols,
        /* [out] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IProtocolVersion ** ppVer);

    CARAPI GetSSLv3(
        /* [out] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IProtocolVersion ** ppSslv3);

    CARAPI GetTLSv1(
        /* [out] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IProtocolVersion ** ppTlsv1);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CPROTOCOLVERSIONHELPER_H__
