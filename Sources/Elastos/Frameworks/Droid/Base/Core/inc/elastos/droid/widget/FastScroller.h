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

#ifndef __ELASTOS_DROID_WIDGET_FASTSCROLLER_H__
#define __ELASTOS_DROID_WIDGET_FASTSCROLLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/utility/Int32Property.h"

#include <elastos/core/Object.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Runnable;
using Elastos::Droid::Utility::Int32Property;
using Elastos::Droid::Utility::IProperty;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupOverlay;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * Helper class for AbsListView to draw and control the Fast Scroll thumb
 */
class FastScroller : public Object
{
    friend class AbsListView;
private:
    class MyRunnable : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ FastScroller* host);

        CARAPI Run();

    /**
     * Used to delay hiding fast scroll decorations.
     */
    private:
        FastScroller* mHost;
    };

    /**
     * Used to effect a transition from primary to secondary text.
     */
    class MyAnimatorListenerAdapter : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter(
            /* [in] */ FastScroller* host);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        FastScroller* mHost;
    };

public:
    class Leftroperty : public Int32Property
    {
    public:
        Leftroperty(
            /* [in] */ const String& name);

        CARAPI Get(
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** rst);

        CARAPI SetValue(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 value);
    };

    class RightProperty : public Int32Property
    {
    public:
        RightProperty(
            /* [in] */ const String& name);

        CARAPI Get(
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** rst);

        CARAPI SetValue(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 value);
    };

    class TopProperty : public Int32Property
    {
    public:
        TopProperty(
            /* [in] */ const String& name);

        CARAPI Get(
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** rst);

        CARAPI SetValue(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 value);
    };

    class BottomProperty : public Int32Property
    {
    public:
        BottomProperty(
            /* [in] */ const String& name);

        CARAPI Get(
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** rst);

        CARAPI SetValue(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 value);
    };

