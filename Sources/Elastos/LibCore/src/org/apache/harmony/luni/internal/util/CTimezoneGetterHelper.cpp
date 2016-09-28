#include "org/apache/harmony/luni/internal/util/CTimezoneGetterHelper.h"
#include "org/apache/harmony/luni/internal/util/TimezoneGetter.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Luni {
namespace Internal {
namespace Util {

CAR_INTERFACE_IMPL(CTimezoneGetterHelper, Singleton, ITimezoneGetterHelper)
CAR_SINGLETON_IMPL(CTimezoneGetterHelper)

ECode CTimezoneGetterHelper::GetInstance(
    /* [out] */ ITimezoneGetter** result)
{
    VALIDATE_NOT_NULL(result);
    *result = TimezoneGetter::GetInstance();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTimezoneGetterHelper::SetInstance(
    /* [in] */ ITimezoneGetter* getter)
{
    TimezoneGetter::SetInstance(getter);
    return NOERROR;
}

} // namespace Util
} // namespace Internal
} // namespace Luni
} // namespace Harmony
} // namespace Apache
} // namespace Org
