#ifndef __ELASTOS_DROID_GESTURE_CFADEOUTRUNNABLE_H__
#define __ELASTOS_DROID_GESTURE_CFADEOUTRUNNABLE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/gesture/GestureOverlayView.h"
#include "_Elastos_Droid_Gesture_CFadeOutRunnable.h"

using Elastos::Droid::Gesture::CFadeOutRunnable;

namespace Elastos {
namespace Droid {
namespace Gesture {

CarClass(CFadeOutRunnable)
    , public GestureOverlayView::FadeOutRunnable
{
public:
    CAR_OBJECT_DECL();

    CFadeOutRunnable();

    virtual ~CFadeOutRunnable();

};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif
