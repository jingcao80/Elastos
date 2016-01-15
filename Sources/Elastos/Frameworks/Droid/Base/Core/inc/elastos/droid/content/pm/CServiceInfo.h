
#ifndef __ELASTOS_DROID_CONTENT_PM_CSERVICEINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CSERVICEINFO_H__

#include "_Elastos_Droid_Content_Pm_CServiceInfo.h"
#include "ServiceInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Information you can retrieve about a particular application
 * service. This corresponds to information collected from the
 * AndroidManifest.xml's &lt;service&gt; tags.
 */
CarClass(CServiceInfo)
    , public ServiceInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CSERVICEINFO_H__