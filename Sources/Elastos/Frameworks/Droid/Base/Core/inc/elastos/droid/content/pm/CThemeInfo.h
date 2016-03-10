#ifndef __ELASTOS_DROID_CONTENT_PM_CTHEMEINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CTHEMEINFO_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Content_Pm_CThemeInfo.h"
#include "elastos/droid/content/pm/BaseThemeInfo.h"

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CThemeInfo)
    , public BaseThemeInfo
    , public IThemeInfo
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IBundle* bundle);
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CThemeInfo_H__
