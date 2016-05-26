
#include "CGeneralNamesHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CGeneralNamesHelper)

CAR_INTERFACE_IMPL(CGeneralNamesHelper, Singleton, IGeneralNamesHelper)

ECode CGeneralNamesHelper::GetASN1(
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

