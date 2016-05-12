#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_GESTURE_CEDGEGESTUREPOSITION_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_GESTURE_CEDGEGESTUREPOSITION_H__

#include "_Elastos_Droid_Internal_Utility_Gesture_CEdgeGesturePosition.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Gesture {

CarClass(CEdgeGesturePosition)
    , public Object
    , public IEdgeGesturePosition
{
public:
    CEdgeGesturePosition();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 index,
        /* [in] */ Int32 factor);

    CARAPI GetIndex(
        /* [out] */ Int32* index);

    CARAPI GetFlag(
        /* [out] */ Int32* flag);

    CARAPI GetFactor(
        /* [out] */ Int32* factor);

    static AutoPtr<ArrayOf<IEdgeGesturePosition*> > Values();

public:
    const static AutoPtr<IEdgeGesturePosition> LEFT;
    const static AutoPtr<IEdgeGesturePosition> BOTTOM;
    const static AutoPtr<IEdgeGesturePosition> RIGHT;
    const static AutoPtr<IEdgeGesturePosition> TOP;

private:
    Int32 mIndex;
    Int32 mFlag;
    /**
     * This is 1 when the position is not at the axis (like {@link EdgeGesturePosition.RIGHT} is
     * at {@code Layout.getWidth()} not at {@code 0}).
     */
    Int32 mFactor;
};

} // namespace Gesture
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_GESTURE_CEDGEGESTUREPOSITION_H__
