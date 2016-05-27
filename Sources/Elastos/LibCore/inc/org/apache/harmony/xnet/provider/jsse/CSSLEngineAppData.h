
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLENGINEAPPDATA_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLENGINEAPPDATA_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CSSLEngineAppData.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CSSLEngineAppData)
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CARAPI Append(
        /* [in] */ ArrayOf<Byte> * pSrc);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLENGINEAPPDATA_H__
