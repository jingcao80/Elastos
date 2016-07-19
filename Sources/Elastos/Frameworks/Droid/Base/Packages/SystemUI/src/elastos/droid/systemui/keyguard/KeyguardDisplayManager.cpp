
#include "elastos/droid/systemui/keyguard/KeyguardDisplayManager.h"
#include "Elastos.Droid.View.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

//==========================================================
// KeyguardDisplayManager::MyMediaRouterSimpleCallback
//==========================================================

ECode KeyguardDisplayManager::MyMediaRouterSimpleCallback::OnRouteSelected(
    /* [in] */ IMediaRouter* router,
    /* [in] */ Int32 type,
    /* [in] */ IRouteInfo* info)
{
    if (DEBUG) Slogger::D(TAG, "onRouteSelected: type=%d, info=%s", type, TO_CSTR(info));
    return mHost->UpdateDisplays(mShowing);
}

ECode KeyguardDisplayManager::MyMediaRouterSimpleCallback::OnRouteUnselected(
    /* [in] */ IMediaRouter* router,
    /* [in] */ Int32 type,
    /* [in] */ IRouteInfo* info)
{
    if (DEBUG) Slogger::D(TAG, "onRouteUnselected: type=%d, info=%s", type, TO_CSTR(info));
    return mHost->UpdateDisplays(mShowing);
}

ECode KeyguardDisplayManager::MyMediaRouterSimpleCallback::OnRoutePresentationDisplayChanged(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IRouteInfo* info)
{
    if (DEBUG) Slogger::D(TAG, "onRoutePresentationDisplayChanged: info=%s", TO_CSTR(info));
    return mHost->UpdateDisplays(mShowing);
}

//==========================================================
// KeyguardDisplayManager::MyDialogInterfaceOnDismissListener
//==========================================================
CAR_INTERFACE_IMPL(KeyguardDisplayManager::MyDialogInterfaceOnDismissListener, Object, IDialogInterfaceOnDismissListener)

ECode KeyguardDisplayManager::MyDialogInterfaceOnDismissListener::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    mHost->mPresentation = NULL;
    return NOERROR;
}

//==========================================================
// KeyguardDisplayManager::KeyguardPresentation::MyRunnable
//==========================================================
ECode KeyguardDisplayManager::KeyguardPresentation::MyRunnable::Run()
{
    Int32 width, height;
    mClock->GetWidth(&width);
    mClock->GetHeight(&height);
    Int32 x = mMarginLeft + (Int32) (Math::Random() * (mUsableWidth - width));
    Int32 y = mMarginTop + (Int32) (Math::Random() * (mUsableHeight - height));
    mClock->SetTranslationX(x);
    mClock->SetTranslationY(y);
    return mClock->PostDelayed(mMoveTextRunnable, MOVE_CLOCK_TIMEOUT);
}

//==========================================================
// KeyguardDisplayManager::KeyguardPresentation
//==========================================================
const Int32 KeyguardDisplayManager::KeyguardPresentation::VIDEO_SAFE_REGION = 80; // Percentage of display width & height
const Int32 KeyguardDisplayManager::KeyguardPresentation::MOVE_CLOCK_TIMEOUT = 10000; // 10s

KeyguardDisplayManager::KeyguardPresentation::KeyguardPresentation()
    : mUsableWidth(0)
    , mUsableHeight(0)
    , mMarginTop(0)
    , mMarginLeft(0)
{
}

ECode KeyguardDisplayManager::KeyguardPresentation::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IDisplay* display)
{
    Presentation::constructor(context, display);

    mMoveTextRunnable = new MyRunnable(this);
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    return window->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);
}

ECode KeyguardDisplayManager::KeyguardPresentation::OnDetachedFromWindow()
{
    return mClock->RemoveCallbacks(mMoveTextRunnable);
}

