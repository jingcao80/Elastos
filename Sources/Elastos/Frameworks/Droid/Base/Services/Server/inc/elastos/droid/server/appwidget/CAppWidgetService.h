
#ifndef __ELASTOS_DROID_SERVER_APPWIDGET_CAPPWIDGET_SERVICE_H__
#define __ELASTOS_DROID_SERVER_APPWIDGET_CAPPWIDGET_SERVICE_H__

#include "_Elastos_Droid_Server_AppWidget_CAppWidgetService.h"
#include "elastos/droid/server/appwidget/AppWidgetService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace AppWidget {

CarClass(CAppWidgetService)
    , public AppWidgetService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace AppWidget
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_APPWIDGET_CAPPWIDGET_SERVICE_H__
