
#ifndef __ELASTOS_DROID_APPWIDGET_CAPPWIDGETHOSTCALLBACKS_H__
#define __ELASTOS_DROID_APPWIDGET_CAPPWIDGETHOSTCALLBACKS_H__

#include "_Elastos_Droid_AppWidget_CAppWidgetHostCallbacks.h"
#include "elastos/droid/appwidget/AppWidgetHost.h"

namespace Elastos {
namespace Droid {
namespace AppWidget {

CarClass(CAppWidgetHostCallbacks)
    , public AppWidgetHost::Callbacks
{
public:
    CAR_OBJECT_DECL();
};

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_APPWIDGET_CAPPWIDGETHOSTCALLBACKS_H__
