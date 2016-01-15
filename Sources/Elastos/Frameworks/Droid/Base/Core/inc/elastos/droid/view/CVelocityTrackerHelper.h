#ifndef __ELASTOS_DROID_VIEW_CVELOCITYTRACKERHELPER_H__
#define __ELASTOS_DROID_VIEW_CVELOCITYTRACKERHELPER_H__

#include "_Elastos_Droid_View_CVelocityTrackerHelper.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CVelocityTrackerHelper)
{
public:
    CARAPI Obtain(
        /* [out] */ IVelocityTracker** velocity);

    CARAPI Obtain(
        /* [in] */ const String& strategy,
        /* [out] */ IVelocityTracker** velocity);
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CVELOCITYTRACKERHELPER_H__
