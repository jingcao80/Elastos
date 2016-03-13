
#include "elastos/droid/systemui/statusbar/stack/StackScrollAlgorithm.h"
#include "elastos/droid/systemui/statusbar/stack/PiecewiseLinearIndentationFunctor.h"
#include "../../R.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::EIID_IViewOnLayoutChangeListener;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Stack {

StackScrollAlgorithm::StackScrollAlgorithmState::StackScrollAlgorithmState()
    : mScrollY(0)
    , mItemsInTopStack(0)
    , mPartialInTop(0)
    , mScrolledPixelsTop(0)
    , mLastTopStackIndex(0)
    , mItemsInBottomStack(0)
    , mPartialInBottom(0)
{
    CArrayList::New((IArrayList**)&mVisibleChildren);
}

CAR_INTERFACE_IMPL(StackScrollAlgorithm::OnLayoutChangeListener, Object, IViewOnLayoutChangeListener);
StackScrollAlgorithm::OnLayoutChangeListener::OnLayoutChangeListener(
    /* [in] */ StackScrollAlgorithm* host)
    : mHost(host)
{}

ECode StackScrollAlgorithm::OnLayoutChangeListener::OnLayoutChange(
    /* [in] */ IView* v,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 oldLeft,
    /* [in] */ Int32 oldTop,
    /* [in] */ Int32 oldRight,
    /* [in] */ Int32 oldBottom)
{
    if (mHost->mFirstChildWhileExpanding != NULL) {
        mHost->mFirstChildMaxHeight = mHost->GetMaxAllowedChildHeight(
                IView::Probe(mHost->mFirstChildWhileExpanding));
    }
    else {
        mHost->mFirstChildMaxHeight = 0;
    }
    v->RemoveOnLayoutChangeListener(this);
    return NOERROR;
}

StackScrollAlgorithm::Runnable1::Runnable1(
    /* [in] */ StackScrollAlgorithm* host,
    /* [in] */ IViewGroup* hostView)
    : mHost(host)
    , mHostView(hostView)
{}

ECode StackScrollAlgorithm::Runnable1::Run()
{
    mHost->UpdateFirstChildHeightWhileExpanding(mHostView);
    return NOERROR;
}

const String StackScrollAlgorithm::TAG("StackScrollAlgorithm");
const Int32 StackScrollAlgorithm::MAX_ITEMS_IN_BOTTOM_STACK = 3;
const Int32 StackScrollAlgorithm::MAX_ITEMS_IN_TOP_STACK = 3;
const Float StackScrollAlgorithm::DIMMED_SCALE = 0.95f;
CAR_INTERFACE_IMPL(StackScrollAlgorithm, Object, IStackScrollAlgorithm);
StackScrollAlgorithm::StackScrollAlgorithm(
    /* [in] */ IContext* context)
    : mPaddingBetweenElements(0)
    , mCollapsedSize(0)
    , mTopStackPeekSize(0)
    , mBottomStackPeekSize(0)
    , mZDistanceBetweenElements(0)
    , mZBasicHeight(0)
    , mRoundedRectCornerRadius(0)
    , mLayoutHeight(0)
    , mInnerHeight(0)
    , mTopPadding(0)
    , mIsExpansionChanging(FALSE)
    , mFirstChildMaxHeight(0)
    , mIsExpanded(FALSE)
    , mExpandedOnStart(FALSE)
    , mTopStackTotalSize(0)
    , mPaddingBetweenElementsDimmed(0)
    , mPaddingBetweenElementsNormal(0)
    , mBottomStackSlowDownLength(0)
    , mTopStackSlowDownLength(0)
    , mCollapseSecondCardPadding(0)
    , mIsSmallScreen(FALSE)
    , mMaxNotificationHeight(0)
    , mScaleDimmed(FALSE)
{
    mTempAlgorithmState = new StackScrollAlgorithmState();
    InitConstants(context);
    UpdatePadding(FALSE);
}

void StackScrollAlgorithm::UpdatePadding(
    /* [in] */ Boolean dimmed)
{
    mPaddingBetweenElements = dimmed && mScaleDimmed
            ? mPaddingBetweenElementsDimmed
            : mPaddingBetweenElementsNormal;
    mTopStackTotalSize = mTopStackSlowDownLength + mPaddingBetweenElements
            + mTopStackPeekSize;
    mTopStackIndentationFunctor = new PiecewiseLinearIndentationFunctor(
            MAX_ITEMS_IN_TOP_STACK,
            mTopStackPeekSize,
            mTopStackTotalSize - mTopStackPeekSize,
            0.5f);

    Int32 len = 0;
    GetBottomStackSlowDownLength(&len);
    mBottomStackIndentationFunctor = new PiecewiseLinearIndentationFunctor(
            MAX_ITEMS_IN_BOTTOM_STACK,
            mBottomStackPeekSize,
            len,
            0.5f);
}

ECode StackScrollAlgorithm::GetBottomStackSlowDownLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBottomStackSlowDownLength + mPaddingBetweenElements;
    return NOERROR;
}

