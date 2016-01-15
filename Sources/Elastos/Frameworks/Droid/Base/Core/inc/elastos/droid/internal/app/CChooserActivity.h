
#ifndef __ELASTOS_DROID_INTERNAL_APP_CCHOOSERACTIVITY_H__
#define __ELASTOS_DROID_INTERNAL_APP_CCHOOSERACTIVITY_H__

#include "_Elastos_Droid_Internal_App_CChooserActivity.h"
#include "elastos/droid/internal/app/ChooserActivity.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CarClass(CChooserActivity)
    , public ChooserActivity
{
public:
    CAR_OBJECT_DECL()
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_CCHOOSERACTIVITY_H__
