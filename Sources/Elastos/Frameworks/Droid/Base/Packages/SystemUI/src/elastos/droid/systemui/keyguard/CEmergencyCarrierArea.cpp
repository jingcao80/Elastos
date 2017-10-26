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

#include "elastos/droid/systemui/keyguard/CEmergencyCarrierArea.h"
#include "R.h"

using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::EIID_IViewOnTouchListener;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_INTERFACE_IMPL(CEmergencyCarrierArea::MyOnClickListener, Object, IViewOnTouchListener)

ECode CEmergencyCarrierArea::MyOnClickListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 visible;
    IView::Probe(mHost->mCarrierText)->GetVisibility(&visible);
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
    return IView::Probe(mCarrierText)->SetVisibility(visible ? IView::VISIBLE : IView::GONE);
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
    return IView::Probe(mEmergencyButton)->SetOnTouchListener(lis);
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
