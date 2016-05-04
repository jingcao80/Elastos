
#ifndef __ELASTOS_DROID_CONTENT_PM_CLAUNCHERAPPSONAPPSCHANGEDLISTENER_H__
#define __ELASTOS_DROID_CONTENT_PM_CLAUNCHERAPPSONAPPSCHANGEDLISTENER_H__

#include "_Elastos_Droid_Content_Pm_CLauncherAppsOnAppsChangedListener.h"
#include "elastos/droid/content/pm/CLauncherApps.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CLauncherAppsOnAppsChangedListener)
    , public CLauncherApps::MyOnAppsChangedListener
{
public:
    CAR_OBJECT_DECL()
};

}
}
}
}

#endif // __ELASTOS_DROID_CONTENT_PM_CLAUNCHERAPPSONAPPSCHANGEDLISTENER_H__
