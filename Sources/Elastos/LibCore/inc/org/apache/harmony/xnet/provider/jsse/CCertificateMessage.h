
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCERTIFICATEMESSAGE_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCERTIFICATEMESSAGE_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CCertificateMessage.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CCertificateMessage)
{
public:
    CARAPI GetLength(
        /* [out] */ Int32 * pLength);

    CARAPI GetAuthType(
        /* [out] */ String * pAuthType);

    CARAPI constructor(
        /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IHandshakeIODataStream * pIds,
        /* [in] */ Int32 length);

    CARAPI constructor(
        /* [in] */ ArrayOf<Elastos::Security::Cert::IX509Certificate *> * pCerts);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCERTIFICATEMESSAGE_H__
