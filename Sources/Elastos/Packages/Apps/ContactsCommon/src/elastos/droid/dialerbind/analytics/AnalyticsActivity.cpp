
#include "elastos/droid/dialerbind/analytics/AnalyticsActivity.h"

namespace Elastos {
namespace Droid {
namespace DailerBind {
namespace Analytics {

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
} // DailerBind
} // Droid
} // Elastos
