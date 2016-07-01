#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CCALLLOGADAPTERHELPER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CCALLLOGADAPTERHELPER_H__

#include "_Elastos_Droid_Dialer_CallLog_CCallLogAdapterHelper.h"
#include <elastos/core/Singleton.h>
#include "elastos/apps/dialer/calllog/CallLogAdapter.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

CarClass(CCallLogAdapterHelper)
    , public Singleton
    , public ICallLogAdapterHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ExpandVoicemailTranscriptionView(
        /* [in] */ ICallLogListItemViews* views,
        /* [in] */ Boolean isExpanded);
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CCALLLOGADAPTERHELPER_H__
