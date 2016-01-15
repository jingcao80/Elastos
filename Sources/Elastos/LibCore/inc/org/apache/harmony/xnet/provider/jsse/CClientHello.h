
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCLIENTHELLO_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCLIENTHELLO_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CClientHello.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CClientHello)
{
public:
    CARAPI GetLength(
        /* [out] */ Int32 * pLength);

    CARAPI GetRandom(
        /* [out, callee] */ ArrayOf<Byte> ** ppRandom);

    CARAPI constructor(
        /* [in] */ Elastos::Security::ISecureRandom * pSr,
        /* [in] */ ArrayOf<Byte> * pVer,
        /* [in] */ ArrayOf<Byte> * pSes_id,
        /* [in] */ ArrayOf<Org::Apache::Harmony::Xnet::Provider::Jsse::ICipherSuite *> * pCipher_suite);

    CARAPI constructor(
        /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IHandshakeIODataStream * pIds,
        /* [in] */ Int32 length);

    CARAPI constructor(
        /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IHandshakeIODataStream * pIds);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCLIENTHELLO_H__