void StackScrollAlgorithm::InitConstants(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::notification_padding_dimmed, &mPaddingBetweenElementsDimmed);
    res->GetDimensionPixelSize(R::dimen::notification_padding, &mPaddingBetweenElementsNormal);
    res->GetDimensionPixelSize(R::dimen::notification_min_height, &mCollapsedSize);
    res->GetDimensionPixelSize(R::dimen::notification_max_height, &mMaxNotificationHeight);
    res->GetDimensionPixelSize(R::dimen::top_stack_peek_amount, &mTopStackPeekSize);
    res->GetDimensionPixelSize(R::dimen::bottom_stack_peek_amount, &mBottomStackPeekSize);
    res->GetDimensionPixelSize(R::dimen::z_distance_between_notifications, &mZDistanceBetweenElements);
    mZBasicHeight = (MAX_ITEMS_IN_BOTTOM_STACK + 1) * mZDistanceBetweenElements;
    res->GetDimensionPixelSize(R::dimen::bottom_stack_slow_down_length, &mBottomStackSlowDownLength);
    res->GetDimensionPixelSize(R::dimen::top_stack_slow_down_length, &mTopStackSlowDownLength);
    res->GetDimensionPixelSize(R::dimen::notification_material_rounded_rect_radius, &mRoundedRectCornerRadius);
    res->GetDimensionPixelSize(R::dimen::notification_collapse_second_card_padding, &mCollapseSecondCardPadding);

    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Int32 densityDpi = 0;
    dm->GetDensityDpi(&densityDpi);
    mScaleDimmed = densityDpi >= IDisplayMetrics::DENSITY_XXHIGH;
}

ECode StackScrollAlgorithm::ShouldScaleDimmed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mScaleDimmed;
    return NOERROR;
}

ECode StackScrollAlgorithm::GetStackScrollState(
    /* [in] */ IAmbientState* ambientState,
    /* [in] */ IStackScrollState* resultState)
{
    // The state of the local variables are saved in an algorithmState to easily subdivide it
    // into multiple phases.
    AutoPtr<StackScrollAlgorithmState> algorithmState = mTempAlgorithmState;

    // First we reset the view states to their default values.
    resultState->ResetViewStates();

    algorithmState->mItemsInTopStack = 0.0f;
    algorithmState->mPartialInTop = 0.0f;
    algorithmState->mLastTopStackIndex = 0;
    algorithmState->mScrolledPixelsTop = 0;
    algorithmState->mItemsInBottomStack = 0.0f;
    algorithmState->mPartialInBottom = 0.0f;
    Float bottomOverScroll = 0;
    ambientState->GetOverScrollAmount(FALSE /* onTop */, &bottomOverScroll);

    Int32 scrollY = 0;
    ambientState->GetScrollY(&scrollY);

    // Due to the overScroller, the stackscroller can have negative scroll state. This is
    // already accounted for by the top padding and doesn't need an additional adaption
    scrollY = Elastos::Core::Math::Max(0, scrollY);
    algorithmState->mScrollY = (Int32) (scrollY + mCollapsedSize + bottomOverScroll);

    UpdateVisibleChildren((StackScrollState*)resultState, algorithmState);

    // Phase 1:
    FindNumberOfItemsInTopStackAndUpdateState((StackScrollState*)resultState, algorithmState);

    // Phase 2:
    UpdatePositionsForState((StackScrollState*)resultState, algorithmState);

    // Phase 3:
    UpdateZValuesForState((StackScrollState*)resultState, algorithmState);

    HandleDraggedViews(ambientState, (StackScrollState*)resultState, algorithmState);
    UpdateDimmedActivatedHideSensitive(ambientState, (StackScrollState*)resultState, algorithmState);
    UpdateClipping((StackScrollState*)resultState, algorithmState);
    Int32 index = 0;
    ambientState->GetSpeedBumpIndex(&index);
    UpdateSpeedBumpState((StackScrollState*)resultState, algorithmState, index);
    return NOERROR;
}

void StackScrollAlgorithm::UpdateSpeedBumpState(
    /* [in] */ StackScrollState* resultState,
    /* [in] */ StackScrollAlgorithmState* algorithmState,
    /* [in] */ Int32 speedBumpIndex)
{
    Int32 childCount = 0;
    algorithmState->mVisibleChildren->GetSize(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IInterface> obj;
        algorithmState->mVisibleChildren->Get(i, (IInterface**)&obj);
        AutoPtr<IView> child = IView::Probe(obj);

        obj = NULL;
        resultState->GetViewStateForView(child, (IInterface**)&obj);
        AutoPtr<ViewState> childViewState = (ViewState*)IObject::Probe(obj);

        // The speed bump can also be gone, so equality needs to be taken when comparing
        // indices.
        childViewState->mBelowSpeedBump = speedBumpIndex != -1 && i >= speedBumpIndex;
    }
}

