
#ifndef __ELASTOS_DROID_CONTENT_PM_CPROVIDERINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CPROVIDERINFO_H__

#include "_Elastos_Droid_Content_Pm_CProviderInfo.h"
#include "elastos/droid/content/pm/ProviderInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Holds information about a specific
 * {@link android.content.ContentProvider content provider}. This is returned by
 * {@link android.content.pm.PackageManager#resolveContentProvider(java.lang.String, int)
 * PackageManager.resolveContentProvider()}.
 */
CarClass(CProviderInfo)
    , public ProviderInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CPROVIDERINFO_H__
