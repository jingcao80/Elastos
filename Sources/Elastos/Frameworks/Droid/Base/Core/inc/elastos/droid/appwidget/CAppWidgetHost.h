
#ifndef __ELASTOS_DROID_APPWIDGET_CAPPWIDGETHOST_H__
#define __ELASTOS_DROID_APPWIDGET_CAPPWIDGETHOST_H__

#include "_Elastos_Droid_AppWidget_CAppWidgetHost.h"
#include "elastos/droid/appwidget/AppWidgetHost.h"

namespace Elastos {
namespace Droid {
namespace AppWidget {

CarClass(CAppWidgetHost)
    , public AppWidgetHost
{
public:
    CAR_OBJECT_DECL()
};

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APPWIDGET_CAPPWIDGETHOST_H__
