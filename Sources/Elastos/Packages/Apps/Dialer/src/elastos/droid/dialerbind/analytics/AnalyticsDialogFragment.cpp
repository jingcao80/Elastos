
#include "elastos/droid/dialerbind/analytics/AnalyticsDialogFragment.h"

namespace Elastos {
namespace Droid {
namespace DialerBind {
namespace Analytics {

CAR_INTERFACE_IMPL(AnalyticsDialogFragment, DialogFragment, IAnalyticsInterface)

ECode AnalyticsDialogFragment::SendHitEvent(
    /* [in] */ const String& categoryId,
    /* [in] */ const String& actionId,
    /* [in] */ const String& labelId)
{
    return NOERROR;
}

ECode AnalyticsDialogFragment::SendScreenView()
{
    return NOERROR;
}

} // Analytics
} // DialerBind
} // Droid
} // Elastos
