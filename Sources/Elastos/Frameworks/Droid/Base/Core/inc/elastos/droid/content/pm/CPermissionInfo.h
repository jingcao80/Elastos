
#ifndef __ELASTOS_DROID_CONTENT_PM_CPERMISSIONINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CPERMISSIONINFO_H__

#include "_Elastos_Droid_Content_Pm_CPermissionInfo.h"
#include "PermissionInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CPermissionInfo)
    , public PermissionInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CPERMISSIONINFO_H__
