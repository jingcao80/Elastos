
#include "elastos/droid/systemui/statusbar/stack/StackScrollState.h"
#include "../../R.h"
#include "Elastos.Droid.Content.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CRect;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Stack {

const Int32 StackScrollState::ViewState::LOCATION_UNKNOWN = 0x00;
const Int32 StackScrollState::ViewState::LOCATION_FIRST_CARD = 0x01;
const Int32 StackScrollState::ViewState::LOCATION_TOP_STACK_HIDDEN = 0x02;
const Int32 StackScrollState::ViewState::LOCATION_TOP_STACK_PEEKING = 0x04;
const Int32 StackScrollState::ViewState::LOCATION_MAIN_AREA = 0x08;
const Int32 StackScrollState::ViewState::LOCATION_BOTTOM_STACK_PEEKING = 0x10;
const Int32 StackScrollState::ViewState::LOCATION_BOTTOM_STACK_HIDDEN = 0x20;
StackScrollState::ViewState::ViewState()
    : mAlpha(0)
    , mYTranslation(0)
    , mZTranslation(0)
    , mHeight(0)
    , mGone(FALSE)
    , mScale(0)
    , mDimmed(FALSE)
    , mDark(FALSE)
    , mHideSensitive(FALSE)
    , mBelowSpeedBump(FALSE)
    , mClipTopAmount(0)
    , mTopOverLap(0)
    , mNotGoneIndex(0)
    , mLocation(0)
{}

const String StackScrollState::CHILD_NOT_FOUND_TAG("StackScrollStateNoSuchChild");
CAR_INTERFACE_IMPL(StackScrollState, Object, IStackScrollState);
StackScrollState::StackScrollState(
    /* [in] */ IViewGroup* hostView)
    : mClearAllTopPadding(0)
{
    CRect::New((IRect**)&mClipRect);
    mHostView = hostView;
    CHashMap::New((IMap**)&mStateMap);

    AutoPtr<IContext> context;
    IView::Probe(hostView)->GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::clear_all_padding_top, &mClearAllTopPadding);
}

ECode StackScrollState::GetHostView(
    /* [out] */ IViewGroup** vg)
{
    VALIDATE_NOT_NULL(vg);
    *vg = mHostView;
    REFCOUNT_ADD(*vg);
    return NOERROR;
}

ECode StackScrollState::ResetViewStates()
{
    Int32 numChildren = 0;
    mHostView->GetChildCount(&numChildren);
    for (Int32 i = 0; i < numChildren; i++) {
        AutoPtr<IView> v;
        mHostView->GetChildAt(i, (IView**)&v);
        AutoPtr<IExpandableView> child = IExpandableView::Probe(v);

        AutoPtr<IInterface> obj;
        mStateMap->Get(IView::Probe(child), (IInterface**)&obj);
        AutoPtr<ViewState> viewState = (ViewState*)IObject::Probe(obj);
        if (viewState == NULL) {
            viewState = new ViewState();
            mStateMap->Put(IView::Probe(child), (IObject*)viewState->Probe(EIID_IObject));
        }
        // initialize with the default values of the view
        child->GetIntrinsicHeight(&viewState->mHeight);
        Int32 visibility = 0;
        IView::Probe(child)->GetVisibility(&visibility);
        viewState->mGone = visibility == IView::GONE;
        viewState->mAlpha = 1;
        viewState->mNotGoneIndex = -1;
    }
    return NOERROR;
}

ECode StackScrollState::GetViewStateForView(
    /* [in] */ IView* requestedView,
    /* [out] */ IInterface** vs)
{
    VALIDATE_NOT_NULL(vs);
    return mStateMap->Get(requestedView, vs);
}

ECode StackScrollState::RemoveViewStateForView(
    /* [in] */ IView* child)
{
    mStateMap->Remove(child);
    return NOERROR;
}

