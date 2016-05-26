
#include "CReasonCodeHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CReasonCodeHelper)

CAR_INTERFACE_IMPL(CReasonCodeHelper, Singleton, IReasonCodeHelper)

ECode CReasonCodeHelper::GetASN1(
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

