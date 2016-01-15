
#ifndef __ELASTOS_DROID_CONTENT_PM_CPERMISSIONGROUPINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CPERMISSIONGROUPINFO_H__

#include "_Elastos_Droid_Content_Pm_CPermissionGroupInfo.h"
#include "PermissionGroupInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CPermissionGroupInfo)
    , public PermissionGroupInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CPERMISSIONGROUPINFO_H__
