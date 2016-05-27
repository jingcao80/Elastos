
#include "CTrustedCertificateIndex.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(CTrustedCertificateIndex)

CAR_INTERFACE_IMPL(CTrustedCertificateIndex, Object, ITrustedCertificateIndex)

ECode CTrustedCertificateIndex::CheckServerTrustedEx(
    /* [in] */ ArrayOf<Elastos::Security::Cert::IX509Certificate *> * pChain,
    /* [in] */ const String& authType,
    /* [in] */ const String& host,
    /* [out] */ Elastos::Utility::IList ** ppCertificates)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateIndex::HandleTrustStorageUpdate()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateIndex::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateIndex::constructor(
    /* [in] */ Elastos::Utility::ISet * pAnchors)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}
}

