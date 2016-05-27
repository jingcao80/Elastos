
#include "CTrustedCertificateStoreHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_SINGLETON_IMPL(CTrustedCertificateStoreHelper)

CAR_INTERFACE_IMPL(CTrustedCertificateStoreHelper, Singleton, ITrustedCertificateStoreHelper)

ECode CTrustedCertificateStoreHelper::IsSystem(
    /* [in] */ const String& alias,
    /* [out] */ Boolean * pIsSystem)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateStoreHelper::IsUser(
    /* [in] */ const String& alias,
    /* [out] */ Boolean * pIsUser)
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

