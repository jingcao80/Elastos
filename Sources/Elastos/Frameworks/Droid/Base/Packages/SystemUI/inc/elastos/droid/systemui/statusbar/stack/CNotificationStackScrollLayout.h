
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_CNOTICICATIONSTACKSCROLLLAYOUT_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_CNOTICICATIONSTACKSCROLLLAYOUT_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Stack_CNotificationStackScrollLayout.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Runnable.h>
#include <elastos/droid/view/ViewGroup.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::SystemUI::StatusBar::IExpandableViewOnHeightChangedListener;
using Elastos::Droid::SystemUI::StatusBar::Phone::IPhoneStatusBar;
using Elastos::Droid::SystemUI::StatusBar::Policy::IScrollAdapter;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IOnPreDrawListener;
using Elastos::Droid::View::IVelocityTracker;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::Widget::IOverScroller;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashSet;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Stack {

/**
 * A layout which handles a dynamic amount of notifications and presents them in a scrollable stack.
 */
CarClass(CNotificationStackScrollLayout)
    , public Elastos::Droid::View::ViewGroup
    , public INotificationStackScrollLayout
    , public ISwipeHelperCallback
    , public IExpandHelperCallback
    , public IScrollAdapter
{
public:
    class InnerListener
        : public Object
        , public IExpandableViewOnHeightChangedListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CNotificationStackScrollLayout* host);

        // @Override
        CARAPI OnHeightChanged(
            /* [in] */ IExpandableView* view);

        // @Override
        CARAPI OnReset(
            /* [in] */ IExpandableView* view);
    private:
        CNotificationStackScrollLayout* mHost;
    };

    class AnimationEvent: public Object
    {
    public:
        AnimationEvent(
            /* [in] */ IView* view,
            /* [in] */ Int32 type);

        AnimationEvent(
            /* [in] */ IView* view,
            /* [in] */ Int32 type,
            /* [in] */ Int64 length);

        CARAPI_(void) Init(
            /* [in] */ IView* view,
            /* [in] */ Int32 type,
            /* [in] */ Int64 length);

        /**
         * Combines the length of several animation events into a single value.
         *
         * @param events The events of the lengths to combine.
         * @return The combined length. Depending on the event types, this might be the maximum of
         *         all events or the length of a specific event.
         */
        static CARAPI_(Int64) CombineLength(
            /* [in] */ IArrayList/*<AnimationEvent>*/* events);

    public:
        static AutoPtr<ArrayOf<IAnimationFilter*> > FILTERS;

        static Int32 LENGTHS[];

        static const Int32 ANIMATION_TYPE_ADD = 0;
        static const Int32 ANIMATION_TYPE_REMOVE = 1;
        static const Int32 ANIMATION_TYPE_REMOVE_SWIPED_OUT = 2;
        static const Int32 ANIMATION_TYPE_TOP_PADDING_CHANGED = 3;
        static const Int32 ANIMATION_TYPE_START_DRAG = 4;
        static const Int32 ANIMATION_TYPE_SNAP_BACK = 5;
        static const Int32 ANIMATION_TYPE_ACTIVATED_CHILD = 6;
        static const Int32 ANIMATION_TYPE_DIMMED = 7;
        static const Int32 ANIMATION_TYPE_CHANGE_POSITION = 8;
        static const Int32 ANIMATION_TYPE_DARK = 9;
        static const Int32 ANIMATION_TYPE_GO_TO_FULL_SHADE = 10;
        static const Int32 ANIMATION_TYPE_HIDE_SENSITIVE = 11;
        static const Int32 ANIMATION_TYPE_VIEW_RESIZE = 12;
        static const Int32 ANIMATION_TYPE_EVERYTHING = 13;

        Int64 mEventStartTime;
        AutoPtr<IView> mChangingView;
        Int32 mAnimationType;
        AutoPtr<IAnimationFilter> mFilter;
        Int64 mLength;
        AutoPtr<IView> mViewAfterChangingView;
    };

