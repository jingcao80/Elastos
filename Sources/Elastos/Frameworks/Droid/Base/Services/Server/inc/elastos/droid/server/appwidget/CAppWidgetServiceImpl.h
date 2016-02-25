
#ifndef __ELASTOS_DROID_SERVER_APPWIDGET_CAPPWIDGETSERVICEIMPL_H__
#define __ELASTOS_DROID_SERVER_APPWIDGET_CAPPWIDGETSERVICEIMPL_H__

#include "_Elastos_Droid_Server_AppWidget_CAppWidgetServiceImpl.h"
#include "elastos/droid/server/appwidget/AppWidgetServiceImpl.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace AppWidget {

CarClass(CAppWidgetServiceImpl)
    , public AppWidgetServiceImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace AppWidget
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_APPWIDGET_CAPPWIDGETSERVICEIMPL_H__

