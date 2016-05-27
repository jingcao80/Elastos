
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCERTIFICATEREQUEST_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCERTIFICATEREQUEST_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CCertificateRequest.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CCertificateRequest)
    , public Object
    , public ICertificateRequest
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetLength(
        /* [out] */ Int32 * pLength);

    CARAPI GetTypesAsString(
        /* [out, callee] */ ArrayOf<String> ** ppTypes);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * pCertificate_types,
        /* [in] */ ArrayOf<Elastos::Security::Cert::IX509Certificate *> * pAccepted);

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

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCERTIFICATEREQUEST_H__
