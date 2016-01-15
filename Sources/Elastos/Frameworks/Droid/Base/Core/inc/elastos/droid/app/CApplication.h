
#ifndef __ELASTOS_DROID_APP_CAPPLICATION_H__
#define __ELASTOS_DROID_APP_CAPPLICATION_H__

#include "_Elastos_Droid_App_CApplication.h"
#include "elastos/droid/app/Application.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CApplication)
    , public Application
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CAPPLICATION_H__
