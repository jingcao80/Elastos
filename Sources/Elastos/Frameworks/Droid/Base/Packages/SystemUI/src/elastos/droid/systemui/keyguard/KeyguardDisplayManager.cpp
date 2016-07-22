
#include "elastos/droid/systemui/keyguard/KeyguardDisplayManager.h"
#include <elastos/utility/logging/Slogger.h>
#include "../R.h"
#include "Elastos.Droid.Graphics.h"
#include <elastos/core/Math.h>

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::View::IWindow;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

static const String DTAG("KeyguardDisplayManager");
Boolean KeyguardDisplayManager::DEBUG = TRUE;

//==========================================================
// KeyguardDisplayManager::MyMediaRouterSimpleCallback
//==========================================================

ECode KeyguardDisplayManager::MyMediaRouterSimpleCallback::OnRouteSelected(
    /* [in] */ IMediaRouter* router,
    /* [in] */ Int32 type,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    if (DEBUG) Slogger::D(DTAG, "onRouteSelected: type=%d, info=%s", type, TO_CSTR(info));
    return mHost->UpdateDisplays(mHost->mShowing);
}

ECode KeyguardDisplayManager::MyMediaRouterSimpleCallback::OnRouteUnselected(
    /* [in] */ IMediaRouter* router,
    /* [in] */ Int32 type,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    if (DEBUG) Slogger::D(DTAG, "onRouteUnselected: type=%d, info=%s", type, TO_CSTR(info));
    return mHost->UpdateDisplays(mHost->mShowing);
}

ECode KeyguardDisplayManager::MyMediaRouterSimpleCallback::OnRoutePresentationDisplayChanged(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    if (DEBUG) Slogger::D(DTAG, "onRoutePresentationDisplayChanged: info=%s", TO_CSTR(info));
    return mHost->UpdateDisplays(mHost->mShowing);
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
    using Elastos::Core::Math;
    Int32 width, height;
    mHost->mClock->GetWidth(&width);
    mHost->mClock->GetHeight(&height);
    Int32 x = mHost->mMarginLeft + (Int32) (Math::Random() * (mHost->mUsableWidth - width));
    Int32 y = mHost->mMarginTop + (Int32) (Math::Random() * (mHost->mUsableHeight - height));
    mHost->mClock->SetTranslationX(x);
    mHost-> mClock->SetTranslationY(y);
    Boolean bval;
    return mHost->mClock->PostDelayed(mHost->mMoveTextRunnable, MOVE_CLOCK_TIMEOUT, &bval);
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
    Boolean bval;
    return mClock->RemoveCallbacks(mMoveTextRunnable, &bval);
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
    Boolean bval;
    return mClock->Post(mMoveTextRunnable, &bval);
}

//==========================================================
// KeyguardDisplayManager
//==========================================================

CAR_INTERFACE_IMPL(KeyguardDisplayManager, Object, IKeyguardDisplayManager)

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
    mMediaRouter = IMediaRouter::Probe(obj);
    return NOERROR;
}

ECode KeyguardDisplayManager::Show()
{
    if (!mShowing) {
        if (DEBUG) Slogger::V(DTAG, "show");
        if (mMediaRouter != NULL) {
            mMediaRouter->AddCallback(IMediaRouter::ROUTE_TYPE_REMOTE_DISPLAY,
                    mMediaRouterCallback, IMediaRouter::CALLBACK_FLAG_PASSIVE_DISCOVERY);
        }
        UpdateDisplays(TRUE);
    }
    mShowing = TRUE;
    return NOERROR;
}

ECode KeyguardDisplayManager::Hide()
{
    if (mShowing) {
        if (DEBUG) Slogger::V(DTAG, "hide");
        if (mMediaRouter) {
            mMediaRouter->RemoveCallback(mMediaRouterCallback);
        }
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
        if (mMediaRouter != NULL) {
            mMediaRouter->GetSelectedRoute(IMediaRouter::ROUTE_TYPE_REMOTE_DISPLAY,
                (IMediaRouterRouteInfo**)&route);
        }

        Int32 type;
        Boolean useDisplay = route != NULL
                && ((route->GetPlaybackType(&type), type) == IMediaRouterRouteInfo::PLAYBACK_TYPE_REMOTE);
        AutoPtr<IDisplay> presentationDisplay;
        if (useDisplay) {
            route->GetPresentationDisplay((IDisplay**)&presentationDisplay);
        }

        if (mPresentation != NULL) {
            AutoPtr<IDisplay> display;
            mPresentation->GetDisplay((IDisplay**)&display);
            if (display.Get() != presentationDisplay.Get()) {
                if (DEBUG) Slogger::V(DTAG, "Display gone: %s", TO_CSTR(display));
                IDialogInterface::Probe(mPresentation)->Dismiss();
                mPresentation = NULL;
            }
        }

        if (mPresentation == NULL && presentationDisplay != NULL) {
            if (DEBUG) Slogger::I(DTAG, "Keyguard enabled on display: %s", TO_CSTR(presentationDisplay));
            AutoPtr<KeyguardPresentation> kp = new KeyguardPresentation();
            kp->constructor(mContext, presentationDisplay);
            mPresentation = kp.Get();
            kp->SetOnDismissListener(mOnDismissListener);
            //try {
            ECode ec = kp->Show();
            //} catch (WindowManager.InvalidDisplayException ex) {
            if (ec == (ECode)E_INVALID_DISPLAY_EXCEPTION) {
                Slogger::W(DTAG, "Invalid display:%d", ec);
                mPresentation = NULL;
            }
        }
    }
    else {
        if (mPresentation != NULL) {
            IDialogInterface::Probe(mPresentation)->Dismiss();
            mPresentation = NULL;
        }
    }
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
