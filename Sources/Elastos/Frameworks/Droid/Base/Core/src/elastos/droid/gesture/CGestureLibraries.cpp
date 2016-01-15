
#include "elastos/droid/gesture/CGestureLibraries.h"
#include "elastos/droid/gesture/GestureLibraries.h"

using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::CFile;

namespace Elastos {
namespace Droid {
namespace Gesture {

CAR_SINGLETON_IMPL(CGestureLibraries)

CAR_INTERFACE_IMPL(CGestureLibraries, Singleton, IGestureLibraries)

ECode CGestureLibraries::FromFile(
    /* [in] */ const String& path,
    /* [out] */ IGestureLibrary** gestureLib)
{
    VALIDATE_NOT_NULL(gestureLib);
    AutoPtr<IGestureLibrary> ret = GestureLibraries::FromFile(path);
    *gestureLib = ret;
    REFCOUNT_ADD(*gestureLib);
    return NOERROR;
}

CARAPI CGestureLibraries::FromFile(
    /* [in] */ IFile* path,
    /* [out] */ IGestureLibrary** gestureLib)
{
    VALIDATE_NOT_NULL(gestureLib);
    AutoPtr<IGestureLibrary> ret = GestureLibraries::FromFile(path);
    *gestureLib = ret;
    REFCOUNT_ADD(*gestureLib);
    return NOERROR;
}

CARAPI CGestureLibraries::FromPrivateFile(
    /* [in] */ IContext* ctx,
    /* [in] */ const String& name,
    /* [out] */ IGestureLibrary** gestureLib)
{
    VALIDATE_NOT_NULL(gestureLib);
    AutoPtr<IGestureLibrary> ret = GestureLibraries::FromPrivateFile(ctx, name);
    *gestureLib = ret;
    REFCOUNT_ADD(*gestureLib);
    return NOERROR;
}

CARAPI CGestureLibraries::FromRawResource(
    /* [in] */ IContext* ctx,
    /* [in] */ Int32 resourceId,
    /* [out] */ IGestureLibrary** gestureLib)
{
    VALIDATE_NOT_NULL(gestureLib);
    AutoPtr<IGestureLibrary> ret = GestureLibraries::FromRawResource(ctx, resourceId);
    *gestureLib = ret;
    REFCOUNT_ADD(*gestureLib);
    return NOERROR;
}

} // namespace Gesture
} // namespace Droid
} // namespace Elastos
