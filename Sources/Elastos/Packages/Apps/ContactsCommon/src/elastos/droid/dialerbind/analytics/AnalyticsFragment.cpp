
#include "elastos/droid/dialerbind/analytics/AnalyticsFragment.h"

namespace Elastos {
namespace Droid {
namespace DialerBind {
namespace Analytics {

CAR_INTERFACE_IMPL(AnalyticsFragment, Fragment, IAnalyticsInterface)

ECode AnalyticsFragment::constructor()
{
    return Fragment::constructor();
}

ECode AnalyticsFragment::SendHitEvent(
    /* [in] */ const String& categoryId,
    /* [in] */ const String& actionId,
    /* [in] */ const String& labelId)
{
    return NOERROR;
}

ECode AnalyticsFragment::SendScreenView()
{
    return NOERROR;
}

} // Analytics
} // DialerBind
} // Droid
} // Elastos
