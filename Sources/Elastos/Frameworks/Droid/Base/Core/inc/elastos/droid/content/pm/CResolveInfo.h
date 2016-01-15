
#ifndef __ELASTOS_DROID_CONTENT_PM_CRESOLVEINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CRESOLVEINFO_H__

#include "_Elastos_Droid_Content_Pm_CResolveInfo.h"
#include "elastos/droid/content/pm/ResolveInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CResolveInfo)
    , public ResolveInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CRESOLVEINFO_H__
