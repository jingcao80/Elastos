#ifndef __ELASTOS_DROID_SERVER_WM_FOCUSEDSTACKFRAME_H__
#define __ELASTOS_DROID_SERVER_WM_FOCUSEDSTACKFRAME_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/server/wm/TaskStack.h"

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::ISurfaceControl;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::ISurfaceSession;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class FocusedStackFrame : public Object
{
public:
    FocusedStackFrame(
        /* [in] */ IDisplay* display,
        /* [in] */ ISurfaceSession* session);

    // Note: caller responsible for being inside
    // Surface.openTransaction() / closeTransaction()
    CARAPI_(void) SetVisibility(
        /* [in] */ Boolean on);

    CARAPI_(void) SetBounds(
        /* [in] */ TaskStack* stack);

    CARAPI_(void) SetLayer(
        /* [in] */ Int32 layer);

private:
    CARAPI_(void) Draw(
        /* [in] */ IRect* bounds,
        /* [in] */ Int32 color);

    CARAPI_(void) PositionSurface(
        /* [in] */ IRect* bounds);

private:
    static const String TAG;

    static const Int32 THICKNESS = 10;
    static const Float ALPHA = 0.3f;

    AutoPtr<ISurfaceControl> mSurfaceControl;
    AutoPtr<ISurface> mSurface;
    AutoPtr<IRect> mLastBounds;
    AutoPtr<IRect> mBounds;
    AutoPtr<IRect> mTmpDrawRect;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_FOCUSEDSTACKFRAME_H__
