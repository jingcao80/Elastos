
#include "CCRLDistributionPoints.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CCRLDistributionPoints)

CAR_INTERFACE_IMPL(CCRLDistributionPoints, Object, ICRLDistributionPoints)

ECode CCRLDistributionPoints::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCRLDistributionPoints::DumpValue(
    /* [in] */ Elastos::Core::IStringBuilder * pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCRLDistributionPoints::DumpValueEx(
    /* [in] */ Elastos::Core::IStringBuilder * pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

