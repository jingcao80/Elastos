#ifndef __ELASTOS_DROID_SERVER_WM_CIRCULARDISPLAYMASK_H__
#define __ELASTOS_DROID_SERVER_WM_CIRCULARDISPLAYMASK_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::View::ISurfaceSession;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::ISurfaceControl;
using Elastos::Droid::View::IDisplay;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class CircularDisplayMask : public Object
{
public:
    CircularDisplayMask(
        /* [in] */ IDisplay* display,
        /* [in] */ ISurfaceSession* session,
        /* [in] */ Int32 zOrder,
        /* [in] */ Int32 screenOffset);

    // Note: caller responsible for being inside
    // Surface.openTransaction() / closeTransaction()
    CARAPI_(void) SetVisibility(
        /* [in] */ Boolean on);

    CARAPI_(void) PositionSurface(
        /* [in] */ Int32 dw,
        /* [in] */ Int32 dh,
        /* [in] */ Int32 rotation);

private:
    CARAPI_(void) DrawIfNeeded();

private:
    static const String TAG;

    static cosnt Int32 STROKE_WIDTH = 2;
    // size of the chin
    Int32 mScreenOffset;
    // Display dimensions
    AutoPtr<IPoint> mScreenSize;

    AutoPtr<ISurfaceControl> mSurfaceControl;
    AutoPtr<ISurface> mSurface;
    Int32 mLastDW;
    Int32 mLastDH;
    Boolean mDrawNeeded;
    AutoPtr<IPaint> mPaint;
    Int32 mRotation;
    Boolean mVisible;
    Boolean mDimensionsUnequal;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_CIRCULARDISPLAYMASK_H__
