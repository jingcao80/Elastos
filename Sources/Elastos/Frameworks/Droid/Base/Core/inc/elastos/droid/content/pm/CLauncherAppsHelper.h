
#ifndef __ELASTOS_DROID_CONTENT_PM_CLAUNCHERAPPSHELPER_H__
#define __ELASTOS_DROID_CONTENT_PM_CLAUNCHERAPPSHELPER_H__

#include "_Elastos_Droid_Content_Pm_CLauncherAppsHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CLauncherAppsHelper)
    , public Singleton
    , public ILauncherAppsHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetComponentName(
        /* [in] */ IResolveInfo* info,
        /* [out] */ IComponentName** cname);
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CLAUNCHERAPPSHELPER_H__
