
#ifndef __ELASTOS_DROID_APP_CLAUNCHER_ACTIVITY_LISTITEM_H__
#define __ELASTOS_DROID_APP_CLAUNCHER_ACTIVITY_LISTITEM_H__

#include "_Elastos_Droid_App_CLauncherActivityListItem.h"
#include "elastos/droid/app/LauncherActivity.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CLauncherActivityListItem)
    , public LauncherActivity::ListItem
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CLAUNCHER_ACTIVITY_LISTITEM_H__