
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_PAGEDVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_PAGEDVIEW_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/view/ViewGroup.h"
#include "elastos/droid/os/Runnable.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPointF;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::View;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IVelocityTracker;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::IViewGroupOnHierarchyChangeListener;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::Widget::IScroller;
using Elastos::Core::IRunnable;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * An abstraction of the original Workspace which supports browsing through a
 * sequential list of "pages"
 */
class PagedView
    : public ViewGroup
    , public IPagedView
    , public IViewGroupOnHierarchyChangeListener
{
public:
    class SavedState
        : public View::BaseSavedState
    {
    public:
        TO_STRING_IMPL("PagedView::SavedState")

        SavedState();

        CARAPI constructor(
            /* [in] */ IParcelable* superState);

        //@Override
        CARAPI WriteToParcel(
            /* [in] */ IParcel* out);

        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

    public:
        Int32 mCurrentPage;

        // public static final Parcelable.Creator<SavedState> CREATOR =
        //         new Parcelable.Creator<SavedState>() {
        //     public SavedState createFromParcel(Parcel in) {
        //         return new SavedState(in);
        //     }

        //     public SavedState[] newArray(int size) {
        //         return new SavedState[size];
        //     }
        // };
    };

private:
    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PagedView::MyRunnable")

        MyRunnable(
            /* [in] */ PagedView* host,
            /* [in] */ Int32 pageUnderPointIndex,
            /* [in] */ Int32 dragViewIndex)
            : mHost(host)
            , mPageUnderPointIndex(pageUnderPointIndex)
            , mDragViewIndex(dragViewIndex)
        {}

        //@Override
        CARAPI Run();

    private:
        PagedView* mHost;
        Int32 mPageUnderPointIndex;
        Int32 mDragViewIndex;
    };

    class ScrollInterpolator
        : public Object
        , public IInterpolator
        , public ITimeInterpolator
    {
    public:
        TO_STRING_IMPL("PagedView::ScrollInterpolator")

        CAR_INTERFACE_DECL()

        CARAPI GetInterpolation(
            /* [in] */ Float input,
            /* [out] */ Float* output);

        CARAPI HasNativeInterpolator(
            /* [out] */ Boolean* res);
    };

    class MyRunnable2
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PagedView::MyRunnable2")

        MyRunnable2(
            /* [in] */ PagedView* host)
            : mHost(host)
        {}

        //@Override
        CARAPI Run();

    private:
        PagedView* mHost;
    };

    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("PagedView::MyAnimatorListenerAdapter")

        MyAnimatorListenerAdapter(
            /* [in] */ PagedView* host)
            : mHost(host)
            , mCancelled(FALSE)
        {}

        //@Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        PagedView* mHost;
        Boolean mCancelled;
    };

    class MyAnimatorListenerAdapter2
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("PagedView::MyAnimatorListenerAdapter2")

        MyAnimatorListenerAdapter2(
            /* [in] */ PagedView* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        PagedView* mHost;
    };

    class MyAnimatorListenerAdapter3
        : public AnimatorListenerAdapter
    {
    private:
        class MyAnimatorListenerAdapter4
            : public AnimatorListenerAdapter
        {
        public:
            TO_STRING_IMPL("PagedView::MyAnimatorListenerAdapter3::MyAnimatorListenerAdapter4")

            MyAnimatorListenerAdapter4(
                /* [in] */ PagedView* host)
                : mHost(host)
            {}

            //@Override
            CARAPI OnAnimationStart(
                /* [in] */ IAnimator* animation);

        private:
            PagedView* mHost;
        };

    public:
        TO_STRING_IMPL("PagedView::MyAnimatorListenerAdapter3")

        MyAnimatorListenerAdapter3(
            /* [in] */ PagedView* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

    private:
        PagedView* mHost;
    };

    class MyRunnable3
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PagedView::MyRunnable3")

        MyRunnable3(
            /* [in] */ PagedView* host)
            : mHost(host)
        {}

        //@Override
        CARAPI Run();

    private:
        PagedView* mHost;
    };

    class MyRunnable4
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PagedView::MyRunnable4")

        MyRunnable4(
            /* [in] */ PagedView* host,
            /* [in] */ IRunnable* onCompleteRunnable)
            : mHost(host)
            , mOnCompleteRunnable(onCompleteRunnable)
        {}

        //@Override
        CARAPI Run();

    private:
        PagedView* mHost;
        AutoPtr<IRunnable> mOnCompleteRunnable;
    };

    class MyAnimatorListenerAdapter5
        : public AnimatorListenerAdapter
    {
    private:
        class MyAnimatorListenerAdapter6
            : public AnimatorListenerAdapter
        {
        public:
            TO_STRING_IMPL("PagedView::MyAnimatorListenerAdapter5::MyAnimatorListenerAdapter6")

            MyAnimatorListenerAdapter6(
                /* [in] */ PagedView* host)
                : mHost(host)
            {}

            //@Override
            CARAPI OnAnimationEnd(
                /* [in] */ IAnimator* animation);

        private:
            PagedView* mHost;
        };

    public:
        TO_STRING_IMPL("PagedView::MyAnimatorListenerAdapter5")

        MyAnimatorListenerAdapter5(
            /* [in] */ PagedView* host,
            /* [in] */ IRunnable* onCompleteRunnable)
            : mHost(host)
            , mOnCompleteRunnable(onCompleteRunnable)
        {}

        //@Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        //@Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        PagedView* mHost;
        AutoPtr<IRunnable> mOnCompleteRunnable;
    };

    /**
     * Creates an animation from the current drag view along its current velocity vector.
     * For this animation, the alpha runs for a fixed duration and we update the position
     * progressively.
     */
    class FlingAlongVectorAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        TO_STRING_IMPL("PagedView::FlingAlongVectorAnimatorUpdateListener")

        CAR_INTERFACE_DECL()

        FlingAlongVectorAnimatorUpdateListener();

        CARAPI constructor(
            /* [in] */ IView* dragView,
            /* [in] */ IPointF* vel,
            /* [in] */ IRect* from,
            /* [in] */ Int64 startTime,
            /* [in] */ Float friction);

        //@Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        AutoPtr<IView> mDragView;
        AutoPtr<IPointF> mVelocity;
        AutoPtr<IRect> mFrom;
        Int64 mPrevTime;
        Float mFriction;

        AutoPtr<ITimeInterpolator> mAlphaInterpolator;
    };

    class MyRunnable5
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PagedView::MyRunnable5")

        MyRunnable5(
            /* [in] */ PagedView* host,
            /* [in] */ IView* dragView)
            : mHost(host)
            , mDragView(dragView)
        {}

        //@Override
        CARAPI Run();

    private:
        PagedView* mHost;
        AutoPtr<IView> mDragView;
    };

    class MyAnimatorListenerAdapter7
        : public AnimatorListenerAdapter
    {
    private:
        class MyRunnable6
            : public Runnable
        {
        public:
            TO_STRING_IMPL("PagedView::MyAnimatorListenerAdapter7::MyRunnable6")

            MyRunnable6(
                /* [in] */ PagedView* host)
                : mHost(host)
            {}

            //@Override
            CARAPI Run();

        private:
            PagedView* mHost;
        };
    public:
        TO_STRING_IMPL("PagedView::MyAnimatorListenerAdapter7")

        MyAnimatorListenerAdapter7(
            /* [in] */ PagedView* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        PagedView* mHost;
    };

    class MyTimeInterpolator
        : public Object
        , public ITimeInterpolator
    {
    public:
        TO_STRING_IMPL("PagedView::MyTimeInterpolator")

        CAR_INTERFACE_DECL()

        MyTimeInterpolator(
            /* [in] */ PagedView* host,
            /* [in] */ Int64 startTime)
            : mHost(host)
            , mCount(-1)
            , mStartTime(startTime)
            , mOffset(0.0f)
        {}

        CARAPI GetInterpolation(
            /* [in] */ Float input,
            /* [out] */ Float* output);

        CARAPI HasNativeInterpolator(
            /* [out] */ Boolean* res);

    private:
        PagedView* mHost;
        Int32 mCount;
        Int64 mStartTime;
        Float mOffset;
    };

    class MyAnimatorListenerAdapter8
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("PagedView::MyAnimatorListenerAdapter8")

        MyAnimatorListenerAdapter8(
            /* [in] */ IRunnable* onAnimationEndRunnable)
            : mOnAnimationEndRunnable(onAnimationEndRunnable)
        {}

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IRunnable> mOnAnimationEndRunnable;
    };

    class MyAnimatorListenerAdapter9
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("PagedView::MyAnimatorListenerAdapter9")

        MyAnimatorListenerAdapter9(
            /* [in] */ IRunnable* onAnimationEndRunnable)
            : mOnAnimationEndRunnable(onAnimationEndRunnable)
        {}

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IRunnable> mOnAnimationEndRunnable;
    };

    class MyAnimatorListenerAdapter10
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("PagedView::MyAnimatorListenerAdapter10")

        MyAnimatorListenerAdapter10(
            /* [in] */ PagedView* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        //@Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

    private:
        PagedView* mHost;
    };

    class MyAnimatorListenerAdapter11
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("PagedView::MyAnimatorListenerAdapter11")

        MyAnimatorListenerAdapter11(
            /* [in] */ PagedView* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        //@Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

    private:
        PagedView* mHost;
    };

