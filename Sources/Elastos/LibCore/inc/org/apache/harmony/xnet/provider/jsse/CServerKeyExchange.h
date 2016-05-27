
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSERVERKEYEXCHANGE_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSERVERKEYEXCHANGE_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CServerKeyExchange.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CServerKeyExchange)
    , public Object
    , public IServerKeyExchange
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetLength(
        /* [out] */ Int32 * pLength);

    CARAPI GetRSAPublicKey(
        /* [out] */ Elastos::Security::Interfaces::IRSAPublicKey ** ppKey);

    CARAPI constructor(
        /* [in] */ Elastos::Math::IBigInteger * pPar1,
        /* [in] */ Elastos::Math::IBigInteger * pPar2,
        /* [in] */ Elastos::Math::IBigInteger * pPar3,
        /* [in] */ ArrayOf<Byte> * pHash);

    CARAPI constructor(
        /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IHandshakeIODataStream * pIds,
        /* [in] */ Int32 length,
        /* [in] */ Int32 keyExchange);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSERVERKEYEXCHANGE_H__
