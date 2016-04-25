
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CCLEARCALLLOGDIALOG_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CCLEARCALLLOGDIALOG_H__

#include "_Elastos_Apps_Dialer_CallLog_CClearCallLogDialog.h"
#include "ClearCallLogDialog.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace CallLog {

CarClass(CClearCallLogDialog)
    , public ClearCallLogDialog
{
public:
    CAR_OBJECT_DECL()

};

} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CCLEARCALLLOGDIALOG_H__