void StackScrollAlgorithm::UpdateClipping(
    /* [in] */ StackScrollState* resultState,
    /* [in] */ StackScrollAlgorithmState* algorithmState)
{
    Float previousNotificationEnd = 0;
    Float previousNotificationStart = 0;
    Boolean previousNotificationIsSwiped = FALSE;
    Int32 childCount = 0;
    algorithmState->mVisibleChildren->GetSize(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IInterface> obj;
        algorithmState->mVisibleChildren->Get(i, (IInterface**)&obj);
        AutoPtr<IExpandableView> child = IExpandableView::Probe(obj);

        obj = NULL;
        resultState->GetViewStateForView(IView::Probe(child), (IInterface**)&obj);
        AutoPtr<ViewState> state = (ViewState*)IObject::Probe(obj);
        Float newYTranslation = state->mYTranslation + state->mHeight * (1.f - state->mScale) / 2.f;
        Float newHeight = state->mHeight * state->mScale;
        // apply clipping and shadow
        Float newNotificationEnd = newYTranslation + newHeight;

        // In the unlocked shade we have to clip a little bit higher because of the rounded
        // corners of the notifications.
        Float clippingCorrection = state->mDimmed ? 0 : mRoundedRectCornerRadius * state->mScale;

        // When the previous notification is swiped, we don't clip the content to the
        // bottom of it.
        Float clipHeight = previousNotificationIsSwiped
                ? newHeight
                : newNotificationEnd - (previousNotificationEnd - clippingCorrection);

        UpdateChildClippingAndBackground(state, newHeight, clipHeight,
                newHeight - (previousNotificationStart - newYTranslation));

        Boolean tmp = FALSE;
        if (child->IsTransparent(&tmp), !tmp) {
            // Only update the previous values if we are not transparent,
            // otherwise we would clip to a transparent view.
            previousNotificationStart = newYTranslation + state->mClipTopAmount * state->mScale;
            previousNotificationEnd = newNotificationEnd;
            Float f = 0;
            IView::Probe(child)->GetTranslationX(&f);
            previousNotificationIsSwiped = f != 0;
        }
    }
}

void StackScrollAlgorithm::UpdateChildClippingAndBackground(
    /* [in] */ ViewState* state,
    /* [in] */ Float realHeight,
    /* [in] */ Float clipHeight,
    /* [in] */ Float backgroundHeight)
{
    if (realHeight > clipHeight) {
        // Rather overlap than create a hole.
        state->mTopOverLap = (Int32) Elastos::Core::Math::Floor((realHeight - clipHeight) / state->mScale);
    }
    else {
        state->mTopOverLap = 0;
    }
    if (realHeight > backgroundHeight) {
        // Rather overlap than create a hole.
        state->mClipTopAmount = (Int32) Elastos::Core::Math::Floor((realHeight - backgroundHeight) / state->mScale);
    }
    else {
        state->mClipTopAmount = 0;
    }
}

void StackScrollAlgorithm::UpdateDimmedActivatedHideSensitive(
    /* [in] */ IAmbientState* ambientState,
    /* [in] */ StackScrollState* resultState,
    /* [in] */ StackScrollAlgorithmState* algorithmState)
{
    Boolean dimmed = FALSE;
    ambientState->IsDimmed(&dimmed);
    Boolean dark = FALSE;
    ambientState->IsDark(&dark);
    Boolean hideSensitive = FALSE;
    ambientState->IsHideSensitive(&hideSensitive);
    AutoPtr<IActivatableNotificationView> anv;
    ambientState->GetActivatedChild((IActivatableNotificationView**)&anv);
    AutoPtr<IView> activatedChild = IView::Probe(anv);
    Int32 childCount = 0;
    algorithmState->mVisibleChildren->GetSize(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IInterface> obj;
        algorithmState->mVisibleChildren->Get(i, (IInterface**)&obj);
        AutoPtr<IView> child = IView::Probe(obj);

        obj = NULL;
        resultState->GetViewStateForView(child, (IInterface**)&obj);
        AutoPtr<ViewState> childViewState = (ViewState*)IObject::Probe(obj);
        childViewState->mDimmed = dimmed;
        childViewState->mDark = dark;
        childViewState->mHideSensitive = hideSensitive;
        Boolean isActivatedChild = activatedChild == child;
        childViewState->mScale = !mScaleDimmed || !dimmed || isActivatedChild
                ? 1.0f
                : DIMMED_SCALE;
        if (dimmed && isActivatedChild) {
            childViewState->mZTranslation += 2.0f * mZDistanceBetweenElements;
        }
    }
}

