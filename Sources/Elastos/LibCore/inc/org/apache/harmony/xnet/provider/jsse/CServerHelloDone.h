
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSERVERHELLODONE_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSERVERHELLODONE_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CServerHelloDone.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CServerHelloDone)
{
public:
    CARAPI GetLength(
        /* [out] */ Int32 * pLength);

    CARAPI constructor();

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

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSERVERHELLODONE_H__
