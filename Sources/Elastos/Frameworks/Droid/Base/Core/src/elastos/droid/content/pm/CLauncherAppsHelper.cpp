
#include "elastos/droid/content/pm/CLauncherAppsHelper.h"
#include "elastos/droid/content/pm/CLauncherApps.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL(CLauncherAppsHelper, Singleton, ILauncherAppsHelper)

CAR_SINGLETON_IMPL(CLauncherAppsHelper)

ECode CLauncherAppsHelper::GetComponentName(
    /* [in] */ IResolveInfo* info,
    /* [out] */ IComponentName** cname)
{
    VALIDATE_NOT_NULL(cname);

    AutoPtr<IComponentName> name = CLauncherApps::GetComponentName(info);
    *cname = name;
    REFCOUNT_ADD(*cname);
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