void StackScrollAlgorithm::HandleDraggedViews(
    /* [in] */ IAmbientState* ambientState,
    /* [in] */ StackScrollState* resultState,
    /* [in] */ StackScrollAlgorithmState* algorithmState)
{
    AutoPtr<IArrayList/*<View*/> draggedViews;
    ambientState->GetDraggedViews((IArrayList**)&draggedViews);

    AutoPtr<IIterator> it;
    draggedViews->GetIterator((IIterator**)&it);
    Boolean bHasMore = FALSE;
    while ((it->HasNext(&bHasMore), bHasMore)) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IView> draggedView = IView::Probe(obj);
        Int32 childIndex = 0;
        algorithmState->mVisibleChildren->IndexOf(draggedView, &childIndex);
        Int32 tmp = 0;
        if (childIndex >= 0 && childIndex < (algorithmState->mVisibleChildren->GetSize(&tmp), tmp) - 1) {
            obj = NULL;
            algorithmState->mVisibleChildren->Get(childIndex + 1, (IInterface**)&obj);
            AutoPtr<IView> nextChild = IView::Probe(obj);

            Boolean contains = FALSE;
            if (draggedViews->Contains(nextChild, &contains), contains) {
                // only if the view is not dragged itself we modify its state to be fully
                // visible
                obj = NULL;
                resultState->GetViewStateForView(nextChild, (IInterface**)&obj);
                AutoPtr<ViewState> viewState = (ViewState*)IObject::Probe(obj);
                // The child below the dragged one must be fully visible
                viewState->mAlpha = 1;
            }

            // Lets set the alpha to the one it currently has, as its currently being dragged
            obj = NULL;
            resultState->GetViewStateForView(draggedView, (IInterface**)&obj);
            AutoPtr<ViewState> viewState = (ViewState*)IObject::Probe(obj);
            // The dragged child should keep the set alpha
            draggedView->GetAlpha(&viewState->mAlpha);
        }
    }
}

void StackScrollAlgorithm::UpdateVisibleChildren(
    /* [in] */ StackScrollState* resultState,
    /* [in] */ StackScrollAlgorithmState* state)
{
    AutoPtr<IViewGroup> hostView;
    resultState->GetHostView((IViewGroup**)&hostView);
    Int32 childCount = 0;
    hostView->GetChildCount(&childCount);
    state->mVisibleChildren->Clear();
    state->mVisibleChildren->EnsureCapacity(childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> view;
        hostView->GetChildAt(i, (IView**)&view);
        AutoPtr<IExpandableView> v = IExpandableView::Probe(view);
        Int32 visibility = 0;
        if ((IView::Probe(v)->GetVisibility(&visibility), visibility) != IView::GONE) {
            AutoPtr<IInterface> obj;
            resultState->GetViewStateForView(IView::Probe(v), (IInterface**)&obj);
            AutoPtr<ViewState> viewState = (ViewState*)IObject::Probe(obj);
            state->mVisibleChildren->GetSize(&viewState->mNotGoneIndex);
            state->mVisibleChildren->Add(IView::Probe(v));
        }
    }
}

void StackScrollAlgorithm::UpdatePositionsForState(
    /* [in] */ StackScrollState* resultState,
    /* [in] */ StackScrollAlgorithmState* algorithmState)
{
    // The starting position of the bottom stack peek
    Float bottomPeekStart = mInnerHeight - mBottomStackPeekSize;

    // The position where the bottom stack starts.
    Float bottomStackStart = bottomPeekStart - mBottomStackSlowDownLength;

    // The y coordinate of the current child.
    Float currentYPosition = 0.0f;

    // How far in is the element currently transitioning into the bottom stack.
    Float yPositionInScrollView = 0.0f;

    Int32 childCount = 0;
    algorithmState->mVisibleChildren->GetSize(&childCount);
    Int32 numberOfElementsCompletelyIn = (Int32) algorithmState->mItemsInTopStack;
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IInterface> obj;
        algorithmState->mVisibleChildren->Get(i, (IInterface**)&obj);
        AutoPtr<IExpandableView> child = IExpandableView::Probe(obj);

        obj = NULL;
        resultState->GetViewStateForView(IView::Probe(child), (IInterface**)&obj);
        AutoPtr<ViewState> childViewState = (ViewState*)IObject::Probe(obj);
        childViewState->mLocation = ViewState::LOCATION_UNKNOWN;
        Int32 childHeight = GetMaxAllowedChildHeight(IView::Probe(child));
        Float yPositionInScrollViewAfterElement = yPositionInScrollView
                + childHeight
                + mPaddingBetweenElements;
        Float scrollOffset = yPositionInScrollView - algorithmState->mScrollY + mCollapsedSize;

        if (i == algorithmState->mLastTopStackIndex + 1) {
            // Normally the position of this child is the position in the regular scrollview,
            // but if the two stacks are very close to each other,
            // then have have to push it even more upwards to the position of the bottom
            // stack start.
            currentYPosition = Elastos::Core::Math::Min(scrollOffset, bottomStackStart);
        }
        childViewState->mYTranslation = currentYPosition;

        // The y position after this element
        Float nextYPosition = currentYPosition + childHeight +
                mPaddingBetweenElements;

        if (i <= algorithmState->mLastTopStackIndex) {
            // Case 1:
            // We are in the top Stack
            UpdateStateForTopStackChild(algorithmState,
                    numberOfElementsCompletelyIn, i, childHeight, childViewState, scrollOffset);
            ClampPositionToTopStackEnd(childViewState, childHeight);

            // check if we are overlapping with the bottom stack
            if (childViewState->mYTranslation + childHeight + mPaddingBetweenElements
                    >= bottomStackStart && !mIsExpansionChanging && i != 0 && mIsSmallScreen) {
                // we just collapse this element slightly
                Int32 newSize = (Int32) Elastos::Core::Math::Max(bottomStackStart - mPaddingBetweenElements -
                        childViewState->mYTranslation, (Float)mCollapsedSize);
                childViewState->mHeight = newSize;
                UpdateStateForChildTransitioningInBottom(algorithmState, bottomStackStart,
                        bottomPeekStart, childViewState->mYTranslation, childViewState,
                        childHeight);
            }
            ClampPositionToBottomStackStart(childViewState, childViewState->mHeight);
        }
        else if (nextYPosition >= bottomStackStart) {
            // Case 2:
            // We are in the bottom stack.
            if (currentYPosition >= bottomStackStart) {
                // According to the regular scroll view we are fully translated out of the
                // bottom of the screen so we are fully in the bottom stack
                UpdateStateForChildFullyInBottomStack(algorithmState,
                        bottomStackStart, childViewState, childHeight);
            }
            else {
                // According to the regular scroll view we are currently translating out of /
                // into the bottom of the screen
                UpdateStateForChildTransitioningInBottom(algorithmState,
                        bottomStackStart, bottomPeekStart, currentYPosition,
                        childViewState, childHeight);
            }
        }
        else {
            // Case 3:
            // We are in the regular scroll area.
            childViewState->mLocation = ViewState::LOCATION_MAIN_AREA;
            ClampYTranslation(childViewState, childHeight);
        }

        // The first card is always rendered.
        if (i == 0) {
            childViewState->mAlpha = 1.0f;
            childViewState->mYTranslation = Elastos::Core::Math::Max(mCollapsedSize - algorithmState->mScrollY, 0);
            if (childViewState->mYTranslation + childViewState->mHeight
                    > bottomPeekStart - mCollapseSecondCardPadding) {
                childViewState->mHeight = (Int32) Elastos::Core::Math::Max(
                        bottomPeekStart - mCollapseSecondCardPadding
                                - childViewState->mYTranslation, (Float)mCollapsedSize);
            }
            childViewState->mLocation = ViewState::LOCATION_FIRST_CARD;
        }
        if (childViewState->mLocation == ViewState::LOCATION_UNKNOWN) {
            Logger::W(TAG, "Failed to assign location for child %d", i);
        }
        currentYPosition = childViewState->mYTranslation + childHeight + mPaddingBetweenElements;
        yPositionInScrollView = yPositionInScrollViewAfterElement;

        childViewState->mYTranslation += mTopPadding;
    }
}

