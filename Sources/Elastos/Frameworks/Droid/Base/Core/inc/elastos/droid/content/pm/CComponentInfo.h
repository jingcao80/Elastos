
#ifndef __ELASTOS_DROID_CONTENT_PM_CCOMPONENTINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CCOMPONENTINFO_H__

#include "_Elastos_Droid_Content_Pm_CComponentInfo.h"
#include "ComponentInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Base class containing information common to all application components
 * ({@link ActivityInfo}, {@link ServiceInfo}).  This class is not intended
 * to be used by itself; it is simply here to share common definitions
 * between all application components.  As such, it does not itself
 * implement Parcelable, but does provide convenience methods to assist
 * in the implementation of Parcelable in subclasses.
 */
CarClass(CComponentInfo)
    , public ComponentInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CCOMPONENTINFO_H__
