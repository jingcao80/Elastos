
#include "CExtendedKeyUsageHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CExtendedKeyUsageHelper)

CAR_INTERFACE_IMPL(CExtendedKeyUsageHelper, Singleton, IExtendedKeyUsageHelper)

ECode CExtendedKeyUsageHelper::GetASN1(
    /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1Type ** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtendedKeyUsageHelper::SetASN1(
    /* [in] */ Org::Apache::Harmony::Security::Asn1::IASN1Type * pAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

