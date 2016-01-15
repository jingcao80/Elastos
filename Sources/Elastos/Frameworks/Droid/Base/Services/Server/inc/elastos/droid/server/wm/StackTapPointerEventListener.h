#ifndef __ELASTOS_DROID_SERVER_WM_STACKTAPPOINTEREVENTLISTENER_H__
#define __ELASTOS_DROID_SERVER_WM_STACKTAPPOINTEREVENTLISTENER_H__

#include "wm/CWindowManagerService.h"

using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::View::IPointerEventListener;
using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class StackTapPointerEventListener
    : public Object
    , public IPointerEventListener
{
public:
    StackTapPointerEventListener(
        /* [in] */ CWindowManagerService* service,
        /* [in] */ DisplayContent* displayContent);

    CARAPI OnPointerEvent(
        /* [in] */ IMotionEvent* motionEvent);

public:
    static const Int32 TAP_TIMEOUT_MSEC = 300;
    static const Float TAP_MOTION_SLOP_INCHES = 0.125f;

    Int32 mMotionSlop;
    Float mDownX;
    Float mDownY;
    Int32 mPointerId;
    AutoPtr<IRegion> mTouchExcludeRegion;
    AutoPtr<CWindowManagerService> mService;
    AutoPtr<DisplayContent> mDisplayContent;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_STACKTAPPOINTEREVENTLISTENER_H__