public:
    FastScroller();

    CARAPI constructor(
        /* [in] */ IAbsListView* listView,
        /* [in] */ Int32 styleResId);

    CARAPI_(void) SetStyle(
        /* [in] */ Int32 resId);

    /**
     * Removes this FastScroller overlay from the host view.
     */
    CARAPI_(void) Remove();

    /**
     * @param enabled Whether the fast scroll thumb is enabled.
     */
    CARAPI_(void) SetEnabled(
        /* [in] */ Boolean enabled);

    /**
     * @return Whether the fast scroll thumb is enabled.
     */
    CARAPI_(Boolean) IsEnabled();

    /**
     * @param alwaysShow Whether the fast scroll thumb should always be shown
     */
    CARAPI_(void) SetAlwaysShow(
        /* [in] */ Boolean alwaysShow);

    /**
     * @return Whether the fast scroll thumb will always be shown
     * @see #setAlwaysShow(boolean)
     */
    CARAPI_(Boolean) IsAlwaysShowEnabled();

    CARAPI_(void) SetScrollBarStyle(
        /* [in] */ Int32 style);

    /**
     * Immediately transitions the fast scroller decorations to a hidden state.
     */
    CARAPI_(void) Stop();

    CARAPI_(void) SetScrollbarPosition(
        /* [in] */ Int32 position);

    CARAPI_(Int32) GetWidth();

    CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    CARAPI_(void) OnItemCountChanged(
        /* [in] */ Int32 childCount,
        /* [in] */ Int32 itemCount);

    /**
     * Measures and layouts the scrollbar and decorations.
     */
    CARAPI_(void) UpdateLayout();

    CARAPI_(void) OnScroll(
        /* [in] */ Int32 firstVisibleItem,
        /* [in] */ Int32 visibleItemCount,
        /* [in] */ Int32 totalItemCount);

    CARAPI_(void) OnSectionsChanged();

    CARAPI_(Boolean) OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(Boolean) OnInterceptHoverEvent(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(Boolean) OnTouchEvent(
        /* [in] */ IMotionEvent* me);

private:
    CARAPI_(void) UpdateAppearance();

    /**
     * Called when one of the variables affecting enabled state changes.
     *
     * @param peekIfEnabled whether the thumb should peek, if enabled
     */
    CARAPI_(void) OnStateDependencyChanged(
        /* [in] */ Boolean peekIfEnabled);

    CARAPI_(void) UpdateLongList(
        /* [in] */ Int32 childCount,
        /* [in] */ Int32 itemCount);

    /**
     * Creates a view into which preview text can be placed.
     */
    CARAPI_(AutoPtr<ITextView>) CreatePreviewTextView(
        /* [in] */ IContext* context);

    /**
     * Layouts a view within the specified bounds and pins the pivot point to
     * the appropriate edge.
     *
     * @param view The view to layout.
     * @param bounds Bounds at which to layout the view.
     */
    CARAPI_(void) ApplyLayout(
        /* [in] */ IView* view,
        /* [in] */ IRect* bounds);

    /**
     * Measures the preview text bounds, taking preview image padding into
     * account. This method should only be called after {@link #layoutThumb()}
     * and {@link #layoutTrack()} have both been called at least once.
     *
     * @param v The preview text view to measure.
     * @param out Rectangle into which measured bounds are placed.
     */
    CARAPI_(void) MeasurePreview(
        /* [in] */ IView* v,
        /* [in] */ IRect* out);

    /**
     * Measures the bounds for a view that should be laid out against the edge
     * of an adjacent view. If no adjacent view is provided, lays out against
     * the list edge.
     *
     * @param view The view to measure for layout.
     * @param adjacent (Optional) The adjacent view, may be null to align to the
     *            list edge.
     * @param margins Layout margins to apply to the view.
     * @param out Rectangle into which measured bounds are placed.
     */
    CARAPI_(void) MeasureViewToSide(
        /* [in] */ IView* view,
        /* [in] */ IView* adjacent,
        /* [in] */ IRect* margins,
        /* [in] */ IRect* out);

    CARAPI_(void) MeasureFloating(
        /* [in] */ IView* preview,
        /* [in] */ IRect* margins,
        /* [in] */ IRect* out);

    /**
     * Updates the container rectangle used for layout.
     */
    CARAPI_(void) UpdateContainerRect();

    /**
     * Lays out the thumb according to the current scrollbar position.
     */
    CARAPI_(void) LayoutThumb();

    /**
     * Lays out the track centered on the thumb. Must be called after
     * {@link #layoutThumb}.
     */
    CARAPI_(void) LayoutTrack();

    CARAPI_(void) SetState(
        /* [in] */ Int32 state);

    CARAPI_(void) RefreshDrawablePressedState();

    /**
     * Shows nothing.
     */
    CARAPI_(void) TransitionToHidden();

    /**
     * Shows the thumb and track.
     */
    CARAPI_(void) TransitionToVisible();

    /**
     * Shows the thumb, preview, and track.
     */
    CARAPI_(void) TransitionToDragging();

    CARAPI_(void) PostAutoHide();

    CARAPI_(void) GetSectionsFromIndexer();

    /**
     * Scrolls to a specific position within the section
     * @param position
     */
    CARAPI_(void) ScrollTo(
        /* [in] */ Float position);


    /**
     * Transitions the preview text to a new section. Handles animation,
     * measurement, and layout. If the new preview text is empty, returns false.
     *
     * @param sectionIndex The section index to which the preview should
     *            transition.
     * @return False if the new preview text is empty.
     */
    CARAPI_(Boolean) TransitionPreviewLayout(
        /* [in] */ Int32 sectionIndex);

    /**
     * Positions the thumb and preview widgets.
     *
     * @param position The position, between 0 and 1, along the track at which
     *            to place the thumb.
     */
    CARAPI_(void) SetThumbPos(
        /* [in] */ Float position);

    CARAPI_(Float) GetPosFromMotionEvent(
        /* [in] */ Float y);

    CARAPI_(Float) GetPosFromItemCount(
        /* [in] */ Int32 firstVisibleItem,
        /* [in] */ Int32 visibleItemCount,
        /* [in] */ Int32 totalItemCount);

    /**
     * Cancels an ongoing fling event by injecting a
     * {@link MotionEvent#ACTION_CANCEL} into the host view.
     */
    CARAPI_(void) CancelFling();

    /**
     * Cancels a pending drag.
     *
     * @see #startPendingDrag()
     */
    CARAPI_(void) CancelPendingDrag();

    /**
     * Delays dragging until after the framework has determined that the user is
     * scrolling, rather than tapping.
     */
    CARAPI_(void) StartPendingDrag();

    CARAPI_(void) BeginDrag();

    /**
     * Returns whether a coordinate is inside the scroller's activation area. If
     * there is a track image, touching anywhere within the thumb-width of the
     * track activates scrolling. Otherwise, the user has to touch inside thumb
     * itself.
     *
     * @param x The x-coordinate.
     * @param y The y-coordinate.
     * @return Whether the coordinate is inside the scroller's activation area.
     */
    CARAPI_(Boolean) IsPointInside(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(Boolean) IsPointInsideX(
        /* [in] */ Float x);

    CARAPI_(Boolean) IsPointInsideY(
        /* [in] */ Float y);

private:

    static CARAPI_(AutoPtr<IAnimator>) GroupAnimatorOfFloat(
        /* [in] */ IProperty* property,
        /* [in] */ Float value,
        /* [in] */ ArrayOf<IView*>* views);

    /**
     * Returns an animator for the view's scaleX value.
     */
    static CARAPI_(AutoPtr<IAnimator>) AnimateScaleX(
        /* [in] */ IView* v,
        /* [in] */ Float target);

    /**
     * Returns an animator for the view's alpha value.
     */
    static CARAPI_(AutoPtr<IAnimator>) AnimateAlpha(
        /* [in] */ IView* v,
        /* [in] */ Float alpha);

    static CARAPI_(AutoPtr<IAnimator>) AnimateBounds(
        /* [in] */ IView* v,
        /* [in] */ IRect* bounds);

private:
    /** Duration of fade-out animation. */
    const static Int32 DURATION_FADE_OUT;// = 300;

    /** Duration of fade-in animation. */
    const static Int32 DURATION_FADE_IN;// = 150;

    /** Duration of transition cross-fade animation. */
    const static Int32 DURATION_CROSS_FADE;// = 50;

    /** Duration of transition resize animation. */
    const static Int32 DURATION_RESIZE;// = 100;

    /** Inactivity timeout before fading controls. */
    const static Int64 FADE_TIMEOUT;// = 1500;

    /** Minimum number of pages to justify showing a fast scroll thumb. */
    const static Int32 MIN_PAGES;// = 4;

    /** Scroll thumb and preview not showing. */
    const static Int32 STATE_NONE;// = 0;

    /** Scroll thumb visible and moving along with the scrollbar. */
    const static Int32 STATE_VISIBLE;// = 1;

    /** Scroll thumb and preview being dragged by user. */
    const static Int32 STATE_DRAGGING;// = 2;

    // Positions for preview image and text.
    const static Int32 OVERLAY_FLOATING;// = 0;
    const static Int32 OVERLAY_AT_THUMB;// = 1;
    const static Int32 OVERLAY_ABOVE_THUMB;// = 2;

    // Indices for mPreviewResId.
    const static Int32 PREVIEW_LEFT;// = 0;
    const static Int32 PREVIEW_RIGHT;// = 1;

    /** Delay before considering a tap in the thumb area to be a drag. */
    const static Int64 TAP_TIMEOUT;// = ViewConfiguration.getTapTimeout();

    static AutoPtr<IProperty> LEFT;

    static AutoPtr<IProperty> TOP;

    static AutoPtr<IProperty> RIGHT;

    static AutoPtr<IProperty> BOTTOM;

    AutoPtr<IRect> mTempBounds; // = new Rect();
    AutoPtr<IRect> mTempMargins; // = new Rect();
    AutoPtr<IRect> mContainerRect; // = new Rect();

    IAbsListView* mList; //
    AutoPtr<IViewGroupOverlay> mOverlay; //
    AutoPtr<ITextView> mPrimaryText; //
    AutoPtr<ITextView> mSecondaryText; //
    AutoPtr<IImageView> mThumbImage; //
    AutoPtr<IImageView> mTrackImage; //
    AutoPtr<IView> mPreviewImage; //

    /**
     * Preview image resource IDs for left- and right-aligned layouts. See
     * {@link #PREVIEW_LEFT} and {@link #PREVIEW_RIGHT}.
     */
    AutoPtr<ArrayOf<Int32> > mPreviewResId; // = new int[2];

    /**
     * Padding in pixels around the preview text. Applied as layout margins to
     * the preview text and padding to the preview image.
     */
    Int32 mPreviewPadding;

    Int32 mPreviewMinWidth;
    Int32 mPreviewMinHeight;
    Int32 mThumbMinWidth;
    Int32 mThumbMinHeight;

    /** Theme-specified text size. Used only if text appearance is not set. */
    Float mTextSize;

    /** Theme-specified text color. Used only if text appearance is not set. */
    AutoPtr<IColorStateList> mTextColor;

    AutoPtr<IDrawable> mThumbDrawable;
    AutoPtr<IDrawable> mTrackDrawable;
    Int32 mTextAppearance;

    /** Total width of decorations. */
    Int32 mWidth;

    /** Set containing decoration transition animations. */
    AutoPtr<IAnimatorSet> mDecorAnimation;

    /** Set containing preview text transition animations. */
    AutoPtr<IAnimatorSet> mPreviewAnimation;

    /** Whether the primary text is showing. */
    Boolean mShowingPrimary;

    /** Whether we're waiting for completion of scrollTo(). */
    Boolean mScrollCompleted;

    /** The position of the first visible item in the list. */
    Int32 mFirstVisibleItem;

    /** The number of headers at the top of the view. */
    Int32 mHeaderCount;

    /** The index of the current section. */
    Int32 mCurrentSection;

    /** The current scrollbar position. */
    Int32 mScrollbarPosition;

    /** Whether the list is long enough to need a fast scroller. */
    Boolean mLongList;

    AutoPtr<ArrayOf<IInterface*> > mSections;

    /** Whether this view is currently performing layout. */
    Boolean mUpdatingLayout;

    /**
     * Current decoration state, one of:
     * <ul>
     * <li>{@link #STATE_NONE}, nothing visible
     * <li>{@link #STATE_VISIBLE}, showing track and thumb
     * <li>{@link #STATE_DRAGGING}, visible and showing preview
     * </ul>
     */
    Int32 mState;

    /** Whether the preview image is visible. */
    Boolean mShowingPreview;

    AutoPtr<IAdapter> mListAdapter;
    AutoPtr<ISectionIndexer> mSectionIndexer;

    /** Whether decorations should be laid out from right to left. */
    Boolean mLayoutFromRight;

    /** Whether the fast scroller is enabled. */
    Boolean mEnabled;

    /** Whether the scrollbar and decorations should always be shown. */
    Boolean mAlwaysShow;

    /**
     * Position for the preview image and text. One of:
     * <ul>
     * <li>{@link #OVERLAY_FLOATING}
     * <li>{@link #OVERLAY_AT_THUMB}
     * <li>{@link #OVERLAY_ABOVE_THUMB}
     * </ul>
     */
    Int32 mOverlayPosition;

    /** Current scrollbar style, including inset and overlay properties. */
    Int32 mScrollBarStyle;

    /** Whether to precisely match the thumb position to the list. */
    Boolean mMatchDragPosition;

    Float mInitialTouchY;
    Int64 mPendingDrag; // = -1;
    Int32 mScaledTouchSlop;

    Int32 mOldItemCount;
    Int32 mOldChildCount;

    AutoPtr<IRunnable> mDeferHide;
    AutoPtr<IAnimatorListener> mSwitchPrimaryListener;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_FASTSCROLLER_H__
