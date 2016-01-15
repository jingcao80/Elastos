
#ifndef __ELASTOS_DROID_CONTENT_PM_CPACKAGEITEMINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CPACKAGEITEMINFO_H__

#include "_Elastos_Droid_Content_Pm_CPackageItemInfo.h"
#include "elastos/droid/content/pm/PackageItemInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CPackageItemInfo)
    , public PackageItemInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CPACKAGEITEMINFO_H__
