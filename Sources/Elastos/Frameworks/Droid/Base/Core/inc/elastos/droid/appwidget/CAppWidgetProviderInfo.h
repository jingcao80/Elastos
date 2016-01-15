
#ifndef __ELASTOS_DROID_APPWIDGET_CAPPWIDGETPROVIDERINFO_H__
#define __ELASTOS_DROID_APPWIDGET_CAPPWIDGETPROVIDERINFO_H__

#include "_Elastos_Droid_AppWidget_CAppWidgetProviderInfo.h"
#include "elastos/droid/appwidget/AppWidgetProviderInfo.h"

namespace Elastos {
namespace Droid {
namespace AppWidget {

CarClass(CAppWidgetProviderInfo)
    , public AppWidgetProviderInfo
{
public:
    CAR_OBJECT_DECL();
};

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APPWIDGET_CAPPWIDGETPROVIDERINFO_H__
