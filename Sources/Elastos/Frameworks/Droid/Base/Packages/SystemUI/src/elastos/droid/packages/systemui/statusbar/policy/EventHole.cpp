#include "elastos/droid/systemui/statusbar/policy/EventHole.h"

using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::Graphics::RegionOp_DIFFERENCE;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::IOnComputeInternalInsetsListener;
using Elastos::Droid::View::EIID_IOnComputeInternalInsetsListener;
using Elastos::Droid::View::EIID_IView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const String EventHole::TAG("StatusBar.EventHole");

EventHole::EventHole()
    : mWindowVis(FALSE)
{
    mLoc[0] = mLoc[1] = 0;
}

EventHole::EventHole(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : View(context, attrs)
    , mWindowVis(FALSE)
{
    mLoc[0] = mLoc[1] = 0;
}

EventHole::EventHole(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : View(context, attrs, defStyle)
    , mWindowVis(FALSE)
{
    mLoc[0] = mLoc[1] = 0;
}

ECode EventHole::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return Init(context, attrs, 0);
}

ECode EventHole::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    // Maybe Android source code is wrong.
    return View::Init(context, attrs);
}

//@Override
void EventHole::OnWindowVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    View::OnWindowVisibilityChanged(visibility);
    mWindowVis = visibility == IView::VISIBLE;
}

ECode EventHole::OnAttachedToWindow()
{
    View::OnAttachedToWindow();
    AutoPtr<IViewTreeObserver> observer = GetViewTreeObserver();
    observer->AddOnComputeInternalInsetsListener(THIS_PROBE(IOnComputeInternalInsetsListener));
    return NOERROR;
}

ECode EventHole::OnDetachedFromWindow()
{
    View::OnDetachedFromWindow();
    AutoPtr<IViewTreeObserver> observer = GetViewTreeObserver();
    observer->RemoveOnComputeInternalInsetsListener(THIS_PROBE(IOnComputeInternalInsetsListener));
    return NOERROR;
}

ECode EventHole::OnComputeInternalInsets(
    /* [in] */ IInternalInsetsInfo* info)
{
    Boolean visible = IsShown() && mWindowVis && GetWidth() > 0 && GetHeight() > 0;

    GetLocationInWindow(&mLoc[0], &mLoc[1]);
    Int32 l = mLoc[0];
    Int32 r = l + GetWidth();
    Int32 t = mLoc[1];
    Int32 b = t + GetHeight();

    AutoPtr<IView> top = THIS_PROBE(IView);
    AutoPtr<IViewParent> parent;
    while (top->GetParent((IViewParent**)&parent) && IView::Probe(parent.Get()) != NULL) {
        top = IView::Probe(parent.Get());
        parent = NULL;
    }

    if (visible) {
        info->SetTouchableInsets(IInternalInsetsInfo::TOUCHABLE_INSETS_REGION);
        AutoPtr<IRegion> region;
        info->GetTouchableRegion((IRegion**)&region);
        Int32 w, h;
        top->GetWidth(&w);
        top->GetHeight(&h);
        Boolean bval;
        region->Set(0, 0, w, h, &bval);
        region->Op(l, t, r, b, RegionOp_DIFFERENCE, &bval);
    }
    else {
        info->SetTouchableInsets(IInternalInsetsInfo::TOUCHABLE_INSETS_FRAME);
    }
    return NOERROR;
}


}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
