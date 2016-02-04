#ifndef __ELASTOS_DROID_CONTENT_PM_CPACKAGEPARSERHELPER_H__
#define __ELASTOS_DROID_CONTENT_PM_CPACKAGEPARSERHELPER_H__

#include "_Elastos_Droid_Content_Pm_CPackageParserHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CPackageParserHelper)
    , public Singleton
    , public IPackageParserHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI IsApkFile(
        /* [in] */ IFile* file,
        /* [out] */ Boolean* res);

    CARAPI ParsePackageLite(
        /* [in] */ IFile* packageFile,
        /* [in] */ Int32 flags,
        /* [out] */ IPackageLite** res);
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTENT_PM_CPACKAGEPARSERHELPER_H__