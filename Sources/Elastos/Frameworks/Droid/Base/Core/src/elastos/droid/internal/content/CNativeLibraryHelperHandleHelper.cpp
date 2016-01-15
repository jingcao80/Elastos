
#include "elastos/droid/internal/content/CNativeLibraryHelperHandleHelper.h"
#include "elastos/droid/internal/content/CNativeLibraryHelperHandle.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Content {

CAR_INTERFACE_IMPL(CNativeLibraryHelperHandleHelper, Singleton, INativeLibraryHelperHandleHelper)
CAR_SINGLETON_IMPL(CNativeLibraryHelperHandleHelper)

ECode CNativeLibraryHelperHandleHelper::Create(
    /* [in] */ IFile* packageFile,
    /* [out] */ INativeLibraryHelperHandle** handle)
{
    return CNativeLibraryHelperHandle::Create(packageFile, handle);
}

ECode CNativeLibraryHelperHandleHelper::Create(
    /* [in] */ Handle32 pkg,
    /* [out] */ INativeLibraryHelperHandle** handle)
{
    return CNativeLibraryHelperHandle::Create(pkg, handle);
}

ECode CNativeLibraryHelperHandleHelper::Create(
    /* [in] */ IPackageLite* lite,
    /* [out] */ INativeLibraryHelperHandle** handle)
{
    return CNativeLibraryHelperHandle::Create(lite, handle);
}

} // namespace Content
} // namespace Internal
} // namespace Droid
} // namespace Elastos