private:
    class ChildrenUpdater
        : public Object
        , public IOnPreDrawListener
    {
    public:
        CAR_INTERFACE_DECL()

        ChildrenUpdater(
            /* [in] */ CNotificationStackScrollLayout* host);

        // @Override
        CARAPI OnPreDraw(
            /* [out] */ Boolean* result);

    private:
        CNotificationStackScrollLayout* mHost;
    };

    class EmptyShadeViewGoneRunnable: public Runnable
    {
    public:
        EmptyShadeViewGoneRunnable(
            /* [in] */ CNotificationStackScrollLayout* host);

        // @Override
        CARAPI Run();

    private:
        CNotificationStackScrollLayout* mHost;
    };

    class DismissViewGoneRunnable: public Runnable
    {
    public:
        DismissViewGoneRunnable(
            /* [in] */ CNotificationStackScrollLayout* host);

        // @Override
        CARAPI Run();

    private:
        CNotificationStackScrollLayout* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CNotificationStackScrollLayout();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI UpdateSpeedBumpIndex(
        /* [in] */ Int32 newIndex);

    CARAPI SetChildLocationsChangedListener(
        /* [in] */ IOnChildLocationsChangedListener* listener);

    /**
     * Returns the location the given child is currently rendered at.
     *
     * @param child the child to get the location for
     * @return one of {@link ViewState}'s <code>LOCATION_*</code> constants
     */
    CARAPI GetChildLocation(
        /* [in] */ IView* child,
        /* [out] */ Int32* result);

    CARAPI GetTopPadding(
        /* [out] */ Int32* padding);

    /**
     * Update the height of the stack to a new height.
     *
     * @param height the new height of the stack
     */
    CARAPI SetStackHeight(
        /* [in] */ Float height);

    CARAPI GetItemHeight(
        /* [out] */ Int32* result);

    CARAPI GetBottomStackPeekSize(
        /* [out] */ Int32* result);

    CARAPI GetCollapseSecondCardPadding(
        /* [out] */ Int32* result);

    CARAPI SetLongPressListener(
        /* [in] */ ISwipeHelperLongPressListener* listener);

    CARAPI SetScrollView(
        /* [in] */ IViewGroup* scrollView);

    CARAPI SetInterceptDelegateEnabled(
        /* [in] */ Boolean interceptDelegateEnabled);

    CARAPI OnChildDismissed(
        /* [in] */ IView* v);

    // @Override
    CARAPI OnChildSnappedBack(
        /* [in] */ IView* animView);

    // @Override
    CARAPI UpdateSwipeProgress(
        /* [in] */ IView* animView,
        /* [in] */ Boolean dismissable,
        /* [in] */ Float swipeProgress,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetFalsingThresholdFactor(
        /* [out] */ Float* result);

    CARAPI OnBeginDrag(
        /* [in] */ IView* v);

    CARAPI OnDragCancelled(
        /* [in] */ IView* v);

    CARAPI GetChildAtPosition(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ IView** view);

    CARAPI GetChildAtRawPosition(
        /* [in] */ Float touchX,
        /* [in] */ Float touchY,
        /* [out] */ IExpandableView** view);

    CARAPI GetChildAtPosition(
        /* [in] */ Float touchX,
        /* [in] */ Float touchY,
        /* [out] */ IExpandableView** view);

    CARAPI CanChildBeExpanded(
        /* [in] */ IView* v,
        /* [out] */ Boolean* result);

    CARAPI SetUserExpandedChild(
        /* [in] */ IView* v,
        /* [in] */ Boolean userExpanded);

    CARAPI SetUserLockedChild(
        /* [in] */ IView* v,
        /* [in] */ Boolean userLocked);

    // @Override
    CARAPI ExpansionStateChanged(
        /* [in] */ Boolean isExpanding);

    CARAPI SetScrollingEnabled(
        /* [in] */ Boolean enable);

    CARAPI SetExpandingEnabled(
        /* [in] */ Boolean enable);

    CARAPI GetChildContentView(
        /* [in] */ IView* v,
        /* [out] */ IView** view);

    CARAPI CanChildBeDismissed(
        /* [in] */ IView* v,
        /* [out] */ Boolean* can);

    // @Override
    CARAPI IsAntiFalsingNeeded(
        /* [out] */ Boolean* result);

    CARAPI DismissViewAnimated(
        /* [in] */ IView* child,
        /* [in] */ IRunnable* endRunnable,
        /* [in] */ Int32 delay,
        /* [in] */ Int64 duration);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ComputeScroll();

    /**
     * Set the amount of overScrolled pixels which will force the view to apply a rubber-banded
     * overscroll effect based on numPixels. By default this will also cancel animations on the
     * same overScroll edge.
     *
     * @param numPixels The amount of pixels to overScroll by. These will be scaled according to
     *                  the rubber-banding logic.
     * @param onTop Should the effect be applied on top of the scroller.
     * @param animate Should an animation be performed.
     */
    CARAPI SetOverScrolledPixels(
        /* [in] */ Float numPixels,
        /* [in] */ Boolean onTop,
        /* [in] */ Boolean animate);

    /**
     * Set the effective overScroll amount which will be directly reflected in the layout.
     * By default this will also cancel animations on the same overScroll edge.
     *
     * @param amount The amount to overScroll by.
     * @param onTop Should the effect be applied on top of the scroller.
     * @param animate Should an animation be performed.
     */
    CARAPI SetOverScrollAmount(
        /* [in] */ Float amount,
        /* [in] */ Boolean onTop,
        /* [in] */ Boolean animate);

    /**
     * Set the effective overScroll amount which will be directly reflected in the layout.
     *
     * @param amount The amount to overScroll by.
     * @param onTop Should the effect be applied on top of the scroller.
     * @param animate Should an animation be performed.
     * @param cancelAnimators Should running animations be cancelled.
     */
    CARAPI SetOverScrollAmount(
        /* [in] */ Float amount,
        /* [in] */ Boolean onTop,
        /* [in] */ Boolean animate,
        /* [in] */ Boolean cancelAnimators);

    /**
     * Set the effective overScroll amount which will be directly reflected in the layout.
     *
     * @param amount The amount to overScroll by.
     * @param onTop Should the effect be applied on top of the scroller.
     * @param animate Should an animation be performed.
     * @param cancelAnimators Should running animations be cancelled.
     * @param isRubberbanded The value which will be passed to
     *                     {@link OnOverscrollTopChangedListener#onOverscrollTopChanged}
     */
    CARAPI SetOverScrollAmount(
        /* [in] */ Float amount,
        /* [in] */ Boolean onTop,
        /* [in] */ Boolean animate,
        /* [in] */ Boolean cancelAnimators,
        /* [in] */ Boolean isRubberbanded);

    CARAPI SetOverscrollTopChangedListener(
        /* [in] */ IOnOverscrollTopChangedListener* overscrollTopChangedListener);

    CARAPI GetCurrentOverScrollAmount(
        /* [in] */ Boolean top,
        /* [out] */ Float* result);

    CARAPI GetCurrentOverScrolledPixels(
        /* [in] */ Boolean top,
        /* [out] */ Float* result);

    /**
     * @return the last child which has visibility unequal to GONE
     */
    CARAPI GetLastChildNotGone(
        /* [out] */ IView** view);

    /**
     * @return the number of children which have visibility unequal to GONE
     */
    CARAPI GetNotGoneChildCount(
        /* [out] */ Int32* result);

    CARAPI GetContentHeight(
        /* [out] */ Int32* result);

    CARAPI UpdateTopPadding(
        /* [in] */ Float qsHeight,
        /* [in] */ Int32 scrollY,
        /* [in] */ Boolean animate);

    CARAPI GetNotificationTopPadding(
        /* [out] */ Int32* result);

    CARAPI GetMinStackHeight(
        /* [out] */ Int32* result);

    CARAPI GetTopPaddingOverflow(
        /* [out] */ Float* result);

    CARAPI GetPeekHeight(
        /* [out] */ Int32* result);

    // @Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI SetAnimationsEnabled(
        /* [in] */ Boolean animationsEnabled);

    CARAPI IsAddOrRemoveAnimationPending(
        /* [out] */ Boolean* result);

    /**
     * Generate an animation for an added child view.
     *
     * @param child The view to be added.
     * @param fromMoreCard Whether this add is coming from the "more" card on lockscreen.
     */
    CARAPI GenerateAddAnimation(
        /* [in] */ IView* child,
        /* [in] */ Boolean fromMoreCard);

    /**
     * Change the position of child to a new location
     *
     * @param child the view to change the position for
     * @param newIndex the new index
     */
    CARAPI ChangeViewPosition(
        /* [in] */ IView* child,
        /* [in] */ Int32 newIndex);

    // @Override
    CARAPI OnWindowFocusChanged(
        /* [in] */ Boolean hasWindowFocus);

    CARAPI RemoveLongPressCallback();

    // @Override
    CARAPI IsScrolledToTop(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsScrolledToBottom(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetHostView(
        /* [out] */ IView** view);

    CARAPI GetEmptyBottomMargin(
        /* [out] */ Int32* result);

    CARAPI OnExpansionStarted();

    CARAPI OnExpansionStopped();

    // @Override
    CARAPI OnHeightChanged(
        /* [in] */ IExpandableView* view);

    // @Override
    CARAPI OnReset(
        /* [in] */ IExpandableView* view);

    CARAPI SetOnHeightChangedListener(
        /* [in] */ IExpandableViewOnHeightChangedListener* listener);

    CARAPI OnChildAnimationFinished();

    /**
     * See {@link AmbientState#setDimmed}.
     */
    CARAPI SetDimmed(
        /* [in] */ Boolean dimmed,
        /* [in] */ Boolean animate);

    CARAPI SetHideSensitive(
        /* [in] */ Boolean hideSensitive,
        /* [in] */ Boolean animate);

    /**
     * See {@link AmbientState#setActivatedChild}.
     */
    CARAPI SetActivatedChild(
        /* [in] */ IActivatableNotificationView* activatedChild);

    CARAPI GetActivatedChild(
        /* [out] */ IActivatableNotificationView** view);

    CARAPI SetSpeedBumpView(
        /* [in] */ ISpeedBumpView* speedBumpView);

    CARAPI GoToFullShade(
        /* [in] */ Int64 delay);

    CARAPI CancelExpandHelper();

    CARAPI SetIntrinsicPadding(
        /* [in] */ Int32 intrinsicPadding);

    CARAPI GetIntrinsicPadding(
        /* [out] */ Int32* result);

    /**
     * @return the y position of the first notification
     */
    CARAPI GetNotificationsTopY(
        /* [out] */ Float* result);

    // @Override
    CARAPI ShouldDelayChildPressedState(
        /* [out] */ Boolean* result);

    /**
     * See {@link AmbientState#setDark}.
     */
    CARAPI SetDark(
        /* [in] */ Boolean dark,
        /* [in] */ Boolean animate);

    CARAPI SetDismissView(
        /* [in] */ IDismissView* dismissView);

    CARAPI SetEmptyShadeView(
        /* [in] */ IEmptyShadeView* emptyShadeView);

    CARAPI UpdateEmptyShadeView(
        /* [in] */ Boolean visible);

    CARAPI UpdateDismissView(
        /* [in] */ Boolean visible);

    CARAPI SetDismissAllInProgress(
        /* [in] */ Boolean dismissAllInProgress);

    CARAPI IsDismissViewNotGone(
        /* [out] */ Boolean* result);

    CARAPI IsDismissViewVisible(
        /* [out] */ Boolean* result);

    CARAPI GetDismissViewHeight(
        /* [out] */ Int32* result);

    CARAPI GetBottomMostNotificationBottom(
        /* [out] */ Float* result);

    /**
     * @param qsMinHeight The minimum height of the quick settings including padding
     *                    See {@link StackScrollAlgorithm#updateIsSmallScreen}.
     */
    CARAPI UpdateIsSmallScreen(
        /* [in] */ Int32 qsMinHeight);

    CARAPI SetPhoneStatusBar(
        /* [in] */ IPhoneStatusBar* phoneStatusBar);

    CARAPI OnGoToKeyguard();

protected:
    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    // @Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    // @Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    // @Override
    CARAPI_(Boolean) OverScrollBy(
        /* [in] */ Int32 deltaX,
        /* [in] */ Int32 deltaY,
        /* [in] */ Int32 scrollX,
        /* [in] */ Int32 scrollY,
        /* [in] */ Int32 scrollRangeX,
        /* [in] */ Int32 scrollRangeY,
        /* [in] */ Int32 maxOverScrollX,
        /* [in] */ Int32 maxOverScrollY,
        /* [in] */ Boolean isTouchEvent);

    // @Override
    CARAPI OnOverScrolled(
        /* [in] */ Int32 scrollX,
        /* [in] */ Int32 scrollY,
        /* [in] */ Boolean clampedX,
        /* [in] */ Boolean clampedY);

    // @Override
    CARAPI_(void) OnViewRemoved(
        /* [in] */ IView* child);

    // @Override
    CARAPI_(void) OnViewAdded(
        /* [in] */ IView* child);

private:
    CARAPI_(void) InitView(
        /* [in] */ IContext* context);

    CARAPI_(void) UpdatePadding(
        /* [in] */ Boolean dimmed);

    CARAPI_(void) NotifyHeightChangeListener(
        /* [in] */ IExpandableView* view);

    CARAPI_(void) RequestAnimationOnViewResize();

    CARAPI_(void) SetMaxLayoutHeight(
        /* [in] */ Int32 maxLayoutHeight);

    CARAPI_(void) UpdateAlgorithmHeightAndPadding();

    /**
     * @return whether the height of the layout needs to be adapted, in order to ensure that the
     *         last child is not in the bottom stack.
     */
    CARAPI_(Boolean) NeedsHeightAdaption();

    /**
     * Updates the children views according to the stack scroll algorithm. Call this whenever
     * modifications to {@link #mOwnScrollY} are performed to reflect it in the view layout.
     */
    CARAPI_(void) UpdateChildren();

    CARAPI_(void) RequestChildrenUpdate();

    CARAPI_(Boolean) IsCurrentlyAnimating();

    CARAPI_(void) ClampScrollPosition();

    CARAPI_(void) SetTopPadding(
        /* [in] */ Int32 topPadding,
        /* [in] */ Boolean animate);

    /**
     * Get the current height of the view. This is at most the msize of the view given by a the
     * layout but it can also be made smaller by setting {@link #mCurrentStackHeight}
     *
     * @return either the layout height or the externally defined height, whichever is smaller
     */
    CARAPI_(Int32) GetLayoutHeight();

    CARAPI_(Boolean) IsScrollingEnabled();

    CARAPI_(void) SetSwipingInProgress(
        /* [in] */ Boolean isSwiped);

    CARAPI_(void) DispatchDownEventToScroller(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(Boolean) OnScrollTouch(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(void) OnOverScrollFling(
        /* [in] */ Boolean open,
        /* [in] */ Int32 initialVelocity);

    /**
     * Perform a scroll upwards and adapt the overscroll amounts accordingly
     *
     * @param deltaY The amount to scroll upwards, has to be positive.
     * @return The amount of scrolling to be performed by the scroller,
     *         not handled by the overScroll amount.
     */
    CARAPI_(Float) OverScrollUp(
        /* [in] */ Int32 deltaY,
        /* [in] */ Int32 range);

    /**
     * Perform a scroll downward and adapt the overscroll amounts accordingly
     *
     * @param deltaY The amount to scroll downwards, has to be negative.
     * @return The amount of scrolling to be performed by the scroller,
     *         not handled by the overScroll amount.
     */
    CARAPI_(Float) OverScrollDown(
        /* [in] */ Int32 deltaY);

    CARAPI_(void) OnSecondaryPointerUp(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(void) InitVelocityTrackerIfNotExists();

    CARAPI_(void) RecycleVelocityTracker();

    CARAPI_(void) InitOrResetVelocityTracker();

    CARAPI_(void) SetOverScrollAmountInternal(
        /* [in] */ Float amount,
        /* [in] */ Boolean onTop,
        /* [in] */ Boolean animate,
        /* [in] */ Boolean isRubberbanded);

    CARAPI_(void) NotifyOverscrollTopListener(
        /* [in] */ Float amount,
        /* [in] */ Boolean isRubberbanded);

    CARAPI_(void) SetOverScrolledPixels(
        /* [in] */ Float amount,
        /* [in] */ Boolean onTop);

    CARAPI_(void) CustomScrollTo(
        /* [in] */ Int32 y);

    CARAPI_(void) SpringBack();

    CARAPI_(Int32) GetScrollRange();

    /**
     * @return the first child which has visibility unequal to GONE
     */
    CARAPI_(AutoPtr<IView>) GetFirstChildNotGone();

    /**
     * @return The first child which has visibility unequal to GONE which is currently below the
     *         given translationY or equal to it.
     */
    CARAPI_(AutoPtr<IView>) GetFirstChildBelowTranlsationY(
        /* [in] */ Float translationY);

    CARAPI_(Int32) GetMaxExpandHeight(
        /* [in] */ IView* view);

    CARAPI_(void) UpdateContentHeight();

    /**
     * Fling the scroll view
     *
     * @param velocityY The initial velocity in the Y direction. Positive
     *                  numbers mean that the finger/cursor is moving down the screen,
     *                  which means we want to scroll towards the top.
     */
    CARAPI_(void) Fling(
        /* [in] */ Int32 velocityY);

    /**
     * @return Whether a fling performed on the top overscroll edge lead to the expanded
     * overScroll view (i.e QS).
     */
    CARAPI_(Boolean) ShouldOverScrollFling(
        /* [in] */ Int32 initialVelocity);

    CARAPI_(Int32) ClampPadding(
        /* [in] */ Int32 desiredPadding);

    CARAPI_(Float) GetRubberBandFactor(
        /* [in] */ Boolean onTop);

    /**
     * Accompanying function for {@link #getRubberBandFactor}: Returns TRUE if the overscroll is
     * rubberbanded, FALSE if it is technically an overscroll but rather a motion to expand the
     * overscroll view (e.g. expand QS).
     */
    CARAPI_(Boolean) IsRubberbanded(
        /* [in] */ Boolean onTop);

    CARAPI_(void) EndDrag();

    CARAPI_(void) TransformTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [in] */ IView* sourceView,
        /* [in] */ IView* targetView);

    CARAPI_(void) InitDownStates(
        /* [in] */ IMotionEvent* ev);

    /**
     * Generate a remove animation for a child view.
     *
     * @param child The view to generate the remove animation for.
     * @return Whether an animation was generated.
     */
    CARAPI_(Boolean) GenerateRemoveAnimation(
        /* [in] */ IView* child);

    /**
     * Updates the scroll position when a child was removed
     *
     * @param removedChild the removed child
     */
    CARAPI_(void) UpdateScrollStateForRemovedChild(
        /* [in] */ IView* removedChild);

    CARAPI_(Int32) GetIntrinsicHeight(
        /* [in] */ IView* view);

    CARAPI_(Int32) GetPositionInLinearLayout(
        /* [in] */ IView* requestedChild);

    CARAPI_(void) UpdateNotificationAnimationStates();

    CARAPI_(void) UpdateAnimationState(
        /* [in] */ IView* child);

    CARAPI_(void) UpdateAnimationState(
        /* [in] */ Boolean running,
        /* [in] */ IView* child);

    CARAPI_(void) StartAnimationToState();

    CARAPI_(void) GenerateChildHierarchyEvents();

    CARAPI_(void) GenerateViewResizeEvent();

    CARAPI_(void) GenerateSnapBackEvents();

    CARAPI_(void) GenerateDragEvents();

    CARAPI_(void) GenerateChildRemovalEvents();

    CARAPI_(void) GeneratePositionChangeEvents();

    CARAPI_(void) GenerateChildAdditionEvents();

    CARAPI_(void) GenerateTopPaddingEvent();

    CARAPI_(void) GenerateActivateEvent();

    CARAPI_(void) GenerateAnimateEverythingEvent();

    CARAPI_(void) GenerateDimmedEvent();

    CARAPI_(void) GenerateHideSensitiveEvent();

    CARAPI_(void) GenerateDarkEvent();

    CARAPI_(void) GenerateGoToFullShadeEvent();

    CARAPI_(Boolean) OnInterceptTouchEventScroll(
        /* [in] */ IMotionEvent* ev);

    /**
     * @return Whether the specified motion event is actually happening over the content.
     */
    CARAPI_(Boolean) IsInContentBounds(
        /* [in] */ IMotionEvent* event);

    CARAPI_(void) SetIsBeingDragged(
        /* [in] */ Boolean isDragged);

    CARAPI_(void) SetIsExpanded(
        /* [in] */ Boolean isExpanded);

    CARAPI_(void) UpdateScrollPositionOnExpandInBottom(
        /* [in] */ IExpandableView* view);

    CARAPI_(void) ApplyCurrentState();

    CARAPI_(void) UpdateSpeedBump(
        /* [in] */ Boolean visible);

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const Float RUBBER_BAND_FACTOR_NORMAL;
    static const Float RUBBER_BAND_FACTOR_AFTER_EXPAND;
    static const Float RUBBER_BAND_FACTOR_ON_PANEL_EXPAND;

    /**
     * Sentinel value for no current active pointer. Used by {@link #mActivePointerId}.
     */
    static const Int32 INVALID_POINTER;

    AutoPtr<IExpandHelper> mExpandHelper;
    AutoPtr<ISwipeHelper> mSwipeHelper;
    Boolean mSwipingInProgress;
    Int32 mCurrentStackHeight;
    Int32 mOwnScrollY;
    Int32 mMaxLayoutHeight;

    AutoPtr<IVelocityTracker> mVelocityTracker;
    AutoPtr<IOverScroller> mScroller;
    Int32 mTouchSlop;
    Int32 mMinimumVelocity;
    Int32 mMaximumVelocity;
    Int32 mOverflingDistance;
    Float mMaxOverScroll;
    Boolean mIsBeingDragged;
    Int32 mLastMotionY;
    Int32 mDownX;
    Int32 mActivePointerId;

    Int32 mSidePaddings;
    AutoPtr<IPaint> mDebugPaint;
    Int32 mContentHeight;
    Int32 mCollapsedSize;
    Int32 mBottomStackSlowDownHeight;
    Int32 mBottomStackPeekSize;
    Int32 mPaddingBetweenElements;
    Int32 mPaddingBetweenElementsDimmed;
    Int32 mPaddingBetweenElementsNormal;
    Int32 mTopPadding;
    Int32 mCollapseSecondCardPadding;

    /**
     * The algorithm which calculates the properties for our children
     */
    AutoPtr<IStackScrollAlgorithm> mStackScrollAlgorithm;

    /**
     * The current State this Layout is in
     */
    AutoPtr<IStackScrollState> mCurrentStackScrollState;
    AutoPtr<IAmbientState> mAmbientState;
    AutoPtr<IArrayList> mChildrenToAddAnimated;  /*<View*/
    AutoPtr<IArrayList> mChildrenToRemoveAnimated;  /*<View*/
    AutoPtr<IArrayList> mSnappedBackChildren;  /*<View*/
    AutoPtr<IArrayList> mDragAnimPendingChildren;  /*<View*/
    AutoPtr<IArrayList> mChildrenChangingPositions;  /*<View*/
    AutoPtr<IHashSet> mFromMoreCardAdditions;  /*<View*/
    AutoPtr<IArrayList> mAnimationEvents;  /*<AnimationEvent*/
    AutoPtr<IArrayList> mSwipedOutViews;  /*<View*/
    AutoPtr<IStackStateAnimator> mStateAnimator;
    Boolean mAnimationsEnabled;
    Boolean mChangePositionInProgress;

    /**
     * The raw amount of the overScroll on the top, which is not rubber-banded.
     */
    Float mOverScrolledTopPixels;

    /**
     * The raw amount of the overScroll on the bottom, which is not rubber-banded.
     */
    Float mOverScrolledBottomPixels;

    AutoPtr<IOnChildLocationsChangedListener> mListener;
    AutoPtr<IOnOverscrollTopChangedListener> mOverscrollTopChangedListener;
    AutoPtr<IExpandableViewOnHeightChangedListener> mOnHeightChangedListener;
    Boolean mNeedsAnimation;
    Boolean mTopPaddingNeedsAnimation;
    Boolean mDimmedNeedsAnimation;
    Boolean mHideSensitiveNeedsAnimation;
    Boolean mDarkNeedsAnimation;
    Boolean mActivateNeedsAnimation;
    Boolean mGoToFullShadeNeedsAnimation;
    Boolean mIsExpanded;
    Boolean mChildrenUpdateRequested;
    AutoPtr<ISpeedBumpView> mSpeedBumpView;
    Boolean mIsExpansionChanging;
    Boolean mExpandingNotification;
    Boolean mExpandedInThisMotion;
    Boolean mScrollingEnabled;
    AutoPtr<IDismissView> mDismissView;
    AutoPtr<IEmptyShadeView> mEmptyShadeView;
    Boolean mDismissAllInProgress;

    /**
     * Was the scroller scrolled to the top when the down motion was observed?
     */
    Boolean mScrolledToTopOnFirstDown;

    /**
     * The minimal amount of over scroll which is needed in order to switch to the quick settings
     * when over scrolling on a expanded card.
     */
    Float mMinTopOverScrollToEscape;
    Int32 mIntrinsicPadding;
    Int32 mNotificationTopPadding;
    Float mTopPaddingOverflow;
    Boolean mDontReportNextOverScroll;
    Boolean mRequestViewResizeAnimationOnLayout;
    Boolean mNeedViewResizeAnimation;
    Boolean mEverythingNeedsAnimation;

    /**
     * The maximum scrollPosition which we are allowed to reach when a notification was expanded.
     * This is needed to avoid scrolling too far after the notification was collapsed in the same
     * motion.
     */
    Int32 mMaxScrollAfterExpand;
    AutoPtr<ISwipeHelperLongPressListener> mLongPressListener;

    /**
     * Should in this touch motion only be scrolling allowed? It's TRUE when the scroller was
     * animating.
     */
    Boolean mOnlyScrollingInThisMotion;
    AutoPtr<IViewGroup> mScrollView;
    Boolean mInterceptDelegateEnabled;
    Boolean mDelegateToScrollView;
    Boolean mDisallowScrollingInThisMotion;
    Int64 mGoToFullShadeDelay;

    AutoPtr<IOnPreDrawListener> mChildrenUpdater;

    AutoPtr<IPhoneStatusBar> mPhoneStatusBar;
};

} // namespace Stack
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_CNOTICICATIONSTACKSCROLLLAYOUT_H__
