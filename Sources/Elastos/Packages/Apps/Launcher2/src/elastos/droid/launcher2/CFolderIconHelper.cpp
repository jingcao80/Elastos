
#include "elastos/droid/launcher2/CFolderIconHelper.h"
#include "elastos/droid/launcher2/FolderIcon.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_SINGLETON_IMPL(CFolderIconHelper)

CAR_INTERFACE_IMPL(CFolderIconHelper, Singleton, IFolderIconHelper)

ECode CFolderIconHelper::FromXml(
    /* [in] */ Int32 resId,
    /* [in] */ ILauncher* launcher,
    /* [in] */ IViewGroup* group,
    /* [in] */ IFolderInfo* folderInfo,
    /* [in] */ IIconCache* iconCache,
    /* [out] */ IFolderIcon** icon)
{
    VALIDATE_NOT_NULL(icon);

    return FolderIcon::FromXml(resId, launcher, group, folderInfo, iconCache, icon);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos