
#include "CTimeHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CTimeHelper)

CAR_INTERFACE_IMPL(CTimeHelper, Singleton, ITimeHelper)

ECode CTimeHelper::GetASN1(
    /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1Choice ** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

