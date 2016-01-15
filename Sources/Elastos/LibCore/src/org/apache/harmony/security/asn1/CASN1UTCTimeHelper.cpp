
#include "CASN1UTCTimeHelper.h"
#include <CASN1UTCTime.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {


ECode CASN1UTCTimeHelper::GetInstance(
    /* [out] */ IASN1UTCTime** instance)
{
    return CASN1UTCTime::GetInstance(instance);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

