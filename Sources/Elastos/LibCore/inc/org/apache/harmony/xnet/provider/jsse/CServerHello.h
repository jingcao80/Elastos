
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSERVERHELLO_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSERVERHELLO_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CServerHello.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CServerHello)
{
public:
    CARAPI GetLength(
        /* [out] */ Int32 * pLength);

    CARAPI GetRandom(
        /* [out, callee] */ ArrayOf<Byte> ** ppRandom);

    CARAPI constructor(
        /* [in] */ Elastos::Security::ISecureRandom * pSr,
        /* [in] */ ArrayOf<Byte> * pServer_version,
        /* [in] */ ArrayOf<Byte> * pSession_id,
        /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ICipherSuite * pCipher_suite,
        /* [in] */ Byte compression_method);

    CARAPI constructor(
        /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IHandshakeIODataStream * pIds,
        /* [in] */ Int32 length);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSERVERHELLO_H__