ECode StackScrollState::Apply()
{
    Int32 numChildren = 0;
    mHostView->GetChildCount(&numChildren);
    for (Int32 i = 0; i < numChildren; i++) {
        AutoPtr<IView> view;
        mHostView->GetChildAt(i, (IView**)&view);
        AutoPtr<IExpandableView> child = IExpandableView::Probe(view);

        AutoPtr<IInterface> obj;
        mStateMap->Get(IView::Probe(child), (IInterface**)&obj);
        AutoPtr<ViewState> state = (ViewState*)IObject::Probe(obj);
        if (state == NULL) {
            Logger::W(CHILD_NOT_FOUND_TAG, "No child state was found when applying this state to the hostView");
            continue;
        }
        if (!state->mGone) {
            Float alpha = 0;
            IView::Probe(child)->GetAlpha(&alpha);
            Float yTranslation = 0;
            IView::Probe(child)->GetTranslationY(&yTranslation);
            Float xTranslation = 0;
            IView::Probe(child)->GetTranslationX(&xTranslation);
            Float zTranslation = 0;
            IView::Probe(child)->GetTranslationZ(&zTranslation);
            Float scale = 0;
            IView::Probe(child)->GetScaleX(&scale);
            Int32 height = 0;
            child->GetActualHeight(&height);
            Float newAlpha = state->mAlpha;
            Float newYTranslation = state->mYTranslation;
            Float newZTranslation = state->mZTranslation;
            Float newScale = state->mScale;
            Int32 newHeight = state->mHeight;
            Boolean becomesInvisible = newAlpha == 0.0f;
            if (alpha != newAlpha && xTranslation == 0) {
                // apply layer type
                Boolean becomesFullyVisible = newAlpha == 1.0f;
                Boolean newLayerTypeIsHardware = !becomesInvisible && !becomesFullyVisible;
                Int32 layerType = 0;
                IView::Probe(child)->GetLayerType(&layerType);
                Int32 newLayerType = newLayerTypeIsHardware
                        ? IView::LAYER_TYPE_HARDWARE
                        : IView::LAYER_TYPE_NONE;
                if (layerType != newLayerType) {
                    IView::Probe(child)->SetLayerType(newLayerType, NULL);
                }

                // apply alpha
                if (!becomesInvisible) {
                    IView::Probe(child)->SetAlpha(newAlpha);
                }
            }

            // apply visibility
            Int32 oldVisibility = 0;
            IView::Probe(child)->GetVisibility(&oldVisibility);
            Int32 newVisibility = becomesInvisible ? IView::INVISIBLE : IView::VISIBLE;
            if (newVisibility != oldVisibility) {
                IView::Probe(child)->SetVisibility(newVisibility);
            }

            // apply yTranslation
            if (yTranslation != newYTranslation) {
                IView::Probe(child)->SetTranslationY(newYTranslation);
            }

            // apply zTranslation
            if (zTranslation != newZTranslation) {
                IView::Probe(child)->SetTranslationZ(newZTranslation);
            }

            // apply scale
            if (scale != newScale) {
                IView::Probe(child)->SetScaleX(newScale);
                IView::Probe(child)->SetScaleY(newScale);
            }

            // apply height
            if (height != newHeight) {
                child->SetActualHeight(newHeight, FALSE /* notifyListeners */);
            }

            // apply dimming
            child->SetDimmed(state->mDimmed, FALSE /* animate */);

            // apply dark
            child->SetDark(state->mDark, FALSE /* animate */);

            // apply hiding sensitive
            child->SetHideSensitive(
                    state->mHideSensitive, FALSE /* animated */, 0 /* delay */, 0 /* duration */);

            // apply speed bump state
            child->SetBelowSpeedBump(state->mBelowSpeedBump);

            // apply clipping
            Int32 iv = 0;
            child->GetClipTopAmount(&iv);
            Float oldClipTopAmount = iv;
            if (oldClipTopAmount != state->mClipTopAmount) {
                child->SetClipTopAmount(state->mClipTopAmount);
            }
            UpdateChildClip(IView::Probe(child), newHeight, state->mTopOverLap);

            if(ISpeedBumpView::Probe(child)) {
                PerformSpeedBumpAnimation(i, ISpeedBumpView::Probe(child),
                    state->Probe(EIID_IInterface), 0);
            }
            else if (IDismissView::Probe(child)) {
                AutoPtr<IDismissView> dismissView = IDismissView::Probe(child);
                Boolean visible = state->mTopOverLap < mClearAllTopPadding;
                Boolean tmp = FALSE;
                IStackScrollerDecorView::Probe(dismissView)->PerformVisibilityAnimation(
                        visible && (IStackScrollerDecorView::Probe(dismissView)->WillBeGone(&tmp), !tmp));
            }
            else if (IEmptyShadeView::Probe(child)) {
                AutoPtr<IEmptyShadeView> emptyShadeView = IEmptyShadeView::Probe(child);
                Boolean visible = state->mTopOverLap <= 0;
                Boolean tmp = FALSE;
                IStackScrollerDecorView::Probe(emptyShadeView)->PerformVisibilityAnimation(
                        visible && (IStackScrollerDecorView::Probe(emptyShadeView)->WillBeGone(&tmp), !tmp));
            }
        }
    }
    return NOERROR;
}

void StackScrollState::UpdateChildClip(
    /* [in] */ IView* child,
    /* [in] */ Int32 height,
    /* [in] */ Int32 clipInset)
{
    Int32 w = 0;
    child->GetWidth(&w);
    mClipRect->Set(0,
            clipInset,
            w,
            height);
    child->SetClipBounds(mClipRect);
}

ECode StackScrollState::PerformSpeedBumpAnimation(
    /* [in] */ Int32 i,
    /* [in] */ ISpeedBumpView* speedBump,
    /* [in] */ IInterface* _state,
    /* [in] */ Int64 delay)
{
    AutoPtr<IView> nextChild = GetNextChildNotGone(i);
    if (nextChild != NULL) {
        ViewState* state = (ViewState*)IObject::Probe(_state);
        Float lineEnd = state->mYTranslation + state->mHeight / 2;

        AutoPtr<IInterface> obj;
        GetViewStateForView(nextChild, (IInterface**)&obj);
        AutoPtr<ViewState> nextState = (ViewState*)IObject::Probe(obj);
        Boolean startIsAboveNext = nextState->mYTranslation > lineEnd;
        speedBump->AnimateDivider(startIsAboveNext, delay, NULL /* onFinishedRunnable */);
    }
    return NOERROR;
}

AutoPtr<IView> StackScrollState::GetNextChildNotGone(
    /* [in] */ Int32 childIndex)
{
    Int32 childCount = 0;
    mHostView->GetChildCount(&childCount);
    for (Int32 i = childIndex + 1; i < childCount; i++) {
        AutoPtr<IView> child;
        mHostView->GetChildAt(i, (IView**)&child);
        Int32 v = 0;
        if ((child->GetVisibility(&v), v) != IView::GONE) {
            return child;
        }
    }
    return NULL;
}

} // namespace Stack
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
