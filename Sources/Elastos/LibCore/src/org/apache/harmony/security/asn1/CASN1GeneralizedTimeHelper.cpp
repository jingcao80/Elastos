
#include "CASN1GeneralizedTimeHelper.h"
#include "CASN1GeneralizedTime.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {


ECode CASN1GeneralizedTimeHelper::GetInstance(
    /* [out] */ IASN1GeneralizedTime** instance)
{
    return CASN1GeneralizedTime::GetInstance(instance);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
