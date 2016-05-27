
#include "CTSTInfoHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CAR_SINGLETON_IMPL(CTSTInfoHelper)

CAR_INTERFACE_IMPL(CTSTInfoHelper, Singleton, ITSTInfoHelper)

ECode CTSTInfoHelper::GetACCURACY(
    /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1Sequence ** ppAccuracy)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfoHelper::SetACCURACY(
    /* [in] */ Org::Apache::Harmony::Security::Asn1::IASN1Sequence * pAccuracy)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfoHelper::GetASN1(
    /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1Sequence ** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfoHelper::SetASN1(
    /* [in] */ Org::Apache::Harmony::Security::Asn1::IASN1Sequence * pAsn1)
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