void StackScrollAlgorithm::ClampYTranslation(
    /* [in] */ ViewState* childViewState,
    /* [in] */ Int32 childHeight)
{
    ClampPositionToBottomStackStart(childViewState, childHeight);
    ClampPositionToTopStackEnd(childViewState, childHeight);
}

void StackScrollAlgorithm::ClampPositionToBottomStackStart(
    /* [in] */ ViewState* childViewState,
    /* [in] */ Int32 childHeight)
{
    childViewState->mYTranslation = Elastos::Core::Math::Min(childViewState->mYTranslation,
            (Float)mInnerHeight - mBottomStackPeekSize - mCollapseSecondCardPadding - childHeight);
}

void StackScrollAlgorithm::ClampPositionToTopStackEnd(
    /* [in] */ ViewState* childViewState,
    /* [in] */ Int32 childHeight)
{
    childViewState->mYTranslation = Elastos::Core::Math::Max(childViewState->mYTranslation,
            (Float)mCollapsedSize - childHeight);
}

Int32 StackScrollAlgorithm::GetMaxAllowedChildHeight(
    /* [in] */ IView* child)
{
    Int32 iv = 0;
    if (IExpandableNotificationRow::Probe(child)) {
        AutoPtr<IExpandableNotificationRow> row = IExpandableNotificationRow::Probe(child);
        row->GetIntrinsicHeight(&iv);
        return iv;
    }
    else if (IExpandableView::Probe(child)) {
        AutoPtr<IExpandableView> expandableView = IExpandableView::Probe(child);
        expandableView->GetActualHeight(&iv);
        return iv;
    }
    return child == NULL? mCollapsedSize : (child->GetHeight(&iv), iv);
}

void StackScrollAlgorithm::UpdateStateForChildTransitioningInBottom(
    /* [in] */ StackScrollAlgorithmState* algorithmState,
    /* [in] */ Float transitioningPositionStart,
    /* [in] */ Float bottomPeakStart,
    /* [in] */ Float currentYPosition,
    /* [in] */ ViewState* childViewState,
    /* [in] */ Int32 childHeight)
{
    // This is the transitioning element on top of bottom stack, calculate how far we are in.
    algorithmState->mPartialInBottom = 1.0f - (
            (transitioningPositionStart - currentYPosition) / (childHeight +
                    mPaddingBetweenElements));

    // the offset starting at the transitionPosition of the bottom stack
    Float offset = 0;
    mBottomStackIndentationFunctor->GetValue(algorithmState->mPartialInBottom, &offset);
    algorithmState->mItemsInBottomStack += algorithmState->mPartialInBottom;
    Int32 newHeight = childHeight;
    if (childHeight > mCollapsedSize && mIsSmallScreen) {
        newHeight = (Int32) Elastos::Core::Math::Max(Elastos::Core::Math::Min(transitioningPositionStart + offset -
                mPaddingBetweenElements - currentYPosition, (Float)childHeight), (Float)mCollapsedSize);
        childViewState->mHeight = newHeight;
    }
    childViewState->mYTranslation = transitioningPositionStart + offset - newHeight
            - mPaddingBetweenElements;

    // We want at least to be at the end of the top stack when collapsing
    ClampPositionToTopStackEnd(childViewState, newHeight);
    childViewState->mLocation = ViewState::LOCATION_MAIN_AREA;
}

