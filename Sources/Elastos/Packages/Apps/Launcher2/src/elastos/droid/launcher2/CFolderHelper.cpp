
#include "elastos/droid/launcher2/CFolderHelper.h"
#include "elastos/droid/launcher2/Folder.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_SINGLETON_IMPL(CFolderHelper)

CAR_INTERFACE_IMPL(CFolderHelper, Singleton, IFolderHelper)

ECode CFolderHelper::FromXml(
    /* [in] */ IContext* context,
    /* [out] */ IFolder** folder)
{
    VALIDATE_NOT_NULL(folder);

    return Folder::FromXml(context, folder);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos