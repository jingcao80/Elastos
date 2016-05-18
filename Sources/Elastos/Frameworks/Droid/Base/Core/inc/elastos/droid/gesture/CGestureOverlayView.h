#ifndef __ELASTOS_DROID_GESTURE_CGESTUREOVERLAYVIEW_H__
#define __ELASTOS_DROID_GESTURE_CGESTUREOVERLAYVIEW_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/gesture/GestureOverlayView.h"
#include "_Elastos_Droid_Gesture_CGestureOverlayView.h"

namespace Elastos {
namespace Droid {
namespace Gesture {

CarClass(CGestureOverlayView)
    , public GestureOverlayView
{
public:
    CAR_OBJECT_DECL()

    CGestureOverlayView();

    virtual ~CGestureOverlayView();
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GESTURE_CGESTUREOVERLAYVIEW_H__
