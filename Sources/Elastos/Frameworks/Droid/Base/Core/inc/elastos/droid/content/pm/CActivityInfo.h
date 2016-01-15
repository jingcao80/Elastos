#ifndef __ELASTOS_DROID_CONTENT_PM_CACTIVITYINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CACTIVITYINFO_H__

#include "_Elastos_Droid_Content_Pm_CActivityInfo.h"
#include "elastos/droid/content/pm/ActivityInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CActivityInfo)
    , public ActivityInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CACTIVITYINFO_H__
