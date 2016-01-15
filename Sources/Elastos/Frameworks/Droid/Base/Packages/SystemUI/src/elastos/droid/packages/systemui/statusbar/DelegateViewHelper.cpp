#include "elastos/droid/systemui/statusbar/DelegateViewHelper.h"
#include "elastos/droid/R.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/systemui/SystemUIR.h"

using Elastos::Droid::R;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::SystemUI::SystemUIR;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

DelegateViewHelper::DelegateViewHelper(
    /* [in] */ IView* sourceView)
    : mTriggerThreshhold(0)
    , mPanelShowing(FALSE)
    , mStarted(0)
    , mSwapXY(0)
{
    mTempPoint[0] = mTempPoint[1] = 0;
    mDownPoint[0] = mDownPoint[1] = 0.0;
    CRectF::New((IRectF**)&mInitialTouch);

    SetSourceView(sourceView);
}

void DelegateViewHelper::SetDelegateView(
    /* [in] */ IView* view)
{
    mDelegateView = view;
}

void DelegateViewHelper::SetBar(
   /* [in] */ IBaseStatusBar* phoneStatusBar)
{
    mBar = phoneStatusBar;
}

Boolean DelegateViewHelper::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    Boolean disable = FALSE;
    if (mSourceView == NULL || mDelegateView == NULL
        || (mBar->ShouldDisableNavbarGestures(&disable), disable)) {
        return FALSE;
    }

    mSourceView->GetLocationOnScreen(&mTempPoint[0], &mTempPoint[1]);
    Float sourceX = mTempPoint[0];
    Float sourceY = mTempPoint[1];

    Int32 action;
    event->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN: {
            Int32 visibility;
            mDelegateView->GetVisibility(&visibility);
            mPanelShowing = visibility == IView::VISIBLE;
            event->GetX(&mDownPoint[0]);
            event->GetY(&mDownPoint[1]);
            mInitialTouch->Contains(mDownPoint[0] + sourceX, mDownPoint[1] + sourceY, &mStarted);
        }
        break;
    }

    if (!mStarted) {
        return FALSE;
    }

    if (!mPanelShowing && action == IMotionEvent::ACTION_MOVE) {
        Int32 historySize;
        event->GetHistorySize(&historySize);
        Float x, y;
        for (int k = 0; k < historySize + 1; k++) {
            if (k < historySize) {
                event->GetHistoricalX(k, &x);
                event->GetHistoricalY(k, &y);
            }
            else {
                event->GetX(&x);
                event->GetY(&y);
            }

            Float distance = mSwapXY ? (mDownPoint[0] - x) : (mDownPoint[1] - y);
            if (distance > mTriggerThreshhold) {
//TODO                mBar->ShowSearchPanel();
                mPanelShowing = TRUE;
                break;
            }
        }
    }

    mDelegateView->GetLocationOnScreen(&mTempPoint[0], &mTempPoint[1]);
    Float delegateX = mTempPoint[0];
    Float delegateY = mTempPoint[1];

    Float deltaX = sourceX - delegateX;
    Float deltaY = sourceY - delegateY;
    event->OffsetLocation(deltaX, deltaY);
    Boolean result;
    mDelegateView->DispatchTouchEvent(event, &result);
    event->OffsetLocation(-deltaX, -deltaY);
    return mPanelShowing;
}

void DelegateViewHelper::SetSourceView(
    /* [in] */ IView* view)
{
    mSourceView = view;
    if (mSourceView != NULL) {
        AutoPtr<IContext> context;
        mSourceView->GetContext((IContext**)&context);
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        res->GetDimension(SystemUIR::dimen::navbar_search_up_threshhold, &mTriggerThreshhold);
    }
}

void DelegateViewHelper::SetInitialTouchRegion(
    /* [in] */ ArrayOf<IView*>* views)
{
    AutoPtr<IRectF> bounds;
    CRectF::New((IRectF**)&bounds);
    Int32 p[2];
    Int32 w, h;
    for (Int32 i = 0; i < views->GetLength(); i++) {
        IView* view = (*views)[i];
        if (view == NULL) continue;
        view->GetLocationOnScreen(&p[0], &p[1]);
        view->GetWidth(&w);
        view->GetHeight(&h);
        if (i == 0) {
            bounds->Set(p[0], p[1], p[0] + w, p[1] + h);
        } else {
            bounds->Union(p[0], p[1], p[0] + w, p[1] + h);
        }
    }

    Float l, t, r, b;
    bounds->Get(&l, &t, &r, &b);
    mInitialTouch->Set(l, t, r, b);
}

void DelegateViewHelper::SetSwapXY(
    /* [in] */ Boolean swap)
{
    mSwapXY = swap;
}

}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
