
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCERTIFICATEVERIFY_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCERTIFICATEVERIFY_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CCertificateVerify.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CCertificateVerify)
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetLength(
        /* [out] */ Int32 * pLength);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * pHash);

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

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCERTIFICATEVERIFY_H__
