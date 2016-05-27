
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLDSAPUBLICKEY_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLDSAPUBLICKEY_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_COpenSSLDSAPublicKey.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(COpenSSLDSAPublicKey)
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetParams(
        /* [out] */ Elastos::Security::Interfaces::IDSAParams ** ppParameters);

    CARAPI GetY(
        /* [out] */ Elastos::Math::IBigInteger ** ppY);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLDSAPUBLICKEY_H__
