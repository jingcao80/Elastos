#include "elastos/droid/systemui/statusbar/DelegateViewHelper.h"
#include "../R.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/R.h>

using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::SystemUI::R;

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
    mTempPoint = ArrayOf<Int32>::Alloc(2);
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
    assert(0 && "TODO");
    // if (mSourceView == NULL || mDelegateView == NULL
    //     || (mBar->ShouldDisableNavbarGestures(&disable), disable)) {
    //     return FALSE;
    // }

    mSourceView->GetLocationOnScreen(mTempPoint);
    Float sourceX = (*mTempPoint)[0];
    Float sourceY = (*mTempPoint)[1];

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

    if (!mDisabled && !mPanelShowing && action == IMotionEvent::ACTION_MOVE) {
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
                assert(0 && "TODO");
                // mBar->ShowSearchPanel();
                mPanelShowing = TRUE;
                break;
            }
        }
    }

    assert(0 && "TODO");
    // if (action == IMotionEvent::ACTION_DOWN) {
    //     mBar->SetInteracting(IStatusBarManager::WINDOW_NAVIGATION_BAR, TRUE);
    // }
    // else if (action == IMotionEvent::ACTION_UP || action == IMotionEvent::ACTION_CANCEL) {
    //     mBar->SetInteracting(IStatusBarManager::WINDOW_NAVIGATION_BAR, FALSE);
    // }

    mDelegateView->GetLocationOnScreen(mTempPoint);
    Float delegateX = (*mTempPoint)[0];
    Float delegateY = (*mTempPoint)[1];

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
        AutoPtr<IResources> r;
        context->GetResources((IResources**)&r);
        Int32 value = 0;
        r->GetDimensionPixelSize(R::dimen::navigation_bar_min_swipe_distance, &value);
        mTriggerThreshhold = value;
    }
}

void DelegateViewHelper::SetInitialTouchRegion(
    /* [in] */ ArrayOf<IView*>* views)
{
    AutoPtr<IRectF> bounds;
    CRectF::New((IRectF**)&bounds);
    AutoPtr<ArrayOf<Int32> > p = ArrayOf<Int32>::Alloc(2);
    Int32 w, h;
    for (Int32 i = 0; i < views->GetLength(); i++) {
        IView* view = (*views)[i];
        if (view == NULL) continue;
        view->GetLocationOnScreen(p);
        view->GetWidth(&w);
        view->GetHeight(&h);
        if (i == 0) {
            bounds->Set((*p)[0], (*p)[1], (*p)[0] + w, (*p)[1] + h);
        } else {
            bounds->Union((*p)[0], (*p)[1], (*p)[0] + w, (*p)[1] + h);
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

void DelegateViewHelper::SetDisabled(
    /* [in] */ Boolean disabled)
{
    mDisabled = disabled;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
