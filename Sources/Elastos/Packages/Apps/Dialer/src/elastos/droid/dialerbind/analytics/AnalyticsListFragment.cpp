
#include "elastos/droid/dialerbind/analytics/AnalyticsListFragment.h"

namespace Elastos {
namespace Droid {
namespace DialerBind {
namespace Analytics {

CAR_INTERFACE_IMPL(AnalyticsListFragment, ListFragment, IAnalyticsInterface)

ECode AnalyticsListFragment::SendHitEvent(
    /* [in] */ const String& categoryId,
    /* [in] */ const String& actionId,
    /* [in] */ const String& labelId)
{
    return NOERROR;
}

ECode AnalyticsListFragment::SendScreenView()
{
    return NOERROR;
}

} // Analytics
} // DialerBind
} // Droid
} // Elastos