ECode KeyguardDisplayManager::KeyguardPresentation::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Presentation::OnCreate(savedInstanceState);

    AutoPtr<IPoint> p;
    CPoint::New((IPoint**)&p);
    AutoPtr<IDisplay> display;
    GetDisplay((IDisplay**)&display);
    display->GetSize(p);

    Int32 x, y;
    p->GetX(&x);
    p->GetY(&y);

    mUsableWidth = VIDEO_SAFE_REGION * x/100;
    mUsableHeight = VIDEO_SAFE_REGION * y/100;
    mMarginLeft = (100 - VIDEO_SAFE_REGION) * x / 200;
    mMarginTop = (100 - VIDEO_SAFE_REGION) * y / 200;

    SetContentView(R::layout::keyguard_presentation);

    FindViewById(R::id::clock, (IView**)&mClock);

    // Avoid screen burn in
    return mClock->Post(mMoveTextRunnable);
}

//==========================================================
// KeyguardDisplayManager
//==========================================================
const String KeyguardDisplayManager::TAG("KeyguardDisplayManager");

Boolean KeyguardDisplayManager::DEBUG = TRUE;

KeyguardDisplayManager::KeyguardDisplayManager()
    : mShowing(FALSE)
{}

ECode KeyguardDisplayManager::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;

    mMediaRouterCallback = new MyMediaRouterSimpleCallback(this);
    mOnDismissListener = new MyDialogInterfaceOnDismissListener(this);

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::MEDIA_ROUTER_SERVICE, (IInterface**)&obj);
    mMediaRouter = IMediaRouter:Probe(obj);
    return NOERROR;
}

ECode KeyguardDisplayManager::Show()
{
    if (!mShowing) {
        if (DEBUG) Slogger::V(TAG, "show");
        mMediaRouter->AddCallback(IMediaRouter::ROUTE_TYPE_REMOTE_DISPLAY,
                mMediaRouterCallback, IMediaRouter::CALLBACK_FLAG_PASSIVE_DISCOVERY);
        UpdateDisplays(TRUE);
    }
    mShowing = TRUE;
    return NOERROR;
}

ECode KeyguardDisplayManager::Hide()
{
    if (mShowing) {
        if (DEBUG) Slogger::V(TAG, "hide");
        mMediaRouter->RemoveCallback(mMediaRouterCallback);
        UpdateDisplays(FALSE);
    }
    mShowing = FALSE;
    return NOERROR;
}

ECode KeyguardDisplayManager::UpdateDisplays(
    /* [in] */ Boolean showing)
{
    if (showing) {
        AutoPtr<IMediaRouterRouteInfo> route;
        mMediaRouter->GetSelectedRoute(IMediaRouter::ROUTE_TYPE_REMOTE_DISPLAY,
                (IMediaRouterRouteInfo**)&route);

        Int32 type;
        Boolean useDisplay = route != NULL
                && ((route->GetPlaybackType(&type), type) == IMediaRouterRouteInfo::PLAYBACK_TYPE_REMOTE);
        AutoPtr<IDisplay> presentationDisplay;
        if (useDisplay) {
            route->GetPresentationDisplay((IDisplay**)&presentationDisplay);
        }

        if (mPresentation != NULL)
            AutoPtr<IDisplay> display;
            mPresentation->GetDisplay((IDisplay**)&display);
            if (display.Get() != presentationDisplay.Get()) {
                if (DEBUG) Slogger::V(TAG, "Display gone: %s", TO_CSTR(display));
                mPresentation->Dismiss();
                mPresentation = NULL;
        }

        if (mPresentation == NULL && presentationDisplay != NULL) {
            if (DEBUG) Slogger::I(TAG, "Keyguard enabled on display: %s", TO_CSTR(presentationDisplay));
            mPresentation = new KeyguardPresentation(mContext, presentationDisplay);
            mPresentation->SetOnDismissListener(mOnDismissListener);
            //try {
            ECode ec = mPresentation->Show();
            //} catch (WindowManager.InvalidDisplayException ex) {
            if (ec == (ECode)E_INVALID_DISPLAY_EXCEPTION) {
                Slogger::W(TAG, "Invalid display:%d", ec);
                mPresentation = NULL;
            }
        }
    }
    else {
        if (mPresentation != NULL) {
            mPresentation->Dismiss();
            mPresentation = NULL;
        }
    }
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
