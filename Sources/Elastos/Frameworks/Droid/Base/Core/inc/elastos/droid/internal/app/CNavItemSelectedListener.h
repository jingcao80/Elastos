
#ifndef __ELASTOS_DROID_INTERNAL_APP_CNAVITEMSELECTEDLISTENER_H__
#define __ELASTOS_DROID_INTERNAL_APP_CNAVITEMSELECTEDLISTENER_H__

#include "_Elastos_Droid_Internal_App_CNavItemSelectedListener.h"
#include "elastos/droid/internal/app/NavItemSelectedListener.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CarClass(CNavItemSelectedListener)
    , public NavItemSelectedListener
{
public:
    CAR_OBJECT_DECL()
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_CNAVITEMSELECTEDLISTENER_H__
