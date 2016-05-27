
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CPROTOCOLVERSION_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CPROTOCOLVERSION_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CProtocolVersion.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CProtocolVersion)
    , public Object
    , public IProtocolVersion
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetName(
        /* [out] */ String * pName);

    CARAPI GetVersion(
        /* [out, callee] */ ArrayOf<Byte> ** ppVer);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CPROTOCOLVERSION_H__
