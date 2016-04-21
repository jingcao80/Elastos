#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CCALLLOGADAPTER_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CCALLLOGADAPTER_H__

#include "_Elastos_Apps_Dialer_CallLog_CCallLogAdapter.h"
#include "CallLogAdapter.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace CallLog {

CarClass(CCallLogAdapter)
    , public CallLogAdapter
{
public:
    CAR_OBJECT_DECL()
};

} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CCALLLOGADAPTER_H__
