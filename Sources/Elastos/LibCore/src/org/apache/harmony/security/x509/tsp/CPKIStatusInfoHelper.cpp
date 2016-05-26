
#include "CPKIStatusInfoHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CAR_SINGLETON_IMPL(CPKIStatusInfoHelper)

CAR_INTERFACE_IMPL(CPKIStatusInfoHelper, Singleton, IPKIStatusInfoHelper)

ECode CPKIStatusInfoHelper::GetASN1(
    /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1Sequence ** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPKIStatusInfoHelper::SetASN1(
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

