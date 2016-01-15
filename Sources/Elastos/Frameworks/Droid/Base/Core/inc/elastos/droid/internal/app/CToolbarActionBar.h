
#ifndef __ELASTOS_DROID_INTERNAL_APP_CTOOLBARACTIONBAR_H__
#define __ELASTOS_DROID_INTERNAL_APP_CTOOLBARACTIONBAR_H__

#include "_Elastos_Droid_Internal_App_CToolbarActionBar.h"
#include "elastos/droid/internal/app/ToolbarActionBar.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CarClass(CToolbarActionBar)
    , public ToolbarActionBar
{
public:
    CAR_OBJECT_DECL()
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_CTOOLBARACTIONBAR_H__
