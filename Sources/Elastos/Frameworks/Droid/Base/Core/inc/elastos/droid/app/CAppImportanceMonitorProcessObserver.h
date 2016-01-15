
#ifndef __ELASTOS_DROID_APP_CAPPIMPORTANCE_MONITOR_PROCESS_OBSERVER_H__
#define __ELASTOS_DROID_APP_CAPPIMPORTANCE_MONITOR_PROCESS_OBSERVER_H__

#include "_Elastos_Droid_App_CAppImportanceMonitorProcessObserver.h"
#include "elastos/droid/app/AppImportanceMonitor.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CAppImportanceMonitorProcessObserver)
    , public AppImportanceMonitor::ProcessObserver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CAPPIMPORTANCE_MONITOR_PROCESS_OBSERVER_H__