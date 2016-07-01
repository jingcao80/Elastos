
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CCALLDETAILHISTORYADAPTER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CCALLDETAILHISTORYADAPTER_H__

#include "_Elastos_Droid_Dialer_CallLog_CCallDetailHistoryAdapter.h"
#include "elastos/apps/dialer/calllog/CallDetailHistoryAdapter.h"

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CCALLDETAILHISTORYADAPTER_H__
