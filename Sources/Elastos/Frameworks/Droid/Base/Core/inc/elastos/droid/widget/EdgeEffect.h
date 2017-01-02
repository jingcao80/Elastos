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

#ifndef __ELASTOS_DROID_WIDGET_EDGEEFFECT_H__
#define __ELASTOS_DROID_WIDGET_EDGEEFFECT_H__

#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::Widget::IEdgeEffect;

namespace Elastos {
namespace Droid {
namespace Widget {

class EdgeEffect
    : public Object
    , public IEdgeEffect
{
public:
    CAR_INTERFACE_DECL()

    EdgeEffect();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
      * Set the size of this edge effect in pixels.
      *
      * @param width Effect width in pixels
      * @param height Effect height in pixels
      */
    virtual CARAPI SetSize(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
      * Reports if this EdgeEffect's animation is finished. If this method returns false
      * after a call to {@link #draw(Canvas)} the host widget should schedule another
      * drawing pass to continue the animation.
      *
      * @return true if animation is finished, false if drawing should continue on the next frame.
      */
    virtual CARAPI IsFinished(
        /* [out] */ Boolean* result);

    /**
      * Immediately finish the current animation.
      * After this call {@link #isFinished()} will return true.
      */
    virtual CARAPI Finish();

    /**
      * A view should call this when content is pulled away from an edge by the user.
      * This will update the state of the current visual effect and its associated animation.
      * The host view should always {@link android.view.View#invalidate()} after this
      * and draw the results accordingly.
      *
      * <p>Views using EdgeEffect should favor {@link #onPull(float, float)} when the displacement
      * of the pull point is known.</p>
      *
      * @param deltaDistance Change in distance since the last call. Values may be 0 (no change) to
      *                      1.f (full length of the view) or negative values to express change
      *                      back toward the edge reached to initiate the effect.
      */
    virtual CARAPI OnPull(
        /* [in] */ Float deltaDistance);

    /**
      * A view should call this when content is pulled away from an edge by the user.
      * This will update the state of the current visual effect and its associated animation.
      * The host view should always {@link android.view.View#invalidate()} after this
      * and draw the results accordingly.
      *
      * @param deltaDistance Change in distance since the last call. Values may be 0 (no change) to
      *                      1.f (full length of the view) or negative values to express change
      *                      back toward the edge reached to initiate the effect.
      * @param displacement The displacement from the starting side of the effect of the point
      *                     initiating the pull. In the case of touch this is the finger position.
      *                     Values may be from 0-1.
      */
    virtual CARAPI OnPull(
        /* [in] */ Float deltaDistance,
        /* [in] */ Float displacement);

    /**
      * Call when the object is released after being pulled.
      * This will begin the "decay" phase of the effect. After calling this method
      * the host view should {@link android.view.View#invalidate()} and thereby
      * draw the results accordingly.
      */
    virtual CARAPI OnRelease();

    /**
      * Call when the effect absorbs an impact at the given velocity.
      * Used when a fling reaches the scroll boundary.
      *
      * <p>When using a {@link android.widget.Scroller} or {@link android.widget.OverScroller},
      * the method <code>getCurrVelocity</code> will provide a reasonable approximation
      * to use here.</p>
      *
      * @param velocity Velocity at impact in pixels per second.
      */
    virtual CARAPI OnAbsorb(
        /* [in] */ Int32 velocity);

    /**
      * Set the color of this edge effect in argb.
      *
      * @param color Color in argb
      */
    virtual CARAPI SetColor(
        /* [in] */ Int32 color);

    /**
      * Return the color of this edge effect in argb.
      * @return The color of this edge effect in argb
      */
    virtual CARAPI GetColor(
        /* [out] */ Int32* result);

    /**
      * Draw into the provided canvas. Assumes that the canvas has been rotated
      * accordingly and the size has been set. The effect will be drawn the full
      * width of X=0 to X=width, beginning from Y=0 and extending to some factor <
      * 1.f of height.
      *
      * @param canvas Canvas to draw into
      * @return true if drawing should continue beyond this frame to continue the
      *         animation
      */
    virtual CARAPI Draw(
        /* [in] */ ICanvas* canvas,
        /* [out] */ Boolean* result);

    /**
      * Return the maximum height that the edge effect will be drawn at given the original
      * {@link #setSize(int, int) input size}.
      * @return The maximum height of the edge effect
      */
    virtual CARAPI GetMaxHeight(
        /* [out] */ Int32* result);

private:
    CARAPI_(void) Update();

private:
    // @SuppressWarnings("UnusedDeclaration")
    static const String TAG;
    static const Int32 RECEDE_TIME = 600;
    static const Int32 PULL_TIME = 167;
    static const Int32 PULL_DECAY_TIME = 2000;
    static const Float MAX_ALPHA;
    static const Float MAX_GLOW_SCALE;
    static const Float PULL_GLOW_BEGIN;
    static const Int32 MIN_VELOCITY = 100;
    static const Int32 MAX_VELOCITY = 10000;
    static const Float EPSILON;
    static const Double ANGLE;
    static const Float SIN;
    static const Float COS;
    Float mGlowAlpha;
    Float mGlowScaleY;
    Float mGlowAlphaStart;
    Float mGlowAlphaFinish;
    Float mGlowScaleYStart;
    Float mGlowScaleYFinish;
    Int64 mStartTime;
    Float mDuration;
    /*const*/ AutoPtr<IInterpolator> mInterpolator;
    static const Int32 STATE_IDLE = 0;
    static const Int32 STATE_PULL = 1;
    static const Int32 STATE_ABSORB = 2;
    static const Int32 STATE_RECEDE = 3;
    static const Int32 STATE_PULL_DECAY = 4;
    static const Float PULL_DISTANCE_ALPHA_GLOW_FACTOR;
    static const Int32 VELOCITY_GLOW_FACTOR = 6;
    Int32 mState;
    Float mPullDistance;
    /*const*/ AutoPtr<IRect> mBounds;
    /*const*/ AutoPtr<IPaint> mPaint;
    Float mRadius;
    Float mBaseGlowScale;
    Float mDisplacement;
    Float mTargetDisplacement;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_EDGEEFFECT_H__


