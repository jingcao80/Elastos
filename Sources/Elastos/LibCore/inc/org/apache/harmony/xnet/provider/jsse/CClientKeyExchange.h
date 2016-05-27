
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCLIENTKEYEXCHANGE_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCLIENTKEYEXCHANGE_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CClientKeyExchange.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CClientKeyExchange)
    , public Object
    , public IClientKeyExchange
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetLength(
        /* [out] */ Int32 * pLength);

    CARAPI IsEmpty(
        /* [out] */ Boolean * pIsEmpty);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * pEncrypted_pre_master_secret,
        /* [in] */ Boolean isTLS);

    CARAPI constructor(
        /* [in] */ Elastos::Math::IBigInteger * pDh_Yc);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IHandshakeIODataStream * pIds,
        /* [in] */ Int32 length,
        /* [in] */ Boolean isTLS,
        /* [in] */ Boolean isRSA);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCLIENTKEYEXCHANGE_H__
