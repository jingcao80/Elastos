#ifndef __ELASTOS_DROID_GESTURE_CINSTANCEHELPER_H__
#define __ELASTOS_DROID_GESTURE_CINSTANCEHELPER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_Gesture_CInstanceHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Gesture {

CarClass(CInstanceHelper)
    , public Singleton
    , public IInstanceHelper
{
public:
    CAR_SINGLETON_DECL();

    CAR_INTERFACE_DECL()

    CARAPI CreateInstance(
        /* [in] */ Int32 sequenceType,
        /* [in] */ Int32 orientationType,
        /* [in] */ IGesture *gesture,
        /* [in] */ const String& label,
        /* [out] */ IInstance **instance);

    CARAPI SpatialSampler(
        /* [in] */ IGesture *gesture,
        /* [out, callee] */ ArrayOf<Float> **sampler);

    CARAPI TemporalSampler(
        /* [in] */ Int32 orientationType,
        /* [in] */ IGesture *gesture,
        /* [out, callee] */ ArrayOf<Float> **sampler);
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GESTURE_CINSTANCEHELPER_H__
