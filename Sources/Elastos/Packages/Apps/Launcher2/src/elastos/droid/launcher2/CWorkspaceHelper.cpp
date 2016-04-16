
#include "elastos/droid/launcher2/CWorkspaceHelper.h"
#include "elastos/droid/launcher2/Workspace.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_SINGLETON_IMPL(CWorkspaceHelper)

CAR_INTERFACE_IMPL(CWorkspaceHelper, Singleton, IWorkspaceHelper)

ECode CWorkspaceHelper::GetCellLayoutMetrics(
    /* [in] */ ILauncher* launcher,
    /* [in] */ Int32 orientation,
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);

    return Workspace::GetCellLayoutMetrics(launcher, orientation, rect);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos