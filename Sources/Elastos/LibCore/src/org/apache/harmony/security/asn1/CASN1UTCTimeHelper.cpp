
#include "CASN1UTCTimeHelper.h"
#include <CASN1UTCTime.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CAR_SINGLETON_IMPL(CASN1UTCTimeHelper)

CAR_INTERFACE_IMPL(CASN1UTCTimeHelper, Singleton, IASN1UTCTimeHelper)

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

