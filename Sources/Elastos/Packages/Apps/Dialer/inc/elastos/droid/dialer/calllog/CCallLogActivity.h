
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CCALLLOGACTIVITY_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CCALLLOGACTIVITY_H__

#include "_Elastos_Droid_Dialer_CallLog_CCallLogActivity.h"
#include "elastos/apps/dialer/calllog/CallLogActivity.h"

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CCALLLOGACTIVITY_H__