void StackScrollAlgorithm::UpdateStateForChildFullyInBottomStack(
    /* [in] */ StackScrollAlgorithmState* algorithmState,
    /* [in] */ Float transitioningPositionStart,
    /* [in] */ ViewState* childViewState,
    /* [in] */ Int32 childHeight)
{
    Float currentYPosition;
    algorithmState->mItemsInBottomStack += 1.0f;
    if (algorithmState->mItemsInBottomStack < MAX_ITEMS_IN_BOTTOM_STACK) {
        // We are visually entering the bottom stack
        Float fv = 0;
        mBottomStackIndentationFunctor->GetValue(algorithmState->mItemsInBottomStack, &fv);
        currentYPosition = transitioningPositionStart
                + fv
                - mPaddingBetweenElements;
        childViewState->mLocation = ViewState::LOCATION_BOTTOM_STACK_PEEKING;
    }
    else {
        // we are fully inside the stack
        if (algorithmState->mItemsInBottomStack > MAX_ITEMS_IN_BOTTOM_STACK + 2) {
            childViewState->mAlpha = 0.0f;
        }
        else if (algorithmState->mItemsInBottomStack
                > MAX_ITEMS_IN_BOTTOM_STACK + 1) {
            childViewState->mAlpha = 1.0f - algorithmState->mPartialInBottom;
        }
        childViewState->mLocation = ViewState::LOCATION_BOTTOM_STACK_HIDDEN;
        currentYPosition = mInnerHeight;
    }
    childViewState->mYTranslation = currentYPosition - childHeight;
    ClampPositionToTopStackEnd(childViewState, childHeight);
}

void StackScrollAlgorithm::UpdateStateForTopStackChild(
    /* [in] */ StackScrollAlgorithmState* algorithmState,
    /* [in] */ Int32 numberOfElementsCompletelyIn,
    /* [in] */ Int32 i,
    /* [in] */ Int32 childHeight,
    /* [in] */ ViewState* childViewState,
    /* [in] */ Float scrollOffset)
{
    // First we calculate the index relative to the current stack window of size at most
    // {@link #MAX_ITEMS_IN_TOP_STACK}
    Int32 paddedIndex = i - 1
            - Elastos::Core::Math::Max(numberOfElementsCompletelyIn - MAX_ITEMS_IN_TOP_STACK, 0);
    if (paddedIndex >= 0) {
        // We are currently visually entering the top stack
        Float distanceToStack = (childHeight + mPaddingBetweenElements)
                - algorithmState->mScrolledPixelsTop;
        if (i == algorithmState->mLastTopStackIndex
                && distanceToStack > (mTopStackTotalSize + mPaddingBetweenElements)) {

            // Child is currently translating into stack but not yet inside slow down zone.
            // Handle it like the regular scrollview.
            childViewState->mYTranslation = scrollOffset;
        }
        else {
            // Apply stacking logic.
            Float numItemsBefore;
            if (i == algorithmState->mLastTopStackIndex) {
                numItemsBefore = 1.0f
                        - (distanceToStack / (mTopStackTotalSize + mPaddingBetweenElements));
            }
            else {
                numItemsBefore = algorithmState->mItemsInTopStack - i;
            }
            // The end position of the current child
            Float fv = 0;
            mTopStackIndentationFunctor->GetValue(numItemsBefore, &fv);
            Float currentChildEndY = mCollapsedSize + mTopStackTotalSize - fv;
            childViewState->mYTranslation = currentChildEndY - childHeight;
        }
        childViewState->mLocation = ViewState::LOCATION_TOP_STACK_PEEKING;
    }
    else {
        if (paddedIndex == -1) {
            childViewState->mAlpha = 1.0f - algorithmState->mPartialInTop;
        }
        else {
            // We are hidden behind the top card and faded out, so we can hide ourselves.
            childViewState->mAlpha = 0.0f;
        }
        childViewState->mYTranslation = mCollapsedSize - childHeight;
        childViewState->mLocation = ViewState::LOCATION_TOP_STACK_HIDDEN;
    }
}

