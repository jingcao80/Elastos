
#ifndef __ELASTOS_DROID_CONTENT_PM_CAPPLICATIONINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CAPPLICATIONINFO_H__

#include "_Elastos_Droid_Content_Pm_CApplicationInfo.h"
#include "elastos/droid/content/pm/ApplicationInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CApplicationInfo)
    , public ApplicationInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CAPPLICATIONINFO_H__
