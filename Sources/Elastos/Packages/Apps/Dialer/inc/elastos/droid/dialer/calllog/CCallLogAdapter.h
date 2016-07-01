#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CCALLLOGADAPTER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CCALLLOGADAPTER_H__

#include "_Elastos_Droid_Dialer_CallLog_CCallLogAdapter.h"
#include "elastos/apps/dialer/calllog/CallLogAdapter.h"

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CCALLLOGADAPTER_H__
