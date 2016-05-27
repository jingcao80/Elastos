
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSERVERKEYEXCHANGEHELPER_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSERVERKEYEXCHANGEHELPER_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CServerKeyExchangeHelper.h"

#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CServerKeyExchangeHelper)
    , public Singleton
    , public IServerKeyExchangeHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI ToUnsignedByteArray(
        /* [in] */ Elastos::Math::IBigInteger * pBi,
        /* [out, callee] */ ArrayOf<Byte> ** ppByteArray);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSERVERKEYEXCHANGEHELPER_H__
