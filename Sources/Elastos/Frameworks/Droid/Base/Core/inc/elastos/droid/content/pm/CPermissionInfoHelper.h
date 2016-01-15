
#ifndef __ELASTOS_DROID_CONTENT_PM_CPERMISSIONINFOHELPER_H__
#define __ELASTOS_DROID_CONTENT_PM_CPERMISSIONINFOHELPER_H__

#include "_Elastos_Droid_Content_Pm_CPermissionInfoHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CPermissionInfoHelper)
    , public Singleton
    , public IPermissionInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /** @hide */
    CARAPI FixProtectionLevel(
        /* [in] */ Int32 level,
        /* [out] */ Int32* retLevel);

    /** @hide */
    CARAPI ProtectionToString(
        /* [in] */ Int32 level,
        /* [out] */ String* protLevel);
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CPERMISSIONINFOHELPER_H__
