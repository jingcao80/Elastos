
#ifndef __ELASTOS_DROID_CONTENT_PM_CPACKAGEMANAGERHELPER_H__
#define __ELASTOS_DROID_CONTENT_PM_CPACKAGEMANAGERHELPER_H__

#include "_Elastos_Droid_Content_Pm_CPackageManagerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::Pm::IPackageManagerHelper;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CPackageManagerHelper)
    , public Singleton
    , public IPackageManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetDataDirForUser(
        /* [in] */ Int32 userId,
        /* [in] */ const String& packageName,
        /* [out] */ String* directory);

    /** {@hide} */
    CARAPI InstallStatusToString(
        /* [in] */ Int32 status,
        /* [in] */ const String& msg,
        /* [out] */ String* str);

    /** {@hide} */
    CARAPI InstallStatusToString(
        /* [in] */ Int32 status,
        /* [out] */ String* str);

    CARAPI InstallStatusToPublicStatus(
        /* [in] */ Int32 status,
        /* [out] */ Int32* value);

    /** {@hide} */
    CARAPI DeleteStatusToString(
        /* [in] */ Int32 status,
        /* [in] */ const String& msg,
        /* [out] */ String* str);

    /** {@hide} */
    CARAPI DeleteStatusToString(
        /* [in] */ Int32 status,
        /* [out] */ String* str);

    /** {@hide} */
    CARAPI DeleteStatusToPublicStatus(
        /* [in] */ Int32 status,
        /* [out] */ Int32* value);
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CSIGNATUREHELPER_H__
