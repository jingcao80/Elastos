
#ifndef __ELASTOS_DROID_INTERNAL_APP_CMEDIAROUTECONTROLLERDIALOG_H__
#define __ELASTOS_DROID_INTERNAL_APP_CMEDIAROUTECONTROLLERDIALOG_H__

#include "_Elastos_Droid_Internal_App_CMediaRouteControllerDialog.h"
#include "elastos/droid/internal/app/MediaRouteControllerDialog.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CarClass(CMediaRouteControllerDialog)
    , public MediaRouteControllerDialog
{
public:
    CAR_OBJECT_DECL()
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_CMEDIAROUTECONTROLLERDIALOG_H__
