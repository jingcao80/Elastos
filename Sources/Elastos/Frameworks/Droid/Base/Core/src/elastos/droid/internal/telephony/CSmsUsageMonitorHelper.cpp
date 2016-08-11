
#include "elastos/droid/internal/telephony/CSmsUsageMonitorHelper.h"
#include "elastos/droid/internal/telephony/SmsUsageMonitor.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CSmsUsageMonitorHelper, Object, ISmsUsageMonitorHelper)

CAR_SINGLETON_IMPL(CSmsUsageMonitorHelper)

ECode CSmsUsageMonitorHelper::MergeShortCodeCategories(
    /* [in] */ Int32 type1,
    /* [in] */ Int32 type2,
    /* [in] */ Int32* result)
{
    return SmsUsageMonitor::MergeShortCodeCategories(type1, type2, result);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
