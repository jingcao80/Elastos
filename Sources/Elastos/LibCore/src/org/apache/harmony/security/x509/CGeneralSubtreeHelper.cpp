
#include "CGeneralSubtreeHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CGeneralSubtreeHelper)

CAR_INTERFACE_IMPL(CGeneralSubtreeHelper, Singleton, IGeneralSubtreeHelper)

ECode CGeneralSubtreeHelper::GetASN1(
    /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1Sequence ** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

