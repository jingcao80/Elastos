//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SERVER_WM_BlurLayer_H__
#define __ELASTOS_DROID_SERVER_WM_BlurLayer_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/wm/DisplayContent.h"

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::ISurfaceControl;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class CWindowManagerService;
class TaskStack;

class BlurLayer
    : public Object
{
public:
    BlurLayer(
        /* [in] */ CWindowManagerService* service,
        /* [in] */ TaskStack* stack,
        /* [in] */ DisplayContent* displayContent);

    /** Return true if blur layer is showing */
    CARAPI_(Boolean) IsBlurring();

    /** Return true if in a transition period */
    CARAPI_(Boolean) IsAnimating();

    CARAPI_(Float) GetTargetBlur();

    CARAPI_(void) SetLayer(
        /* [in] */ Int32 layer);

    CARAPI_(Int32) GetLayer();

    CARAPI_(void) AdjustSurface(
        /* [in] */ Int32 layer,
        /* [in] */ Boolean inTransaction);

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
     * @param blur The dim value to end at.
     * @param duration How long to take to get there in milliseconds.
     */
    CARAPI_(void) Show(
        /* [in] */ Int32 layer,
        /* [in] */ Float blur,
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
    CARAPI_(void) SetBlur(
        /* [in] */ Float blur);

    /**
     * @param duration The time to test.
     * @return True if the duration would lead to an earlier end to the current animation.
     */
    CARAPI_(Boolean) DurationEndsEarlier(
        /* [in] */ Int64 duration);

public:
    /** Reference to the owner of this object. */
    AutoPtr<DisplayContent> mDisplayContent;

    /** Actual surface that blurs */
    AutoPtr<ISurfaceControl> mBlurSurface;

    /** Last value passed to mBlurSurface.setBlur() */
    Float mBlur;

    /** Last value passed to mBlurSurface.setLayer() */
    Int32 mLayer;

    /** Next values to pass to mBlurSurface.setPosition() and mBlurSurface.setSize() */
    AutoPtr<IRect> mBounds;

    /** Last values passed to mBlurSurface.setPosition() and mBlurSurface.setSize() */
    AutoPtr<IRect> mLastBounds;

    /** Value of mBlur when beginning transition to mTargetBlur */
    Float mStartBlur;

    /** Final value of mBlur following transition */
    Float mTargetBlur;

    /** Time in units of SystemClock.uptimeMillis() at which the current transition started */
    Int64 mStartTime;

    /** Time in milliseconds to take to transition from mStartBlur to mTargetBlur */
    Int64 mDuration;

    /** Owning stack */
    TaskStack* mStack;

private:
    static const String TAG;
    static const Boolean DEBUG = FALSE;

    /** True after mBlurSurface.show() has been called, false after mBlurSurface.hide(). */
    Boolean mShowing;

};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_BlurLayer_H__
