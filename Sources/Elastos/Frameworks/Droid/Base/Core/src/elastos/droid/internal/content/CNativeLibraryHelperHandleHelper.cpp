
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

ECode CNativeLibraryHelperHandleHelper::CreatePackage(
    /* [in] */ Handle64 pkg,
    /* [out] */ INativeLibraryHelperHandle** handle)
{
    return CNativeLibraryHelperHandle::CreatePackage(pkg, handle);
}

ECode CNativeLibraryHelperHandleHelper::CreatePackageLite(
    /* [in] */ Handle64 lite,
    /* [out] */ INativeLibraryHelperHandle** handle)
{
    return CNativeLibraryHelperHandle::CreatePackageLite(lite, handle);
}

} // namespace Content
} // namespace Internal
} // namespace Droid
} // namespace Elastos
