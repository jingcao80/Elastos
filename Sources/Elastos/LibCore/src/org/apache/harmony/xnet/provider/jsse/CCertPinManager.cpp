
#include "CCertPinManager.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(CCertPinManager)

CAR_INTERFACE_IMPL(CCertPinManager, Object, ICertPinManager)

ECode CCertPinManager::ChainIsNotPinned(
    /* [in] */ const String& hostname,
    /* [in] */ Elastos::Utility::IList * pChain,
    /* [out] */ Boolean * pIsNotPinned)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertPinManager::constructor(
    /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ITrustedCertificateStore * pStore)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertPinManager::constructor(
    /* [in] */ const String& path,
    /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ITrustedCertificateStore * pStore)
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

