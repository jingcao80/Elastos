
#include "elastos/droid/dialerbind/analytics/AnalyticsActivity.h"

namespace Elastos {
namespace Droid {
namespace DialerBind {
namespace Analytics {

CAR_INTERFACE_IMPL(AnalyticsActivity, Activity, IAnalyticsInterface)

ECode AnalyticsActivity::SendHitEvent(
    /* [in] */ const String& categoryId,
    /* [in] */ const String& actionId,
    /* [in] */ const String& labelId)
{
    return NOERROR;
}

ECode AnalyticsActivity::SendScreenView()
{
    return NOERROR;
}

} // Analytics
} // DialerBind
} // Droid
} // Elastos
