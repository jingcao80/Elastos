
#include "elastos/droid/dialer/calllog/CCallLogAdapterHelper.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

CAR_INTERFACE_IMPL(CCallLogAdapterHelper, Singleton, ICallLogAdapterHelper);

CAR_SINGLETON_IMPL(CCallLogAdapterHelper);

ECode CCallLogAdapterHelper::ExpandVoicemailTranscriptionView(
    /* [in] */ ICallLogListItemViews* views,
    /* [in] */ Boolean isExpanded)
{
    CallLogAdapter::ExpandVoicemailTranscriptionView(views, isExpanded);
    return NOERROR;
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