public:
    TO_STRING_IMPL("PagedView")

    CAR_INTERFACE_DECL()

    PagedView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetDeleteDropTarget(
        /* [in] */IView* v);

    // Convenience methods to map points from self to parent and vice versa
    CARAPI MapPointFromViewToParent(
        /* [in] */ IView* v,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [out, callee] */ ArrayOf<Float>** array);

    CARAPI MapPointFromParentToView(
        /* [in] */ IView* v,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [out, callee] */ ArrayOf<Float>** array);

    CARAPI UpdateDragViewTranslationDuringDrag();

    CARAPI SetMinScale(
        /* [in] */ Float f);

    //@Override
    CARAPI SetScaleX(
        /* [in] */ Float scaleX);

    // Convenience methods to get the actual width/height of the PagedView (since it is measured
    // to be larger to account for the minimum possible scale)
    CARAPI GetViewportWidth(
        /* [out] */ Int32* width);

    CARAPI GetViewportHeight(
        /* [out] */ Int32* height);

    // Convenience methods to get the offset ASSUMING that we are centering the pages in the
    // PagedView both horizontally and vertically
    CARAPI GetViewportOffsetX(
        /* [out] */ Int32* x);

    CARAPI GetViewportOffsetY(
        /* [out] */ Int32* y);

    CARAPI SetPageSwitchListener(
        /* [in] */ IPagedViewPageSwitchListener* pageSwitchListener);

    /**
     * Returns the index of the currently displayed page.
     *
     * @return The index of the currently displayed page.
     */
    CARAPI GetCurrentPage(
        /* [out] */ Int32* page);

    CARAPI GetNextPage(
        /* [out] */ Int32* page);

    CARAPI GetPageCount(
        /* [out] */ Int32* count);

    CARAPI GetPageAt(
        /* [in] */ Int32 index,
        /* [out] */ IView** view);

    /**
     * Sets the current page.
     */
    CARAPI SetCurrentPage(
        /* [in] */ Int32 currentPage);

    CARAPI SetOnlyAllowEdgeSwipes(
        /* [in] */ Boolean enable);

     /**
     * Registers the specified listener on each page contained in this workspace.
     *
     * @param l The listener used to respond to long clicks.
     */
    //@Override
    CARAPI SetOnLongClickListener(
        /* [in] */ IViewOnLongClickListener* l);

    //@Override
    CARAPI ScrollBy(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    //@Override
    CARAPI ScrollTo(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    //@Override
    CARAPI ComputeScroll();

    CARAPI SetPageSpacing(
        /* [in] */ Int32 pageSpacing);

    //@Override
    CARAPI OnChildViewAdded(
        /* [in] */ IView* parent,
        /* [in] */ IView* child);

    //@Override
    CARAPI OnChildViewRemoved(
        /* [in] */ IView* parent,
        /* [in] */ IView* child);

    CARAPI BoundByReorderablePages(
        /* [in] */ Boolean isReordering,
        /* [in] */ ArrayOf<Int32>* range);

    //@Override
    CARAPI RequestChildRectangleOnScreen(
        /* [in] */ IView* child,
        /* [in] */ IRect* rectangle,
        /* [in] */ Boolean immediate,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI DispatchUnhandledMove(
        /* [in] */ IView* focused,
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI AddFocusables(
        /* [in] */ IArrayList* views,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 focusableMode);

    /**
     * If one of our descendant views decides that it could be focused now, only
     * pass that along if it's on the current page.
     *
     * This happens when live folders requery, and if they're off page, they
     * end up calling requestFocus, which pulls it on page.
     */
    //@Override
    CARAPI FocusableViewAvailable(
        /* [in] */ IView* focused);

    //@Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI OnTouchStateChanged(
        /* [in] */ Int32 newTouchState);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    //public abstract void onFlingToDelete(View v);
    virtual CARAPI OnRemoveView(
        /* [in] */ IView* v,
        /* [in] */ Boolean deletePermanently) = 0;

    virtual CARAPI OnRemoveViewAnimationCompleted() = 0;

    virtual CARAPI OnAddView(
        /* [in] */ IView* v,
        /* [in] */ Int32 index) = 0;

    //@Override
    CARAPI OnGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI RequestChildFocus(
        /* [in] */ IView* child,
        /* [in] */ IView* focused);

    CARAPI GetPageNearestToPoint(
        /* [in] */ Float x,
        /* [out] */ Int32* page);

    CARAPI GetPageNearestToCenterOfScreen(
        /* [out] */ Int32* page);

    // We want the duration of the page snap animation to be influenced by the distance that
    // the screen has to travel, however, we don't want this duration to be effected in a
    // purely linear fashion. Instead, we use this method to moderate the effect that the distance
    // of travel has on the overall snap duration.
    CARAPI DistanceInfluenceForSnapDuration(
        /* [in] */ Float f,
        /* [out] */ Float* duration);

    CARAPI ScrollLeft();

    CARAPI ScrollRight();

    CARAPI GetPageForView(
        /* [in] */ IView* v,
        /* [out] */ Int32* page);

    // Animate the drag view back to the original position
    CARAPI AnimateDragViewToOriginalPosition();

    CARAPI StartReordering(
        /* [out] */ Boolean* result);

    CARAPI IsReordering(
        /* [in] */ Boolean testTouchState,
        /* [out] */ Boolean* result);

    CARAPI EndReordering();

    CARAPI OnFlingToDelete(
        /* [in] */ IPointF* vel);

    /* Accessibility */
    //@Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    //@Override
    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    //@Override
    CARAPI PerformAccessibilityAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnHoverEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI BeginCameraEvent();

    CARAPI EndCameraEvent();

    /**
     * Swaps the position of the views by setting the left and right edges appropriately.
     */
    CARAPI SwapPages(
        /* [in] */ Int32 indexA,
        /* [in] */ Int32 indexB);

    CARAPI StartPageWarp(
        /* [in] */ Int32 pageIndex);

    CARAPI StopPageWarp();

    CARAPI OnPageBeginWarp();

    CARAPI OnPageEndWarp();

protected:
    /**
     * Initializes various states for this workspace.
     */
    CARAPI Init();

    /**
     * Called by subclasses to mark that data is ready, and that we can begin loading and laying
     * out pages.
     */
    CARAPI SetDataIsReady();

    CARAPI IsDataReady(
        /* [out] */ Boolean* result);

    CARAPI IndexToPage(
        /* [in] */ Int32 index,
        /* [out] */ Int32* page);

    /**
     * Updates the scroll of the current page immediately to its final scroll position.  We use this
     * in CustomizePagedView to allow tabs to share the same PagedView while resetting the scroll of
     * the previous tab page.
     */
    CARAPI UpdateCurrentPageScroll();

    CARAPI NotifyPageSwitching(
        /* [in] */ Int32 whichPage);

    CARAPI NotifyPageSwitched();

    CARAPI PageBeginMoving();

    CARAPI PageEndMoving();

    CARAPI IsPageMoving(
        /* [out] */ Boolean* result);

    // a method that subclasses can override to add behavior
    CARAPI OnPageBeginMoving();

    // a method that subclasses can override to add behavior
    CARAPI OnPageEndMoving();

    // we moved this functionality to a helper function so SmoothPagedView can reuse it
    CARAPI ComputeScrollHelper(
        /* [out] */ Boolean* result);

    CARAPI ShouldSetTopAlignedPivotForWidget(
        /* [in] */ Int32 childIndex,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI ScreenScrolled(
        /* [in] */ Int32 screenCenter);

    CARAPI InvalidateCachedOffsets();

    CARAPI GetChildOffset(
        /* [in] */ Int32 index,
        /* [out] */ Int32* offset);

    CARAPI GetRelativeChildOffset(
        /* [in] */ Int32 index,
        /* [out] */ Int32* outoffset);

    CARAPI GetScaledMeasuredWidth(
        /* [in] */ IView* child,
        /* [out] */ Int32* outwidth);

    // TODO: Fix this
    CARAPI GetVisiblePages(
        /* [in] */ ArrayOf<Int32>* range);

    CARAPI ShouldDrawChild(
        /* [in] */ IView* child,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

    //@Override
    CARAPI OnRequestFocusInDescendants(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect,
        /* [out] */ Boolean* result);

    /**
     * Return true if a tap at (x, y) should trigger a flip to the previous page.
     */
    CARAPI HitsPreviousPage(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [out] */ Boolean* result);

    /**
     * Return true if a tap at (x, y) should trigger a flip to the next page.
     */
    CARAPI HitsNextPage(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [out] */ Boolean* result);

    /*
     * Determines if we should change the touch state to start scrolling after the
     * user moves their touch point too far.
     */
    CARAPI DetermineScrollingStart(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI GetMaxScrollProgress(
        /* [out] */ Float* progress);

    CARAPI GetBoundedScrollProgress(
        /* [in] */ Int32 screenCenter,
        /* [in] */ IView* v,
        /* [in] */ Int32 page,
        /* [out] */ Float* progress);

    CARAPI GetScrollProgress(
        /* [in] */ Int32 screenCenter,
        /* [in] */ IView* v,
        /* [in] */ Int32 page,
        /* [out] */ Float* progress);

    CARAPI AcceleratedOverScroll(
        /* [in] */ Float amount);

    CARAPI DampedOverScroll(
        /* [in] */ Float amount);

    CARAPI OverScroll(
        /* [in] */ Float amount);

    CARAPI MaxOverScroll(
        /* [out] */ Float* scroll);

    CARAPI OnUnhandledTap(
        /* [in] */ IMotionEvent* ev);

    CARAPI GetChildIndexForRelativeOffset(
        /* [in] */ Int32 relativeOffset,
        /* [out] */ Int32* offset);

    CARAPI GetChildWidth(
        /* [in] */ Int32 index,
        /* [out] */ Int32* width);

    CARAPI SnapToDestination();

    CARAPI SnapToPageWithVelocity(
        /* [in] */ Int32 whichPage,
        /* [in] */ Int32 velocity);

    CARAPI SnapToPage(
        /* [in] */ Int32 whichPage);

    CARAPI SnapToPageImmediately(
        /* [in] */ Int32 whichPage);

    CARAPI SnapToPage(
        /* [in] */ Int32 whichPage,
        /* [in] */ Int32 duration);

    CARAPI SnapToPage(
        /* [in] */ Int32 whichPage,
        /* [in] */ Int32 duration,
        /* [in] */ Boolean immediate);

    CARAPI SnapToPage(
        /* [in] */ Int32 whichPage,
        /* [in] */ Int32 delta,
        /* [in] */ Int32 duration);

    CARAPI SnapToPage(
        /* [in] */ Int32 whichPage,
        /* [in] */ Int32 delta,
        /* [in] */ Int32 duration,
        /* [in] */ Boolean immediate);

    CARAPI IsWarping(
        /* [out] */ Boolean* result);

    CARAPI GetScrollingIndicator(
        /* [out] */ IView** view);

    CARAPI IsScrollingIndicatorEnabled(
        /* [out] */ Boolean* result);

    CARAPI FlashScrollingIndicator(
        /* [in] */ Boolean animated);

    CARAPI ShowScrollingIndicator(
        /* [in] */ Boolean immediately);

    CARAPI CancelScrollingIndicatorAnimations();

    CARAPI HideScrollingIndicator(
        /* [in] */ Boolean immediately);

    /**
     * To be overridden by subclasses to determine whether the scroll indicator should stretch to
     * fill its space on the track or not.
     */
    CARAPI HasElasticScrollIndicator(
        /* [out] */ Boolean* result);

    // "Zooms out" the PagedView to reveal more side pages
    CARAPI ZoomOut(
        /* [out] */ Boolean* result);

    CARAPI OnStartReordering();

    CARAPI OnEndReordering();

    // "Zooms in" the PagedView to highlight the current page
    CARAPI ZoomIn(
        /* [in] */ IRunnable* onCompleteRunnable,
        /* [out] */ Boolean* result);

    CARAPI SetPageHoveringOverDeleteDropTarget(
        /* [in] */ Int32 viewIndex,
        /* [in] */ Boolean isHovering);

    CARAPI GetPageWarpIndex(
        /* [out] */ Int32* index);

private:
    CARAPI_(void) DispatchOnPageBeginWarp();

    CARAPI_(void) DispatchOnPageEndWarp();

    /** Returns whether x and y originated within the buffered viewport */
    CARAPI_(Boolean) IsTouchPointInViewportWithBuffer(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

     /** Returns whether x and y originated within the current page view bounds */
    CARAPI_(Boolean) IsTouchPointInCurrentPage(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI_(void) SetTouchState(
        /* [in] */ Int32 touchState);

    /**
     * Save the state when we get {@link MotionEvent#ACTION_DOWN}
     * @param ev
     */
    CARAPI_(void) SaveDownState(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(Boolean) IsHorizontalCameraScroll(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(void) StartScrolling(
        /* [in] */ IMotionEvent* ev);

    // This curve determines how the effect of scrolling over the limits of the page dimishes
    // as the user pulls further and further from the bounds
    CARAPI_(Float) OverScrollInfluenceCurve(
        /* [in] */ Float f);

    CARAPI_(void) ResetTouchState();

    CARAPI_(void) AcquireVelocityTrackerAndAddMovement(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(void) ReleaseVelocityTracker();

    CARAPI_(void) OnSecondaryPointerUp(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(Int32) GetPageSnapDuration();

    CARAPI_(void) UpdateScrollingIndicator();

    CARAPI_(void) UpdateScrollingIndicatorPosition();

    CARAPI_(void) OnPostReorderingAnimationCompleted();

    /*
     * Flinging to delete - IN PROGRESS
     */
    CARAPI_(AutoPtr<IPointF>) IsFlingingToDelete();

    CARAPI_(AutoPtr<IRunnable>) CreatePostDeleteAnimationRunnable(
        /* [in] */ IView* dragView);

    /* Drag to delete */
    CARAPI_(Boolean) IsHoveringOverDeleteDropTarget(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI_(void) OnDropToDelete();

    CARAPI_(void) CancelWarpAnimation(
        /* [in] */ const String& msg,
        /* [in] */ Boolean abortAnimation);

    CARAPI_(Boolean) IsAnimatingWarpPage();

    CARAPI_(void) AnimateWarpPageOnScreen(
        /* [in] */ const String& reason);

    CARAPI_(Int32) GetCurrentWarpOffset();

    CARAPI_(void) AnimateWarpPageOffScreen(
        /* [in] */ const String& reason,
        /* [in] */ Boolean animate);

protected:
    static const Int32 INVALID_PAGE;

    static const Int32 PAGE_SNAP_ANIMATION_DURATION;
    static const Int32 SLOW_PAGE_SNAP_ANIMATION_DURATION;
    static const Float NANOTIME_DIV;

    Int32 mFlingThresholdVelocity;
    Int32 mMinFlingVelocity;
    Int32 mMinSnapVelocity;

    Float mDensity;
    Float mSmoothingTime;
    Float mTouchX;

    Boolean mFirstLayout;

    Int32 mCurrentPage;
    Int32 mChildCountOnLastMeasure;

    Int32 mNextPage;
    Int32 mMaxScrollX;
    AutoPtr<IScroller> mScroller;

    Float mLastMotionX;
    Float mLastMotionXRemainder;
    Float mLastMotionY;
    Float mTotalMotionX;

    const static Int32 TOUCH_STATE_REST;
    const static Int32 TOUCH_STATE_SCROLLING;
    const static Int32 TOUCH_STATE_PREV_PAGE;
    const static Int32 TOUCH_STATE_NEXT_PAGE;
    const static Int32 TOUCH_STATE_REORDERING;
    const static Int32 TOUCH_STATE_READY; // when finger is down

    const static Float ALPHA_QUANTIZE_LEVEL;
    const static Float TOUCH_SLOP_SCALE;

    Int32 mTouchState;
    Boolean mForceScreenScrolled;

    AutoPtr<IViewOnLongClickListener> mLongClickListener;

    Int32 mTouchSlop;

    Int32 mPageSpacing;
    Int32 mCellCountX;
    Int32 mCellCountY;
    Boolean mAllowOverScroll;
    Int32 mUnboundedScrollX;
    AutoPtr<ArrayOf<Int32> > mTempVisiblePagesRange;
    Boolean mForceDrawAllChildrenNextFrame;

    // mOverScrollX is equal to getScrollX() when we're within the normal scroll range. Otherwise
    // it is equal to the scaled overscroll position. We use a separate value so as to prevent
    // the screens from continuing to translate beyond the normal bounds.
    Int32 mOverScrollX;

    // parameter that adjusts the layout to be optimized for pages with that scale factor
    Float mLayoutScale;

    static const Int32 INVALID_POINTER;

    Int32 mActivePointerId;

    AutoPtr<IArrayList> mDirtyPageContent;

    // If true, syncPages and syncPageItems will be called to refresh pages
    Boolean mContentIsRefreshable;

    // If true, modify alpha of neighboring pages as user scrolls left/right
    Boolean mFadeInAdjacentScreens;

    // It true, use a different slop parameter (pagingTouchSlop = 2 * touchSlop) for deciding
    // to switch to a new page
    Boolean mUsePagingTouchSlop;

    // If true, the subclass should directly update scrollX itself in its computeScroll method
    // (SmoothPagedView does this)
    Boolean mDeferScrollUpdate;

    Boolean mIsPageMoving;

    // All syncs and layout passes are deferred until data is ready.
    Boolean mIsDataReady;

    static const Int32 sScrollIndicatorFadeInDuration;
    static const Int32 sScrollIndicatorFadeOutDuration;
    static const Int32 sScrollIndicatorFlashDuration;

    Int32 REORDERING_REORDER_REPOSITION_DURATION;
    Int32 REORDERING_ZOOM_IN_OUT_DURATION;

    AutoPtr<IView> mDragView;
    AutoPtr<IAnimatorSet> mZoomInOutAnim;

    Int32 mFlingToDeleteThresholdVelocity;
private:
    static const Int32 WARP_SNAP_DURATION;
    static const String TAG;
    static const Boolean DEBUG;
    static const Boolean DEBUG_WARP;

    static const Int32 WARP_PEEK_ANIMATION_DURATION;
    static const Float WARP_ANIMATE_AMOUNT; // in dip

    // the min drag distance for a fling to register, to prevent random page shifts
    static const Int32 MIN_LENGTH_FOR_FLING;

    static const Float OVERSCROLL_ACCELERATE_FACTOR;
    static const Float OVERSCROLL_DAMP_FACTOR;

    static const Float RETURN_TO_ORIGINAL_PAGE_THRESHOLD;
    // The page is moved more than halfway, automatically move to the next page on touch up.
    static const Float SIGNIFICANT_MOVE_THRESHOLD;

    // The following constants need to be scaled based on density. The scaled versions will be
    // assigned to the corresponding member variables below.
    static const Int32 FLING_THRESHOLD_VELOCITY;
    static const Int32 MIN_SNAP_VELOCITY;
    static const Int32 MIN_FLING_VELOCITY;

    // We are disabling touch interaction of the widget region for factory ROM.
    static const Boolean DISABLE_TOUCH_INTERACTION;
    static const Boolean DISABLE_TOUCH_SIDE_PAGES;
    static const Boolean DISABLE_FLING_TO_DELETE;

    AutoPtr<IVelocityTracker> mVelocityTracker;

    Float mParentDownMotionX;
    Float mParentDownMotionY;
    Float mDownMotionX;
    Float mDownMotionY;
    Float mDownScrollX;
    Int32 mLastScreenCenter;
    AutoPtr<ArrayOf<Int32> > mChildOffsets;
    AutoPtr<ArrayOf<Int32> > mChildRelativeOffsets;
    AutoPtr<ArrayOf<Int32> > mChildOffsetsWithLayoutScale;
    String mDeleteString; // Accessibility announcement when widget is deleted

    Int32 mPagingTouchSlop;
    Int32 mMaximumVelocity;
    Int32 mMinimumWidth;

    AutoPtr<IPagedViewPageSwitchListener> mPageSwitchListener;

    // Scrolling indicator
    AutoPtr<IValueAnimator> mScrollIndicatorAnimator;
    AutoPtr<IView> mScrollIndicator;
    Int32 mScrollIndicatorPaddingLeft;
    Int32 mScrollIndicatorPaddingRight;
    Boolean mShouldShowScrollIndicator;
    Boolean mShouldShowScrollIndicatorImmediately;

    // The viewport whether the pages are to be contained (the actual view may be larger than the
    // viewport)
    AutoPtr<IRect> mViewport;

    // Reordering
    // We use the min scale to determine how much to expand the actually PagedView measured
    // dimensions such that when we are zoomed out, the view is not clipped
    Int32 REORDERING_DROP_REPOSITION_DURATION;
    Int32 REORDERING_SIDE_PAGE_HOVER_TIMEOUT;
    Float REORDERING_SIDE_PAGE_BUFFER_PERCENTAGE;
    Int64 REORDERING_DELETE_DROP_TARGET_FADE_DURATION;
    Float mMinScale;
    AutoPtr<IRunnable> mSidePageHoverRunnable;
    Int32 mSidePageHoverIndex;
    // This variable's scope is only for the duration of startReordering() and endReordering()
    Boolean mReorderingStarted;
    // This variable's scope is for the duration of startReordering() and after the zoomIn()
    // animation after endReordering()
    Boolean mIsReordering;
    // The runnable that settles the page after snapToPage and animateDragViewToOriginalPosition
    Int32 NUM_ANIMATIONS_RUNNING_BEFORE_ZOOM_OUT;
    Int32 mPostReorderingPreZoomInRemainingAnimationCount;
    AutoPtr<IRunnable> mPostReorderingPreZoomInRunnable;

    // Edge swiping
    Boolean mOnlyAllowEdgeSwipes;
    Boolean mDownEventOnEdge;
    Int32 mEdgeSwipeRegionSize;

    // Convenience/caching
    AutoPtr<IMatrix> mTmpInvMatrix;
    AutoPtr<ArrayOf<Float> > mTmpPoint;
    AutoPtr<IRect> mTmpRect;
    AutoPtr<IRect> mAltTmpRect;

    // Fling to delete
    Int32 FLING_TO_DELETE_FADE_OUT_DURATION;
    Float FLING_TO_DELETE_FRICTION;
    // The degrees specifies how much deviation from the up vector to still consider a fling "up"
    Float FLING_TO_DELETE_MAX_FLING_DEGREES;
    // Drag to delete
    Boolean mDeferringForDelete;
    Int32 DELETE_SLIDE_IN_SIDE_PAGE_DURATION;
    Int32 DRAG_TO_DELETE_FADE_OUT_DURATION;

    // Drop to delete
    AutoPtr<IView> mDeleteDropTarget;

    // Bouncer
    Boolean mTopAlignPageWhenShrinkingForBouncer;

    // Page warping
    Int32 mPageSwapIndex; // the page we swapped out if needed
    Int32 mPageWarpIndex; // the page we intend to warp

    Boolean mWarpPageExposed;
    AutoPtr<IViewPropertyAnimator> mWarpAnimation;

    Boolean mIsCameraEvent;
    Float mWarpPeekAmount;
    Boolean mOnPageEndWarpCalled;
    Boolean mOnPageBeginWarpCalled;

    AutoPtr<IRunnable> mHideScrollingIndicatorRunnable;

    AutoPtr<IAnimatorListener> mOnScreenAnimationListener;

    AutoPtr<IAnimatorListener> mOffScreenAnimationListener;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_PAGEDVIEW_H__