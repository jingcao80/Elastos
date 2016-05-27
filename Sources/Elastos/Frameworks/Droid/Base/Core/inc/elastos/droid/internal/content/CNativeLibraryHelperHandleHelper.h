
#ifndef __ELASTOS_DROID_INTERNAL_CONTENT_CNATIVELIBRARYHELPERHANDLEHELPER_H__
#define __ELASTOS_DROID_INTERNAL_CONTENT_CNATIVELIBRARYHELPERHANDLEHELPER_H__

#include "_Elastos_Droid_Internal_Content_CNativeLibraryHelperHandleHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::Pm::IPackageLite;
using Elastos::Core::Singleton;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Content {

CarClass(CNativeLibraryHelperHandleHelper)
    , public Singleton
    , public INativeLibraryHelperHandleHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Create(
        /* [in] */ IFile* packageFile,
        /* [out] */ INativeLibraryHelperHandle** handle);

    CARAPI CreatePackage(
        /* [in] */ Handle64 pkg,
        /* [out] */ INativeLibraryHelperHandle** handle);

    CARAPI Create(
        /* [in] */ IPackageLite* lite,
        /* [out] */ INativeLibraryHelperHandle** handle);
};

} // namespace Content
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_CONTENT_CNATIVELIBRARYHELPERHANDLEHELPER_H__
