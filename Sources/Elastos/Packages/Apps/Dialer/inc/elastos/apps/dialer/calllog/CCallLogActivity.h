
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CCALLLOGACTIVITY_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CCALLLOGACTIVITY_H__

#include "_Elastos_Apps_Dialer_CallLog_CCallLogActivity.h"
#include "elastos/apps/dialer/calllog/CallLogActivity.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

CarClass(CCallLogActivity)
    , public CallLogActivity
{
public:
    CAR_OBJECT_DECL();
};


} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CCALLLOGACTIVITY_H__
