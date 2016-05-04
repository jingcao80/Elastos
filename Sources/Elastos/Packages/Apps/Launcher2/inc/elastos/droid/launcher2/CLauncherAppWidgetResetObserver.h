#ifndef  __ELASTOS_DROID_LAUNCHER2_CLAUNCHERAPPWIDGETRESETOBSERVER_H__
#define  __ELASTOS_DROID_LAUNCHER2_CLAUNCHERAPPWIDGETRESETOBSERVER_H__

#include "_Elastos_Droid_Launcher2_CLauncherAppWidgetResetObserver.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/Launcher.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CLauncherAppWidgetResetObserver)
    , public Launcher::AppWidgetResetObserver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CLAUNCHERAPPWIDGETRESETOBSERVER_H__