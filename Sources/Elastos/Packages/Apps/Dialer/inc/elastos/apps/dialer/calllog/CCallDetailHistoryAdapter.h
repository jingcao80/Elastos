
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CCALLDETAILHISTORYADAPTER_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CCALLDETAILHISTORYADAPTER_H__

#include "_Elastos_Apps_Dialer_CallLog_CCallDetailHistoryAdapter.h"
#include "CallDetailHistoryAdapter.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace CallLog {

CarClass(CCallDetailHistoryAdapter)
    , public CallDetailHistoryAdapter
{
public:
    CAR_OBJECT_DECL()
};


} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CCALLDETAILHISTORYADAPTER_H__
