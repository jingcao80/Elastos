
#ifndef __ELASTOS_DROID_APP_CBUILDER_REMOTE_VIEWS_H__
#define __ELASTOS_DROID_APP_CBUILDER_REMOTE_VIEWS_H__

#include "_Elastos_Droid_App_CNotificationBuilderRemoteViews.h"
#include "elastos/droid/app/CNotification.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CNotificationBuilderRemoteViews)
    , public CNotification::BuilderRemoteViews
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CBUILDER_REMOTE_VIEWS_H__