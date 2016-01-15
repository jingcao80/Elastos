
#ifndef __ELASTOS_DROID_APPWIDGET_CAPPWIDGETMANAGER_H__
#define __ELASTOS_DROID_APPWIDGET_CAPPWIDGETMANAGER_H__

#include "_Elastos_Droid_AppWidget_CAppWidgetManager.h"
#include "elastos/droid/appwidget/AppWidgetManager.h"

namespace Elastos {
namespace Droid {
namespace AppWidget {

CarClass(CAppWidgetManager)
    , public AppWidgetManager
{
public:
    CAR_OBJECT_DECL();
};

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APPWIDGET_CAPPWIDGETMANAGER_H__
