#ifndef __ELASTOS_DROID_SERVER_WM_DIMLAYER_H__
#define __ELASTOS_DROID_SERVER_WM_DIMLAYER_H__

#include "wm/DisplayContent.h"
#include "wm/TaskStack.h"

using Elastos::IO::IPrintWriter;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::ISurfaceControl;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class DimLayer : public Object
{
public:
    DimLayer(
        /* [in] */ CWindowManagerService* service,
        /* [in] */ TaskStack* taskStack,
        /* [in] */ DisplayContent* displayContent);

    /** Return true if dim layer is showing */
    CARAPI_(Boolean) IsDimming();

    /** Return true if in a transition period */
    CARAPI_(Boolean) IsAnimating();

    CARAPI_(Float) GetTargetAlpha();

    CARAPI_(void) SetLayer(
        /* [in] */ Int32 layer);

    CARAPI_(Int32) GetLayer();

    /**
     * @param layer The new layer value.
     * @param inTransaction Whether the call is made within a surface transaction.
     */
    CARAPI_(void) AdjustSurface(
        /* [in] */ Int32 layer,
        /* [in] */ Boolean inTransaction);

    // Assumes that surface transactions are currently closed.
    CARAPI_(void) SetBounds(
        /* [in] */ IRect* bounds);

    /** Jump to the end of the animation.
     * NOTE: Must be called with Surface transaction open. */
    CARAPI_(void) Show();

    /**
     * Begin an animation to a new dim value.
     * NOTE: Must be called with Surface transaction open.
     *
     * @param layer The layer to set the surface to.
     * @param alpha The dim value to end at.
     * @param duration How long to take to get there in milliseconds.
     */
    CARAPI_(void) Show(
        /* [in] */ Int32 layer,
        /* [in] */ Float alpha,
        /* [in] */ Int64 duration);

    /** Immediate hide.
     * NOTE: Must be called with Surface transaction open. */
    CARAPI_(void) Hide();

    /**
     * Gradually fade to transparent.
     * NOTE: Must be called with Surface transaction open.
     *
     * @param duration Time to fade in milliseconds.
     */
    CARAPI_(void) Hide(
        /* [in] */ Int64 duration);

    /**
     * Advance the dimming per the last #show(int, float, long) call.
     * NOTE: Must be called with Surface transaction open.
     *
     * @return True if animation is still required after this step.
     */
    CARAPI_(Boolean) StepAnimation();

    /** Cleanup */
    CARAPI_(void) DestroySurface();

    CARAPI_(void) PrintTo(
        /* [in] */ const String& prefix,
        /* [in] */ IPrintWriter* pw);

private:
    CARAPI_(void) SetAlpha(
        /* [in] */ Float alpha);

    /**
     * @param duration The time to test.
     * @return True if the duration would lead to an earlier end to the current animation.
     */
    CARAPI_(Boolean) DurationEndsEarlier(
        /* [in] */ Int64 duration);

public:
    /** Reference to the owner of this object. */
    AutoPtr<DisplayContent> mDisplayContent;

    /** Actual surface that dims */
    AutoPtr<ISurfaceControl> mDimSurface;

    /** Last value passed to mDimSurface.setAlpha() */
    Float mAlpha;

    /** Last value passed to mDimSurface.setLayer() */
    Int32 mLayer;

    /** Next values to pass to mDimSurface.setPosition() and mDimSurface.setSize() */
    AutoPtr<IRect> mBounds;

    /** Last values passed to mDimSurface.setPosition() and mDimSurface.setSize() */
    AutoPtr<IRect> mLastBounds;

    /** Value of mAlpha when beginning transition to mTargetAlpha */
    Float mStartAlpha;

    /** Final value of mAlpha following transition */
    Float mTargetAlpha;

    /** Time in units of SystemClock.uptimeMillis() at which the current transition started */
    Int64 mStartTime;

    /** Time in milliseconds to take to transition from mStartAlpha to mTargetAlpha */
    Int64 mDuration;

    /** Owning stack */
    TaskStack* mStack;

private:
    static const String TAG;
    static const Boolean DEBUG = FALSE;

    /** True after mDimSurface.show() has been called, false after mDimSurface.hide(). */
    Boolean mShowing;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_DIMLAYER_H__
