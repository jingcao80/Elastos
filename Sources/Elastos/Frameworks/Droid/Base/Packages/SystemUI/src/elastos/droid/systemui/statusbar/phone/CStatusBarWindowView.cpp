
#include "elastos/droid/systemui/statusbar/phone/CStatusBarWindowView.h"
#include "elastos/droid/systemui/statusbar/BaseStatusBar.h"
#include "../R.h"
#include "Elastos.Droid.Media.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Graphics::IXfermode;
using Elastos::Droid::Graphics::PaintStyle_STROKE;
using Elastos::Droid::Graphics::PorterDuffMode_SRC;
using Elastos::Droid::Media::Session::IMediaSessionLegacyHelper;
using Elastos::Droid::Media::Session::IMediaSessionLegacyHelperHelper;
using Elastos::Droid::Media::Session::CMediaSessionLegacyHelperHelper;
using Elastos::Droid::SystemUI::StatusBar::BaseStatusBar;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::CWindowManagerGlobalHelper;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewRootImpl;
using Elastos::Droid::View::IWindowManagerGlobal;
using Elastos::Droid::View::IWindowManagerGlobalHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

const String CStatusBarWindowView::TAG("StatusBarWindowView");
const Boolean CStatusBarWindowView::DEBUG = BaseStatusBar::DEBUG;

CAR_INTERFACE_IMPL(CStatusBarWindowView, FrameLayout, IStatusBarWindowView)

CAR_OBJECT_IMPL(CStatusBarWindowView)

ECode CStatusBarWindowView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FrameLayout::constructor(context, attrs);
    CPaint::New((IPaint**)&mTransparentSrcPaint);
    SetMotionEventSplittingEnabled(FALSE);
    mTransparentSrcPaint->SetColor(0);
    AutoPtr<IXfermode> mode;
    CPorterDuffXfermode::New(PorterDuffMode_SRC, (IXfermode**)&mode);
    mTransparentSrcPaint->SetXfermode(mode);
    return NOERROR;
}

