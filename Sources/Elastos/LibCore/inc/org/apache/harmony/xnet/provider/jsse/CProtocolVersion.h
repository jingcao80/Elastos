
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CPROTOCOLVERSION_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CPROTOCOLVERSION_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CProtocolVersion.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CProtocolVersion)
{
public:
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
