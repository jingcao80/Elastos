
#ifndef __ELASTOS_DROID_APP_CLAUNCHER_ACTIVITY_ICON_RESIZER_H__
#define __ELASTOS_DROID_APP_CLAUNCHER_ACTIVITY_ICON_RESIZER_H__

#include "_Elastos_Droid_App_CLauncherActivityIconResizer.h"
#include "elastos/droid/app/LauncherActivity.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CLauncherActivityIconResizer)
    , public LauncherActivity::IconResizer
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CLAUNCHER_ACTIVITY_ICON_RESIZER_H__