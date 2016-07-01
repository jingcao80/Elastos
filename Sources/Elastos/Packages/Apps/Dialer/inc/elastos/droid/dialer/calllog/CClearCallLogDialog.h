
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CCLEARCALLLOGDIALOG_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CCLEARCALLLOGDIALOG_H__

#include "_Elastos_Droid_Dialer_CallLog_CClearCallLogDialog.h"
#include "elastos/apps/dialer/calllog/ClearCallLogDialog.h"

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CCLEARCALLLOGDIALOG_H__
