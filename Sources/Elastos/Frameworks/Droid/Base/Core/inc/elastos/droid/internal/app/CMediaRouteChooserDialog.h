
#ifndef __ELASTOS_DROID_INTERNAL_APP_CMEDIAROUTECHOOSERDIALOG_H__
#define __ELASTOS_DROID_INTERNAL_APP_CMEDIAROUTECHOOSERDIALOG_H__

#include "_Elastos_Droid_Internal_App_CMediaRouteChooserDialog.h"
#include "elastos/droid/internal/app/MediaRouteChooserDialog.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CarClass(CMediaRouteChooserDialog)
    , public MediaRouteChooserDialog
{
public:
    CAR_OBJECT_DECL()
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_CMEDIAROUTECHOOSERDIALOG_H__
