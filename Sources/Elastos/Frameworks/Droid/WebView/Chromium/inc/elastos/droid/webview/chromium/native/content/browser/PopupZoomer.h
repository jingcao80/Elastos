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

// wuweizuo automatic build .h file from .java file.
// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_POPUPZOOMER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_POPUPZOOMER_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/view/GestureDetector.h"
#include "elastos/droid/view/View.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Graphics::IPointF;
using Elastos::Droid::Graphics::IPorterDuffXfermode;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::IGestureDetector;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::GestureDetector;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::Animation::IOvershootInterpolator;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
  * PopupZoomer is used to show the on-demand link zooming popup. It handles manipulation of the
  * canvas and touch events to display the on-demand zoom magnifier.
  */
class PopupZoomer
    : public Elastos::Droid::View::View
{
public:
    /**
      * Interface to be implemented to listen for touch events inside the zoomed area.
      * The MotionEvent coordinates correspond to original unzoomed view.
      */
    class OnTapListener : public Object
    {
    public:
        virtual CARAPI_(Boolean) OnSingleTap(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event) = 0;

        virtual CARAPI_(Boolean) OnLongPress(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event) = 0;
    };

    /**
      * Interface to be implemented to add and remove PopupZoomer to/from the view hierarchy.
      */
    class OnVisibilityChangedListener : public Object
    {
    public:
        virtual CARAPI OnPopupZoomerShown(
            /* [in] */ PopupZoomer* zoomer) = 0;

        virtual CARAPI OnPopupZoomerHidden(
            /* [in] */ PopupZoomer* zoomer) = 0;
    };

    class InnerGestureDetectorSimpleOnGestureListener
        : public GestureDetector::SimpleOnGestureListener
    {
    public:
        InnerGestureDetectorSimpleOnGestureListener(
            /* [in] */ PopupZoomer* owner);

        // @Override
        CARAPI OnScroll(
            /* [in] */ IMotionEvent* e1,
            /* [in] */ IMotionEvent* e2,
            /* [in] */ Float distanceX,
            /* [in] */ Float distanceY,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnSingleTapUp(
            /* [in] */ IMotionEvent* e,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnLongPress(
            /* [in] */ IMotionEvent* e);

    private:
        CARAPI_(Boolean) HandleTapOrPress(
            /* [in] */ IMotionEvent* e,
            /* [in] */ Boolean isLongPress);

    private:
        PopupZoomer* mOwner;
    };

private:
    class ReverseInterpolator
        : public Object
        , public IInterpolator
    {
    public:
        ReverseInterpolator(
            /* [in] */ IInterpolator* i);

        // @Override
        CARAPI GetInterpolation(
            /* [in] */ Float input,
            /* [out] */ Float* interpolation);

    private:
        /*const*/ AutoPtr<IInterpolator> mInterpolator;
    };

public:
    /**
      * Creates Popupzoomer.
      * @param context Context to be used.
      * @param overlayRadiusDimensoinResId Resource to be used to get overlay corner radius.
      */
    PopupZoomer(
        /* [in] */ IContext* context);

    /**
      * Sets the OnTapListener.
      */
    virtual CARAPI SetOnTapListener(
        /* [in] */ OnTapListener* listener);

    /**
      * Sets the OnVisibilityChangedListener.
      */
    virtual CARAPI SetOnVisibilityChangedListener(
        /* [in] */ OnVisibilityChangedListener* listener);

    /**
      * Sets the bitmap to be used for the zoomed view.
      */
    virtual CARAPI SetBitmap(
        /* [in] */ IBitmap* bitmap);

    /**
      * Returns true if the view is currently being shown (or is animating).
      */
    virtual CARAPI_(Boolean) IsShowing();

    /**
      * Sets the last touch point (on the unzoomed view).
      */
    virtual CARAPI SetLastTouch(
        /* [in] */ Float x,
        /* [in] */ Float y);

    /**
      * Show the PopupZoomer view with given target bounds.
      */
    virtual CARAPI Show(
        /* [in] */ IRect* rect);

    /**
      * Hide the PopupZoomer view.
      * @param animation true if hide with animation.
      */
    virtual CARAPI Hide(
        /* [in] */ Boolean animation);

    // @Override
    CARAPI_(Boolean) OnTouchEvent(
        /* [in] */ IMotionEvent* event);

protected:
    /*
      * Tests override it as the PopupZoomer is never attached to the view hierarchy.
      */
    virtual CARAPI_(Boolean) AcceptZeroSizeView();

    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    static CARAPI_(Float) GetOverlayCornerRadius(
        /* [in] */ IContext* context);

    /**
      * Gets the drawable that should be used to frame the zooming popup, loading
      * it from the resource bundle if not already cached.
      */
    static CARAPI_(AutoPtr<IDrawable>) GetOverlayDrawable(
        /* [in] */ IContext* context);

    static CARAPI_(Float) Constrain(
        /* [in] */ Float amount,
        /* [in] */ Float low,
        /* [in] */ Float high);

    static CARAPI_(Int32) Constrain(
        /* [in] */ Int32 amount,
        /* [in] */ Int32 low,
        /* [in] */ Int32 high);

    CARAPI Scroll(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI StartAnimation(
        /* [in] */ Boolean show);

    CARAPI HideImmediately();

    CARAPI SetTargetBounds(
        /* [in] */ IRect* rect);

    CARAPI InitDimensions();

    /**
      * Converts the coordinates to a point on the original un-zoomed view.
      */
    CARAPI_(AutoPtr<IPointF>) ConvertTouchPoint(
        /* [in] */ Float x,
        /* [in] */ Float y);

    /**
      * Returns true if the point is inside the final drawable area for this popup zoomer.
      */
    CARAPI_(Boolean) IsTouchOutsideArea(
        /* [in] */ Float x,
        /* [in] */ Float y);

private:
    static const String LOGTAG;
    // The padding between the edges of the view and the popup. Note that there is a mirror
    // constant in content/renderer/render_view_impl.cc which should be kept in sync if
    // this is changed.
    static const Int32 ZOOM_BOUNDS_MARGIN = 25;
    // Time it takes for the animation to finish in ms.
    static const Int64 ANIMATION_DURATION = 300;
    AutoPtr<OnTapListener> mOnTapListener;
    AutoPtr<OnVisibilityChangedListener> mOnVisibilityChangedListener;
    // Cached drawable used to frame the zooming popup.
    // TODO(tonyg): This should be marked purgeable so that if the system wants to recover this
    // memory, we can just reload it from the resource ID next time it is needed.
    // See android.graphics.BitmapFactory.Options#inPurgeable
    static AutoPtr<IDrawable> sOverlayDrawable;
    // The padding used for drawing the overlay around the content, instead of directly above it.
    static AutoPtr<IRect> sOverlayPadding;
    // The radius of the overlay bubble, used for rounding the bitmap to draw underneath it.
    static Float sOverlayCornerRadius;
    /*const*/ AutoPtr<IInterpolator> mShowInterpolator;
    /*const*/ AutoPtr<IInterpolator> mHideInterpolator;
    Boolean mAnimating;
    Boolean mShowing;
    Int64 mAnimationStartTime;
    // The time that was left for the outwards animation to finish.
    // This is used in the case that the zoomer is cancelled while it is still animating outwards,
    // to avoid having it jump to full size then animate closed.
    Int64 mTimeLeft;
    // initDimensions() needs to be called in onDraw().
    Boolean mNeedsToInitDimensions;
    // Available view area after accounting for ZOOM_BOUNDS_MARGIN.
    AutoPtr<IRectF> mViewClipRect;
    // The target rect to be zoomed.
    AutoPtr<IRect> mTargetBounds;
    // The bitmap to hold the zoomed view.
    AutoPtr<IBitmap> mZoomedBitmap;
    // How far to shift the canvas after all zooming is done, to keep it inside the bounds of the
    // view (including margin).
    Float mShiftX;
    Float mShiftY;
    // The magnification factor of the popup. It is recomputed once we have mTargetBounds and
    // mZoomedBitmap.
    Float mScale;
    // The bounds representing the actual zoomed popup.
    AutoPtr<IRectF> mClipRect;
    // The extrusion values are how far the zoomed area (mClipRect) extends from the touch point.
    // These values to used to animate the popup.
    Float mLeftExtrusion;
    Float mTopExtrusion;
    Float mRightExtrusion;
    Float mBottomExtrusion;
    // The last touch point, where the animation will start from.
    /*const*/ AutoPtr<IPointF> mTouch;
    // Since we sometimes overflow the bounds of the mViewClipRect, we need to allow scrolling.
    // Current scroll position.
    Float mPopupScrollX;
    Float mPopupScrollY;
    // Scroll bounds.
    Float mMinScrollX;
    Float mMaxScrollX;
    Float mMinScrollY;
    Float mMaxScrollY;
    AutoPtr<IGestureDetector> mGestureDetector;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_POPUPZOOMER_H_
