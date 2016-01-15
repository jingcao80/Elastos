
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CTRUSTEDCERTIFICATEINDEX_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CTRUSTEDCERTIFICATEINDEX_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CTrustedCertificateIndex.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CTrustedCertificateIndex)
{
public:
    CARAPI CheckServerTrustedEx(
        /* [in] */ ArrayOf<Elastos::Security::Cert::IX509Certificate *> * pChain,
        /* [in] */ const String& authType,
        /* [in] */ const String& host,
        /* [out] */ Elastos::Utility::IList ** ppCertificates);

    CARAPI HandleTrustStorageUpdate();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Elastos::Utility::ISet * pAnchors);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CTRUSTEDCERTIFICATEINDEX_H__
