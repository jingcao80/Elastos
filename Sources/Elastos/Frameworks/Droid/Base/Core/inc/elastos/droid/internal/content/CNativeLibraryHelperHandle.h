
#ifndef __ELASTOS_DROID_INTERNAL_CONTENT_CNATIVELIBRARYHELPERHANDLE_H__
#define __ELASTOS_DROID_INTERNAL_CONTENT_CNATIVELIBRARYHELPERHANDLE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "_Elastos_Droid_Internal_Content_CNativeLibraryHelperHandle.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::Pm::IPackageLite;
using Elastos::Core::Object;
using Elastos::Core::ICloseGuard;
using Elastos::IO::IFile;
using Elastos::IO::ICloseable;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Content {

CarClass(CNativeLibraryHelperHandle)
    , public Object
    , public INativeLibraryHelperHandle
    , public ICloseable
{
    friend class CNativeLibraryHelper;
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CNativeLibraryHelperHandle();

    ~CNativeLibraryHelperHandle();

    CARAPI constructor(
        /* [in] */ ArrayOf<Int64>* apkHandles,
        /* [in] */ Boolean multiArch);

    CARAPI Close();

public:
    static CARAPI Create(
        /* [in] */ IFile* packageFile,
        /* [out] */ INativeLibraryHelperHandle** handle);

    static CARAPI Create(
        /* [in] */ Handle32 _pkg,
        /* [out] */ INativeLibraryHelperHandle** handle);

    static CARAPI Create(
        /* [in] */ IPackageLite* _lite,
        /* [out] */ INativeLibraryHelperHandle** handle);

private:
    static CARAPI Create(
        /* [in] */ List<String>* codePaths,
        /* [in] */ Boolean multiArch,
        /* [out] */ INativeLibraryHelperHandle** handle);

private:
    AutoPtr<ICloseGuard> mGuard;
    volatile Boolean mClosed;

    AutoPtr<ArrayOf<Int64> > mApkHandles;
    Boolean mMultiArch;
};

} // namespace Content
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_CONTENT_CNATIVELIBRARYHELPERHANDLE_H__
