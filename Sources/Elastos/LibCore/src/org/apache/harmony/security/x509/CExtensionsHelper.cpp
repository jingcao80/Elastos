
#include "CExtensionsHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CExtensionsHelper)

CAR_INTERFACE_IMPL(CExtensionsHelper, Singleton, IExtensionsHelper)

ECode CExtensionsHelper::GetASN1(
    /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1Type ** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

