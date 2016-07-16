
#include "elastos/droid/systemui/keyguard/CEmergencyCarrierArea.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_INTERFACE_IMPL(CEmergencyCarrierArea::MyOnClickListener, Object, IViewOnTouchListener)

CEmergencyCarrierArea::MyOnClickListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 visible;
    mHost->mCarrierText->GetVisibility(&visible);
    if (visible != IView::VISIBLE)  {
        *result = FALSE;
        return NOERROR;
    }

    Int32 action;
    event->GetAction(&action);
    switch(action) {
        case IMotionEvent::ACTION_DOWN:
        {
            AutoPtr<IViewPropertyAnimator> animator;
            IView::Probe(mHost->mCarrierText)->Animate((IViewPropertyAnimator**)&animator);
            animator->Alpha(0);
            break;
        }
        case IMotionEvent::ACTION_UP:
        {
            AutoPtr<IViewPropertyAnimator> animator;
            IView::Probe(mHost->mCarrierText)->Animate((IViewPropertyAnimator**)&animator);
            animator->Alpha(1);
            break;
        }
    }
    *result = FALSE;
    return NOERROR;
}

CAR_OBJECT_IMPL(CEmergencyCarrierArea)

CAR_INTERFACE_IMPL(CEmergencyCarrierArea, AlphaOptimizedLinearLayout, IEmergencyCarrierArea)

ECode CEmergencyCarrierArea::constructor(
    /* [in] */ IContext* context)
{
    return AlphaOptimizedLinearLayout::constructor(context);
}

ECode CEmergencyCarrierArea::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return AlphaOptimizedLinearLayout::constructor(context, attrs);
}

ECode CEmergencyCarrierArea::SetCarrierTextVisible(
    /* [in] */ Boolean visible)
{
    return mCarrierText->SetVisibility(visible ? IView::VISIBLE : IView::GONE);
}

ECode CEmergencyCarrierArea::OnFinishInflate()
{
    AlphaOptimizedLinearLayout::OnFinishInflate();

    AutoPtr<IView> view;
    FindViewById(R::id::carrier_text, (IView**)&view);
    mCarrierText = ICarrierText::Probe(view);
    AutoPtr<IView> view2;
    FindViewById(R::id::emergency_call_button, (IView**)&view2);
    mEmergencyButton = IEmergencyButton::Probe(view2);

    // The emergency button overlaps the carrier text, only noticeable when highlighted.
    // So temporarily hide the carrier text while the emergency button is pressed.
    AutoPtr<IViewOnTouchListener> lis = new MyOnClickListener(this);
    return mEmergencyButton->SetOnTouchListener(lis);
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
