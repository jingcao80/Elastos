
#ifndef __ELASTOS_DROID_APPWIDGET_CAPPWIDGETHOSTHELPER_H__
#define  __ELASTOS_DROID_APPWIDGET_CAPPWIDGETHOSTHELPER_H__

#include "_Elastos_Droid_AppWidget_CAppWidgetHostHelper.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace AppWidget {

CarClass(CAppWidgetHostHelper)
    , public Singleton
    , public IAppWidgetHostHelper
{
public:
    CAR_SINGLETON_DECL();

    CAR_INTERFACE_DECL();
    /**
     * Remove all records about all hosts for your package.
     * <ul>
     *   <li>Call this when initializing your database, as it might be because of a data wipe.</li>
     *   <li>Call this to have the AppWidget manager release all resources associated with your
     *   host.  Any future calls about this host will cause the records to be re-allocated.</li>
     * </ul>
     */
    CARAPI DeleteAllHosts();
};

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_APPWIDGET_CAPPWIDGETHOSTHELPER_H__
