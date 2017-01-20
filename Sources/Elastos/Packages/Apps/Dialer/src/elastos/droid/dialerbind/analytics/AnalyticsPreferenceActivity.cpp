
#include "elastos/droid/dialerbind/analytics/AnalyticsPreferenceActivity.h"

namespace Elastos {
namespace Droid {
namespace DialerBind {
namespace Analytics {

CAR_INTERFACE_IMPL(AnalyticsPreferenceActivity, PreferenceActivity, IAnalyticsInterface)

ECode AnalyticsPreferenceActivity::SendHitEvent(
    /* [in] */ const String& categoryId,
    /* [in] */ const String& actionId,
    /* [in] */ const String& labelId)
{
    return NOERROR;
}

ECode AnalyticsPreferenceActivity::SendScreenView()
{
    return NOERROR;
}

} // Analytics
} // DialerBind
} // Droid
} // Elastos