ECode CStatusBarWindowView::FitSystemWindows(
    /* [in] */ IRect* insets,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    if (GetFitsSystemWindows(&tmp), tmp) {
        Int32 l = 0, t = 0, r = 0, b = 0;
        Int32 il = 0, it = 0, ir = 0, ib = 0;
        insets->Get(&il, &it, &ir, &ib);
        GetPadding(&l, &t, &r, &b);
        Boolean changed = il != l || it != t || ir != r || ib != b;
        if (changed) {
            SetPadding(il, it, ir, 0);
        }
        insets->SetLeft(0);
        insets->SetTop(0);
        insets->SetRight(0);
    }
    else {
        Int32 l = 0, t = 0, r = 0, b = 0;
        GetPadding(&l, &t, &r, &b);
        Boolean changed = l != 0 || r != 0 || t != 0 || b != 0;
        if (changed) {
            SetPadding(0, 0, 0, 0);
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CStatusBarWindowView::OnAttachedToWindow()
{
    FrameLayout::OnAttachedToWindow();

    AutoPtr<IView> view;
    FindViewById(R::id::notification_stack_scroller, (IView**)&view);
    mStackScrollLayout = INotificationStackScrollLayout::Probe(view);

    view = NULL;
    FindViewById(R::id::notification_panel, (IView**)&view);
    mNotificationPanel = INotificationPanelView::Probe(view);

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    mDragDownHelper = new DragDownHelper(ctx, this,
        IExpandHelperCallback::Probe(mStackScrollLayout), IDragDownCallback::Probe(mService));
    FindViewById(R::id::brightness_mirror, (IView**)&mBrightnessMirror);

    // We really need to be able to animate while window animations are going on
    // so that activities may be started asynchronously from panel animations
    AutoPtr<IViewRootImpl> root;
    GetViewRootImpl((IViewRootImpl**)&root);
    if (root != NULL) {
        root->SetDrawDuringWindowsAnimating(TRUE);
    }

    // We need to ensure that our window doesn't suffer from overdraw which would normally
    // occur if our window is translucent. Since we are drawing the whole window anyway with
    // the scrim, we don't need the window to be cleared in the beginning.
    Boolean tmp = FALSE;
    if (mService->IsScrimSrcModeEnabled(&tmp), tmp) {
        AutoPtr<IBinder> windowToken;
        GetWindowToken((IBinder**)&windowToken);
        AutoPtr<IViewGroupLayoutParams> vp;
        GetLayoutParams((IViewGroupLayoutParams**)&vp);
        AutoPtr<IWindowManagerLayoutParams> lp = IWindowManagerLayoutParams::Probe(vp);
        lp->SetToken(windowToken);
        SetLayoutParams(vp);

        AutoPtr<IWindowManagerGlobalHelper> helper;
        CWindowManagerGlobalHelper::AcquireSingleton((IWindowManagerGlobalHelper**)&helper);
        AutoPtr<IWindowManagerGlobal> wg;
        helper->GetInstance((IWindowManagerGlobal**)&wg);
        wg->ChangeCanvasOpacity(windowToken, TRUE);
        SetWillNotDraw(FALSE);
    }
    else {
        SetWillNotDraw(!DEBUG);
    }
    return NOERROR;
}

ECode CStatusBarWindowView::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 action = 0, code = 0;
    event->GetAction(&action);
    Boolean down = action == IKeyEvent::ACTION_DOWN;
    switch (event->GetKeyCode(&code), code) {
        case IKeyEvent::KEYCODE_BACK:
            if (!down) {
                Boolean tmp = FALSE;
                mService->OnBackPressed(&tmp);
            }
            *result = TRUE;
            return NOERROR;
        case IKeyEvent::KEYCODE_MENU:
            if (!down) {
                return mService->OnMenuPressed(result);
            }
        case IKeyEvent::KEYCODE_SPACE:
            if (!down) {
                return mService->OnSpacePressed(result);
            }
            break;
        case IKeyEvent::KEYCODE_VOLUME_DOWN:
        case IKeyEvent::KEYCODE_VOLUME_UP: {
            Boolean tmp = FALSE;
            if (mService->IsDozing(&tmp), tmp) {
                AutoPtr<IMediaSessionLegacyHelperHelper> helper;
                CMediaSessionLegacyHelperHelper::AcquireSingleton((IMediaSessionLegacyHelperHelper**)&helper);
                AutoPtr<IMediaSessionLegacyHelper> ms;
                helper->GetHelper(mContext, (IMediaSessionLegacyHelper**)&ms);
                ms->SendVolumeKeyEvent(event, TRUE);
                *result = TRUE;
                return NOERROR;
            }
            break;
        }
    }

    Boolean tmp = FALSE;
    if (mService->InterceptMediaKey(event, &tmp), tmp) {
        *result = TRUE;
        return NOERROR;
    }
    return FrameLayout::DispatchKeyEvent(event, result);
}

ECode CStatusBarWindowView::DispatchTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 v = 0;
    if (mBrightnessMirror != NULL && (mBrightnessMirror->GetVisibility(&v), v) == IView::VISIBLE) {
        // Disallow new pointers while the brightness mirror is visible. This is so that you
        // can't touch anything other than the brightness slider while the mirror is showing
        // and the rest of the panel is transparent.
        Int32 masked = 0;
        if ((ev->GetActionMasked(&masked), masked) == IMotionEvent::ACTION_POINTER_DOWN) {
            *result = FALSE;
            return NOERROR;
        }
    }
    return FrameLayout::DispatchTouchEvent(ev, result);
}

ECode CStatusBarWindowView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean intercept = FALSE;
    Boolean tmp = FALSE;
    Int32 v = 0, state = 0;
    if ((IPanelView::Probe(mNotificationPanel)->IsFullyExpanded(&tmp), tmp)
            && (IView::Probe(mStackScrollLayout)->GetVisibility(&v), v) == IView::VISIBLE
            && (mService->GetBarState(&state), state) == IStatusBarState::KEYGUARD
            && (IBaseStatusBar::Probe(mService)->IsBouncerShowing(&tmp), !tmp)) {
        mDragDownHelper->OnInterceptTouchEvent(ev, &intercept);
        // wake up on a touch down event, if dozing
        if ((ev->GetActionMasked(&v), v) == IMotionEvent::ACTION_DOWN) {
            Int64 t = 0;
            IInputEvent::Probe(ev)->GetEventTime(&t);
            mService->WakeUpIfDozing(t, TRUE);
        }
    }
    if (!intercept) {
        FrameLayout::OnInterceptTouchEvent(ev, &tmp);
    }
    if (intercept) {
        AutoPtr<IMotionEvent> cancellation;
        AutoPtr<IMotionEventHelper> helper;
        CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
        helper->Obtain(ev, (IMotionEvent**)&cancellation);
        cancellation->SetAction(IMotionEvent::ACTION_CANCEL);
        IViewGroup::Probe(mStackScrollLayout)->OnInterceptTouchEvent(cancellation, &tmp);
        IViewGroup::Probe(mNotificationPanel)->OnInterceptTouchEvent(cancellation, &tmp);
        IInputEvent::Probe(cancellation)->Recycle();
    }
    *result = intercept;
    return NOERROR;
}

