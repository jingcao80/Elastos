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

private:
    static CARAPI_(Boolean) InitStatic();

public:
    Int32 INDEX;
    Int32 FLAG;
    /**
     * This is 1 when the position is not at the axis (like {@link EdgeGesturePosition.RIGHT} is
     * at {@code Layout.getWidth()} not at {@code 0}).
     */
    Int32 FACTOR;

    const static AutoPtr<CEdgeGesturePosition> LEFT;
    const static AutoPtr<CEdgeGesturePosition> BOTTOM;
    const static AutoPtr<CEdgeGesturePosition> RIGHT;
    const static AutoPtr<CEdgeGesturePosition> TOP;

private:
    static Boolean sInit;
};

} // namespace Gesture
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_GESTURE_CEDGEGESTUREPOSITION_H__
