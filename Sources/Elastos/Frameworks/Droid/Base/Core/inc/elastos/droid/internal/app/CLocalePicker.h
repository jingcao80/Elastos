
#ifndef __ELASTOS_DROID_INTERNAL_APP_CLOCALEPICKER_H__
#define __ELASTOS_DROID_INTERNAL_APP_CLOCALEPICKER_H__

#include "_Elastos_Droid_Internal_App_CLocalePicker.h"
#include "elastos/droid/internal/app/LocalePicker.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CarClass(CLocalePicker)
    , public LocalePicker
{
public:
    CAR_OBJECT_DECL()
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_CLOCALEPICKER_H__
