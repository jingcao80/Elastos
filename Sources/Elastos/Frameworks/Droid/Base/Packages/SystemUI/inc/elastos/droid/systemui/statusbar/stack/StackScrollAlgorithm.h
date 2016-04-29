
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_STACKSCROLLALGORITHM_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_STACKSCROLLALGORITHM_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/systemui/statusbar/stack/StackScrollState.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnLayoutChangeListener;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

#define ViewState StackScrollState::ViewState

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Stack {

/**
 * The Algorithm of the {@link com.android.systemui.statusbar.stack
 * .NotificationStackScrollLayout} which can be queried for {@link com.android.systemui.statusbar
 * .stack.StackScrollState}
 */
class StackScrollAlgorithm
    : public Object
    , public IStackScrollAlgorithm
{
private:
    class StackScrollAlgorithmState: public Object
    {
    public:
        StackScrollAlgorithmState();

    public:
        /**
         * The scroll position of the algorithm
         */
        Int32 mScrollY;

        /**
         *  The quantity of items which are in the top stack.
         */
        Float mItemsInTopStack;

        /**
         * how far in is the element currently transitioning into the top stack
         */
        Float mPartialInTop;

        /**
         * The number of pixels the last child in the top stack has scrolled in to the stack
         */
        Float mScrolledPixelsTop;

        /**
         * The last item index which is in the top stack.
         */
        Int32 mLastTopStackIndex;

        /**
         * The quantity of items which are in the bottom stack.
         */
        Float mItemsInBottomStack;

        /**
         * how far in is the element currently transitioning into the bottom stack
         */
        Float mPartialInBottom;

        /**
         * The children from the host view which are not gone.
         */
        AutoPtr<IArrayList> mVisibleChildren;  /*<ExpandableView*/
    };

    class OnLayoutChangeListener
        : public Object
        , public IViewOnLayoutChangeListener
    {
    public:
        CAR_INTERFACE_DECL();

        OnLayoutChangeListener(
            /* [in] */ StackScrollAlgorithm* host);

        // @Override
        CARAPI OnLayoutChange(
            /* [in] */ IView* v,
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom,
            /* [in] */ Int32 oldLeft,
            /* [in] */ Int32 oldTop,
            /* [in] */ Int32 oldRight,
            /* [in] */ Int32 oldBottom);

    private:
        StackScrollAlgorithm* mHost;
    };

    class Runnable1: public Runnable
    {
    public:
        Runnable1(
            /* [in] */ StackScrollAlgorithm* host,
            /* [in] */ IViewGroup* hostView);

        // @Override
        CARAPI Run();

    private:
        StackScrollAlgorithm* mHost;
        AutoPtr<IViewGroup> mHostView;
    };

public:
    CAR_INTERFACE_DECL();

    StackScrollAlgorithm(
        /* [in] */ IContext* context);

    CARAPI GetBottomStackSlowDownLength(
        /* [out] */ Int32* result);

    CARAPI ShouldScaleDimmed(
        /* [out] */ Boolean* result);

    CARAPI GetStackScrollState(
        /* [in] */ IAmbientState* ambientState,
        /* [in] */ IStackScrollState* resultState);

    CARAPI SetLayoutHeight(
        /* [in] */ Int32 layoutHeight);

    CARAPI SetTopPadding(
        /* [in] */ Int32 topPadding);

    /**
     * Update whether the device is very small, i.e. Notifications can be in both the top and the
     * bottom stack at the same time
     *
     * @param panelHeight The normal height of the panel when it's open
     */
    CARAPI UpdateIsSmallScreen(
        /* [in] */ Int32 panelHeight);

    CARAPI OnExpansionStarted(
        /* [in] */ IStackScrollState* currentState);

    CARAPI OnExpansionStopped();

    CARAPI SetIsExpanded(
        /* [in] */ Boolean isExpanded);

    CARAPI NotifyChildrenChanged(
        /* [in] */ IViewGroup* hostView);

    CARAPI SetDimmed(
        /* [in] */ Boolean dimmed);

    CARAPI OnReset(
        /* [in] */ IExpandableView* view);

private:
    CARAPI_(void) UpdatePadding(
        /* [in] */ Boolean dimmed);

    CARAPI_(void) InitConstants(
        /* [in] */ IContext* context);

    CARAPI_(void) UpdateSpeedBumpState(
        /* [in] */ StackScrollState* resultState,
        /* [in] */ StackScrollAlgorithmState* algorithmState,
        /* [in] */ Int32 speedBumpIndex);

    CARAPI_(void) UpdateClipping(
        /* [in] */ StackScrollState* resultState,
        /* [in] */ StackScrollAlgorithmState* algorithmState);

    /**
     * Updates the shadow outline and the clipping for a view.
     *
     * @param state the viewState to update
     * @param realHeight the currently applied height of the view
     * @param clipHeight the desired clip height, the rest of the view will be clipped from the top
     * @param backgroundHeight the desired background height. The shadows of the view will be
     *                         based on this height and the content will be clipped from the top
     */
    CARAPI_(void) UpdateChildClippingAndBackground(
        /* [in] */ ViewState* state,
        /* [in] */ Float realHeight,
        /* [in] */ Float clipHeight,
        /* [in] */ Float backgroundHeight);

    /**
     * Updates the dimmed, activated and hiding sensitive states of the children.
     */
    CARAPI_(void) UpdateDimmedActivatedHideSensitive(
        /* [in] */ IAmbientState* ambientState,
        /* [in] */ StackScrollState* resultState,
        /* [in] */ StackScrollAlgorithmState* algorithmState);

    /**
     * Handle the special state when views are being dragged
     */
    CARAPI_(void) HandleDraggedViews(
        /* [in] */ IAmbientState* ambientState,
        /* [in] */ StackScrollState* resultState,
        /* [in] */ StackScrollAlgorithmState* algorithmState);

    /**
     * Update the visible children on the state.
     */
    CARAPI_(void) UpdateVisibleChildren(
        /* [in] */ StackScrollState* resultState,
        /* [in] */ StackScrollAlgorithmState* state);

    /**
     * Determine the positions for the views. This is the main part of the algorithm.
     *
     * @param resultState The result state to update if a change to the properties of a child occurs
     * @param algorithmState The state in which the current pass of the algorithm is currently in
     */
    CARAPI_(void) UpdatePositionsForState(
        /* [in] */ StackScrollState* resultState,
        /* [in] */ StackScrollAlgorithmState* algorithmState);

    /**
     * Clamp the yTranslation both up and down to valid positions.
     *
     * @param childViewState the view state of the child
     * @param childHeight the height of this child
     */
    CARAPI_(void) ClampYTranslation(
        /* [in] */ ViewState* childViewState,
        /* [in] */ Int32 childHeight);

    /**
     * Clamp the yTranslation of the child down such that its end is at most on the beginning of
     * the bottom stack.
     *
     * @param childViewState the view state of the child
     * @param childHeight the height of this child
     */
    CARAPI_(void) ClampPositionToBottomStackStart(
        /* [in] */ ViewState* childViewState,
        /* [in] */ Int32 childHeight);

    /**
     * Clamp the yTranslation of the child up such that its end is at lest on the end of the top
     * stack.get
     *
     * @param childViewState the view state of the child
     * @param childHeight the height of this child
     */
    CARAPI_(void) ClampPositionToTopStackEnd(
        /* [in] */ ViewState* childViewState,
        /* [in] */ Int32 childHeight);

    CARAPI_(Int32) GetMaxAllowedChildHeight(
        /* [in] */ IView* child);

    CARAPI_(void) UpdateStateForChildTransitioningInBottom(
        /* [in] */ StackScrollAlgorithmState* algorithmState,
        /* [in] */ Float transitioningPositionStart,
        /* [in] */ Float bottomPeakStart,
        /* [in] */ Float currentYPosition,
        /* [in] */ ViewState* childViewState,
        /* [in] */ Int32 childHeight);

    CARAPI_(void) UpdateStateForChildFullyInBottomStack(
        /* [in] */ StackScrollAlgorithmState* algorithmState,
        /* [in] */ Float transitioningPositionStart,
        /* [in] */ ViewState* childViewState,
        /* [in] */ Int32 childHeight);

    CARAPI_(void) UpdateStateForTopStackChild(
        /* [in] */ StackScrollAlgorithmState* algorithmState,
        /* [in] */ Int32 numberOfElementsCompletelyIn,
        /* [in] */ Int32 i,
        /* [in] */ Int32 childHeight,
        /* [in] */ ViewState* childViewState,
        /* [in] */ Float scrollOffset);

    /**
     * Find the number of items in the top stack and update the result state if needed.
     *
     * @param resultState The result state to update if a height change of an child occurs
     * @param algorithmState The state in which the current pass of the algorithm is currently in
     */
    CARAPI_(void) FindNumberOfItemsInTopStackAndUpdateState(
        /* [in] */ StackScrollState* resultState,
        /* [in] */ StackScrollAlgorithmState* algorithmState);

    /**
     * Calculate the Z positions for all children based on the number of items in both stacks and
     * save it in the resultState
     *
     * @param resultState The result state to update the zTranslation values
     * @param algorithmState The state in which the current pass of the algorithm is currently in
     */
    CARAPI_(void) UpdateZValuesForState(
        /* [in] */ StackScrollState* resultState,
        /* [in] */ StackScrollAlgorithmState* algorithmState);

    CARAPI_(void) UpdateInnerHeight();

    CARAPI_(void) UpdateFirstChildHeightWhileExpanding(
        /* [in] */ IViewGroup* hostView);

    CARAPI_(void) UpdateFirstChildMaxSizeToMaxHeight();

    CARAPI_(Boolean) IsMaxSizeInitialized(
        /* [in] */ IExpandableView* child);

    CARAPI_(AutoPtr<IView>) FindFirstVisibleChild(
        /* [in] */ IViewGroup* container);

public:
    static const Float DIMMED_SCALE;

private:
    static const String TAG;
    static const Int32 MAX_ITEMS_IN_BOTTOM_STACK;
    static const Int32 MAX_ITEMS_IN_TOP_STACK;

    Int32 mPaddingBetweenElements;
    Int32 mCollapsedSize;
    Int32 mTopStackPeekSize;
    Int32 mBottomStackPeekSize;
    Int32 mZDistanceBetweenElements;
    Int32 mZBasicHeight;
    Int32 mRoundedRectCornerRadius;

    AutoPtr<IStackIndentationFunctor> mTopStackIndentationFunctor;
    AutoPtr<IStackIndentationFunctor> mBottomStackIndentationFunctor;

    Int32 mLayoutHeight;

    /** mLayoutHeight - mTopPadding */
    Int32 mInnerHeight;
    Int32 mTopPadding;
    AutoPtr<StackScrollAlgorithmState> mTempAlgorithmState;
    Boolean mIsExpansionChanging;
    Int32 mFirstChildMaxHeight;
    Boolean mIsExpanded;
    AutoPtr<IExpandableView> mFirstChildWhileExpanding;
    Boolean mExpandedOnStart;
    Int32 mTopStackTotalSize;
    Int32 mPaddingBetweenElementsDimmed;
    Int32 mPaddingBetweenElementsNormal;
    Int32 mBottomStackSlowDownLength;
    Int32 mTopStackSlowDownLength;
    Int32 mCollapseSecondCardPadding;
    Boolean mIsSmallScreen;
    Int32 mMaxNotificationHeight;
    Boolean mScaleDimmed;
};

} // namespace Stack
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_STACKSCROLLALGORITHM_H__
