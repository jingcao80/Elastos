
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CCLEARCALLLOGDIALOGHELPER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CCLEARCALLLOGDIALOGHELPER_H__

#include "_Elastos_Droid_Dialer_CallLog_CClearCallLogDialogHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::App::IFragmentManager;

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CCLEARCALLLOGDIALOGHELPER_H__
