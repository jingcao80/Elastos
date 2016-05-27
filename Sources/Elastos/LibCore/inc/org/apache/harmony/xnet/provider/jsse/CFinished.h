
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CFINISHED_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CFINISHED_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CFinished.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CFinished)
    , public Object
    , public IFinished
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetLength(
        /* [out] */ Int32 * pLength);

    CARAPI GetData(
        /* [out, callee] */ ArrayOf<Byte> ** ppData);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * pBytes);

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

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CFINISHED_H__
