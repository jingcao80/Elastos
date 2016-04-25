
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CCLEARCALLLOGDIALOGHELPER_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CCLEARCALLLOGDIALOGHELPER_H__

#include "_Elastos_Apps_Dialer_CallLog_CClearCallLogDialogHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace CallLog {

CarClass(CClearCallLogDialogHelper)
    , public Singleton
    , public IClearCallLogDialogHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /** Removes the missed call notifications. */
    CARAPI Show(
         /* [in] */ IFragmentManager* fragmentManager);

};

} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CCLEARCALLLOGDIALOGHELPER_H__
