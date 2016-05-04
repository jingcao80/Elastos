
#ifndef __ELASTOS_DROID_SERVER_APPWIDGET_CAPPWIDGETSERVICEIMPLBROADCASTRECEIVER_H__
#define __ELASTOS_DROID_SERVER_APPWIDGET_CAPPWIDGETSERVICEIMPLBROADCASTRECEIVER_H__

#include "_Elastos_Droid_Server_AppWidget_CAppWidgetServiceImplBroadcastReceiver.h"
#include "elastos/droid/server/appwidget/AppWidgetServiceImpl.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace AppWidget {

CarClass(CAppWidgetServiceImplBroadcastReceiver)
    , public AppWidgetServiceImpl::InnerBroadcastReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace AppWidget
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_APPWIDGET_CAPPWIDGETSERVICEIMPLBROADCASTRECEIVER_H__
