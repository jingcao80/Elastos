
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/TouchDelegate.h"
//#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/graphics/CRect.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::CRect;
//using Elastos::Droid::View::CViewConfigurationHelper;

namespace Elastos {
namespace Droid {
namespace View {

//========================================================================================
//              TouchDelegate::
//========================================================================================
CAR_INTERFACE_IMPL(TouchDelegate, Object, ITouchDelegate)

ECode TouchDelegate::constructor()
{
    return NOERROR;
}

ECode TouchDelegate::constructor(
    /* [in] */ IRect* bounds,
    /* [in] */ IView* delegateView)
{
    mBounds = bounds;

    AutoPtr<IContext> cxt;
    delegateView->GetContext((IContext**)&cxt);
    AutoPtr<IViewConfigurationHelper> hlp;
//    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&hlp);
    AutoPtr<IViewConfiguration> cfg;
    hlp->Get(cxt, (IViewConfiguration**)&cfg);
    cfg->GetScaledTouchSlop(&mSlop);
    CRect::New(bounds, (IRect**)&mSlopBounds);
    mSlopBounds->Inset(-mSlop, -mSlop);
    mDelegateView = delegateView;
    return NOERROR;
}

ECode TouchDelegate::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    Float fX, fY;
    event->GetX(&fX);
    event->GetY(&fY);
    Int32 x = (Int32)fX;
    Int32 y = (Int32)fY;
    Boolean sendToDelegate = FALSE;
    Boolean hit = TRUE;
    Boolean handled = FALSE;

    Int32 act = 0;
    event->GetAction(&act);
    switch (act) {
    case IMotionEvent::ACTION_DOWN:
        {
            AutoPtr<IRect> bounds = mBounds;
            Boolean bBoundsContain = FALSE;
            bounds->Contains(x, y, &bBoundsContain);
            if (bBoundsContain) {
                mDelegateTargeted = TRUE;
                sendToDelegate = TRUE;
            }
        }
        break;
    case IMotionEvent::ACTION_UP:
    case IMotionEvent::ACTION_MOVE:
        sendToDelegate = mDelegateTargeted;
        if (sendToDelegate) {
            AutoPtr<IRect> slopBounds = mSlopBounds;
            Boolean bSlopContain = FALSE;
            slopBounds->Contains(x, y, &bSlopContain);
            if (!bSlopContain) {
                hit = FALSE;
            }
        }
        break;
    case IMotionEvent::ACTION_CANCEL:
        sendToDelegate = mDelegateTargeted;
        mDelegateTargeted = FALSE;
        break;
    }
    if (sendToDelegate) {
        AutoPtr<IView> delegateView = mDelegateView;

        if (hit) {
            // Offset event coordinates to be inside the target view
            Int32 w = 0, h = 0;
            delegateView->GetWidth(&w);
            delegateView->GetHeight(&h);
            event->SetLocation(w / 2, h / 2);
        }
        else {
            // Offset event coordinates to be outside the target view (in case it does
            // something like tracking pressed state)
            Int32 slop = mSlop;
            event->SetLocation(-(slop * 2), -(slop * 2));
        }
        delegateView->DispatchTouchEvent(event, &handled);
    }
    *res = handled;
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
