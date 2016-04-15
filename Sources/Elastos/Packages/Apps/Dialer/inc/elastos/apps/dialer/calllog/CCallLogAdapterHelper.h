#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CCALLLOGADAPTERHELPER_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CCALLLOGADAPTERHELPER_H__

#include "_Elastos_Apps_Dialer_CallLog_CCallLogAdapterHelper.h"
#include <elastos/core/Singleton.h>
#include "CallLogAdapter.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace CallLog {

CarClass(CCallLogAdapterHelper)
    , public singleton
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
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CCALLLOGADAPTERHELPER_H__