void StackScrollAlgorithm::FindNumberOfItemsInTopStackAndUpdateState(
    /* [in] */ StackScrollState* resultState,
    /* [in] */ StackScrollAlgorithmState* algorithmState)
{
    // The y Position if the element would be in a regular scrollView
    Float yPositionInScrollView = 0.0f;
    Int32 childCount = 0;
    algorithmState->mVisibleChildren->GetSize(&childCount);

    // find the number of elements in the top stack.
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IInterface> obj;
        algorithmState->mVisibleChildren->Get(i, (IInterface**)&obj);
        AutoPtr<IExpandableView> child = IExpandableView::Probe(obj);

        obj = NULL;
        resultState->GetViewStateForView(IView::Probe(child), (IInterface**)&obj);
        AutoPtr<ViewState> childViewState = (ViewState*)IObject::Probe(obj);
        Int32 childHeight = GetMaxAllowedChildHeight(IView::Probe(child));
        Float yPositionInScrollViewAfterElement = yPositionInScrollView
                + childHeight
                + mPaddingBetweenElements;
        if (yPositionInScrollView < algorithmState->mScrollY) {
            if (i == 0 && algorithmState->mScrollY <= mCollapsedSize) {

                // The starting position of the bottom stack peek
                Int32 bottomPeekStart = mInnerHeight - mBottomStackPeekSize -
                        mCollapseSecondCardPadding;
                // Collapse and expand the first child while the shade is being expanded
                Float maxHeight = mIsExpansionChanging && child == mFirstChildWhileExpanding
                        ? mFirstChildMaxHeight
                        : childHeight;
                childViewState->mHeight = (Int32) Elastos::Core::Math::Max(
                    Elastos::Core::Math::Min((Float)bottomPeekStart, maxHeight), (Float)mCollapsedSize);
                algorithmState->mItemsInTopStack = 1.0f;

            }
            else if (yPositionInScrollViewAfterElement < algorithmState->mScrollY) {
                // According to the regular scroll view we are fully off screen
                algorithmState->mItemsInTopStack += 1.0f;
                if (i == 0) {
                    childViewState->mHeight = mCollapsedSize;
                }
            }
            else {
                // According to the regular scroll view we are partially off screen

                // How much did we scroll into this child
                algorithmState->mScrolledPixelsTop = algorithmState->mScrollY
                        - yPositionInScrollView;
                algorithmState->mPartialInTop = (algorithmState->mScrolledPixelsTop) / (childHeight
                        + mPaddingBetweenElements);

                // Our element can be expanded, so this can get negative
                algorithmState->mPartialInTop = Elastos::Core::Math::Max(0.0f, algorithmState->mPartialInTop);
                algorithmState->mItemsInTopStack += algorithmState->mPartialInTop;

                if (i == 0) {
                    // If it is expanded we have to collapse it to a new size
                    Float newSize = yPositionInScrollViewAfterElement
                            - mPaddingBetweenElements
                            - algorithmState->mScrollY + mCollapsedSize;
                    newSize = Elastos::Core::Math::Max((Float)mCollapsedSize, newSize);
                    algorithmState->mItemsInTopStack = 1.0f;
                    childViewState->mHeight = (Int32) newSize;
                }
                algorithmState->mLastTopStackIndex = i;
                break;
            }
        }
        else {
            algorithmState->mLastTopStackIndex = i - 1;
            // We are already past the stack so we can end the loop
            break;
        }
        yPositionInScrollView = yPositionInScrollViewAfterElement;
    }
}

void StackScrollAlgorithm::UpdateZValuesForState(
    /* [in] */ StackScrollState* resultState,
    /* [in] */ StackScrollAlgorithmState* algorithmState)
{
    Int32 childCount = 0;
    algorithmState->mVisibleChildren->GetSize(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IInterface> obj;
        algorithmState->mVisibleChildren->Get(i, (IInterface**)&obj);
        AutoPtr<IView> child = IView::Probe(obj);

        obj = NULL;
        resultState->GetViewStateForView(child, (IInterface**)&obj);
        AutoPtr<ViewState> childViewState = (ViewState*)IObject::Probe(obj);
        if (i < algorithmState->mItemsInTopStack) {
            Float stackIndex = algorithmState->mItemsInTopStack - i;
            stackIndex = Elastos::Core::Math::Min(stackIndex, (Float)MAX_ITEMS_IN_TOP_STACK + 2);
            if (i == 0 && algorithmState->mItemsInTopStack < 2.0f) {

                // We only have the top item and an additional item in the top stack,
                // Interpolate the index from 0 to 2 while the second item is
                // translating in.
                stackIndex -= 1.0f;
                if (algorithmState->mScrollY > mCollapsedSize) {

                    // Since there is a shadow treshhold, we cant just interpolate from 0 to
                    // 2 but we interpolate from 0.1f to 2.0f when scrolled in. The jump in
                    // height will not be noticable since we have padding in between.
                    stackIndex = 0.1f + stackIndex * 1.9f;
                }
            }
            childViewState->mZTranslation = mZBasicHeight
                    + stackIndex * mZDistanceBetweenElements;
        }
        else if (i > (childCount - 1 - algorithmState->mItemsInBottomStack)) {
            Float numItemsAbove = i - (childCount - 1 - algorithmState->mItemsInBottomStack);
            Float translationZ = mZBasicHeight
                    - numItemsAbove * mZDistanceBetweenElements;
            childViewState->mZTranslation = translationZ;
        }
        else {
            childViewState->mZTranslation = mZBasicHeight;
        }
    }
}