ECode CStatusBarWindowView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean handled = FALSE, tmp = FALSE;
    Int32 v = 0;
    if ((mService->GetBarState(&v), v) == IStatusBarState::KEYGUARD && (mService->IsQsExpanded(&tmp), !tmp)) {
        mDragDownHelper->OnTouchEvent(ev, &handled);
    }
    if (!handled) {
        FrameLayout::OnTouchEvent(ev, &handled);
    }
    Int32 action = 0;
    ev->GetAction(&action);
    if (!handled && (action == IMotionEvent::ACTION_UP || action == IMotionEvent::ACTION_CANCEL)) {
        IBaseStatusBar::Probe(mService)->SetInteracting(IStatusBarManager::WINDOW_STATUS_BAR, FALSE);
    }
    *result = handled;
    return NOERROR;
}

void CStatusBarWindowView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    FrameLayout::OnDraw(canvas);
    Boolean tmp = FALSE;
    if (mService->IsScrimSrcModeEnabled(&tmp), tmp) {
        // We need to ensure that our window is always drawn fully even when we have paddings,
        // since we simulate it to be opaque.
        Int32 h = 0, w = 0, pb = 0, pr = 0, pt = 0, pl = 0;
        GetHeight(&h);
        GetWidth(&w);
        GetPadding(&pl, &pt, &pr, &pb);
        Int32 paddedBottom = h - pb;
        Int32 paddedRight = w - pr;
        if (pt != 0) {
            canvas->DrawRect(0, 0, w, pt, mTransparentSrcPaint);
        }
        if (pb != 0) {
            canvas->DrawRect(0, paddedBottom, w, h, mTransparentSrcPaint);
        }
        if (pl != 0) {
            canvas->DrawRect(0, pt, pl, paddedBottom,
                    mTransparentSrcPaint);
        }
        if (pr != 0) {
            canvas->DrawRect(paddedRight, pt, w, paddedBottom,
                    mTransparentSrcPaint);
        }
    }
    // if (DEBUG) {
    //     AutoPtr<IPaint> pt;
    //     CPaint::New((IPaint**)&pt);
    //     pt->SetColor(0x80FFFF00);
    //     pt->SetStrokeWidth(12.0f);
    //     pt->SetStyle(PaintStyle_STROKE);
    //     Int32 w = 0, h = 0;
    //     canvas->GetWidth(&w);
    //     canvas->GetHeight(&h);
    //     canvas->DrawRect(0, 0, w, h, pt);
    // }
}

ECode CStatusBarWindowView::CancelExpandHelper()
{
    if (mStackScrollLayout != NULL) {
        mStackScrollLayout->CancelExpandHelper();
    }
    return NOERROR;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