ECode StackScrollAlgorithm::SetLayoutHeight(
    /* [in] */ Int32 layoutHeight)
{
    mLayoutHeight = layoutHeight;
    UpdateInnerHeight();
    return NOERROR;
}

ECode StackScrollAlgorithm::SetTopPadding(
    /* [in] */ Int32 topPadding)
{
    mTopPadding = topPadding;
    UpdateInnerHeight();
    return NOERROR;
}

void StackScrollAlgorithm::UpdateInnerHeight()
{
    mInnerHeight = mLayoutHeight - mTopPadding;
}

ECode StackScrollAlgorithm::UpdateIsSmallScreen(
    /* [in] */ Int32 panelHeight)
{
    mIsSmallScreen = panelHeight <
            mCollapsedSize  /* top stack */
            + mBottomStackSlowDownLength + mBottomStackPeekSize /* bottom stack */
            + mMaxNotificationHeight; /* max notification height */
    return NOERROR;
}

ECode StackScrollAlgorithm::OnExpansionStarted(
    /* [in] */ IStackScrollState* currentState)
{
    mIsExpansionChanging = TRUE;
    mExpandedOnStart = mIsExpanded;
    AutoPtr<IViewGroup> hostView;
    currentState->GetHostView((IViewGroup**)&hostView);
    UpdateFirstChildHeightWhileExpanding(hostView);
    return NOERROR;
}

void StackScrollAlgorithm::UpdateFirstChildHeightWhileExpanding(
    /* [in] */ IViewGroup* hostView)
{
    AutoPtr<IView> view = FindFirstVisibleChild(hostView);
    mFirstChildWhileExpanding = IExpandableView::Probe(view);
    if (mFirstChildWhileExpanding != NULL) {
        if (mExpandedOnStart) {
            // We are collapsing the shade, so the first child can get as most as high as the
            // current height or the end value of the animation.
            assert(0 && "TODO");
            // mFirstChildMaxHeight = StackStateAnimator::GetFinalActualHeight(mFirstChildWhileExpanding);
        }
        else {
            UpdateFirstChildMaxSizeToMaxHeight();
        }
    }
    else {
        mFirstChildMaxHeight = 0;
    }
}

void StackScrollAlgorithm::UpdateFirstChildMaxSizeToMaxHeight()
{
    // We are expanding the shade, expand it to its full height.
    if (!IsMaxSizeInitialized(mFirstChildWhileExpanding)) {
        // This child was not layouted yet, wait for a layout pass
        AutoPtr<OnLayoutChangeListener> listener = new OnLayoutChangeListener(this);
        IView::Probe(mFirstChildWhileExpanding)->AddOnLayoutChangeListener(listener);
    }
    else {
        mFirstChildMaxHeight = GetMaxAllowedChildHeight(IView::Probe(mFirstChildWhileExpanding));
    }
}

Boolean StackScrollAlgorithm::IsMaxSizeInitialized(
    /* [in] */ IExpandableView* child)
{
    if (IExpandableNotificationRow::Probe(child)) {
        AutoPtr<IExpandableNotificationRow> row = IExpandableNotificationRow::Probe(child);
        Boolean tmp = FALSE;
        row->IsMaxExpandHeightInitialized(&tmp);
        return tmp;
    }
    Int32 i = 0;
    return child == NULL || (IView::Probe(child)->GetWidth(&i), i) != 0;
}

AutoPtr<IView> StackScrollAlgorithm::FindFirstVisibleChild(
    /* [in] */ IViewGroup* container)
{
    Int32 childCount = 0;
    container->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        container->GetChildAt(i, (IView**)&child);
        Int32 visibility = 0;
        if ((child->GetVisibility(&visibility), visibility) != IView::GONE) {
            return child;
        }
    }
    return NULL;
}

ECode StackScrollAlgorithm::OnExpansionStopped()
{
    mIsExpansionChanging = FALSE;
    mFirstChildWhileExpanding = NULL;
    return NOERROR;
}

ECode StackScrollAlgorithm::SetIsExpanded(
    /* [in] */ Boolean isExpanded)
{
    mIsExpanded = isExpanded;
    return NOERROR;
}

ECode StackScrollAlgorithm::NotifyChildrenChanged(
    /* [in] */ IViewGroup* hostView)
{
    if (mIsExpansionChanging) {
        AutoPtr<Runnable1> run = new Runnable1(this, hostView);
        Boolean tmp = FALSE;
        IView::Probe(hostView)->Post(run, &tmp);
    }
    return NOERROR;
}

ECode StackScrollAlgorithm::SetDimmed(
    /* [in] */ Boolean dimmed)
{
    UpdatePadding(dimmed);
    return NOERROR;
}

ECode StackScrollAlgorithm::OnReset(
    /* [in] */ IExpandableView* view)
{
    Boolean e = FALSE;
    if (IObject::Probe(view)->Equals(mFirstChildWhileExpanding, &e), e) {
        UpdateFirstChildMaxSizeToMaxHeight();
    }
    return NOERROR;
}

} // namespace Stack
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
