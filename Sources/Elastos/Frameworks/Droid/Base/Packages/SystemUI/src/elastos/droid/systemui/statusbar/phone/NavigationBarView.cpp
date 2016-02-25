
#include "elastos/droid/systemui/statusbar/phone/NavigationBarView.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/widget/CLinearLayoutLayoutParams.h"
#include "elastos/droid/systemui/SystemUIR.h"
#include "elastos/droid/view/animation/CAccelerateInterpolator.h"

using Elastos::Droid::SystemUI::SystemUIR;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::Animation::CAccelerateInterpolator;
using Elastos::Droid::View::Animation::IAccelerateInterpolator;
using Elastos::Droid::Widget::ILinearLayoutLayoutParams;
using Elastos::Droid::Widget::CLinearLayoutLayoutParams;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {


//==============================================================================
//                  NavigationBarView::MyViewOnClickListener
//==============================================================================
CAR_INTERFACE_IMPL(NavigationBarView::MyViewOnClickListener, IViewOnClickListener)

NavigationBarView::MyViewOnClickListener::MyViewOnClickListener(
    /* [in] */ NavigationBarView* host)
    : mHost(host)
{
}

ECode NavigationBarView::MyViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    PRINT_ENTER_LEAVE("NavigationBarView::MyViewOnClickListener::OnClick");
    AutoPtr<IView> ssb = mHost->GetScreenShotButton();
    if (v == ssb) {
        mHost->TakeScreenshot();
    }
    return NOERROR;
}

//==============================================================================
//                  NavigationBarView::MyViewOnTouchListener
//==============================================================================
CAR_INTERFACE_IMPL(NavigationBarView::MyViewOnTouchListener, IViewOnTouchListener)

NavigationBarView::MyViewOnTouchListener::MyViewOnTouchListener(
    /* [in] */ NavigationBarView* host)
    : mHost(host)
{
}

ECode NavigationBarView::MyViewOnTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    Int32 action;
    event->GetAction(&action);
    if (action == IMotionEvent::ACTION_DOWN) {
        // even though setting the systemUI visibility below will turn these views
        // on, we need them to come up faster so that they can catch this motion
        // event
        mHost->SetLowProfile(FALSE, FALSE, FALSE);

//        try {
        mHost->mBarService->SetSystemUiVisibility(0, IView::SYSTEM_UI_FLAG_LOW_PROFILE);
//        } catch (android.os.RemoteException ex) {
//        }
    }
    return NOERROR;
}

//==============================================================================
//                  NavigationBarView::ScreenshotTimeoutRunnable
//==============================================================================
NavigationBarView::ScreenshotTimeoutRunnable::ScreenshotTimeoutRunnable(
    /* [in] */ NavigationBarView* host)
    : mHost(host)
{
}

ECode NavigationBarView::ScreenshotTimeoutRunnable::Run()
{
    AutoLock lock(mHost->mScreenshotLock);
    if (mHost->mScreenshotConnection != NULL) {
        mHost->mContext->UnbindService(mHost->mScreenshotConnection);
        mHost->mScreenshotConnection = NULL;
    }

    return NOERROR;
}

//==============================================================================
//                  NavigationBarView::LowLightsAnimationListener
//==============================================================================

NavigationBarView::LowLightsAnimationListener::LowLightsAnimationListener(
    /* [in] */ IView* light)
{
    mLight = light;
}

ECode NavigationBarView::LowLightsAnimationListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mLight->SetVisibility(IView::GONE);
    return NOERROR;
}

//==============================================================================
//                  NavigationBarView
//==============================================================================

const Boolean NavigationBarView::NAVBAR_ALWAYS_AT_RIGHT;
const Boolean NavigationBarView::SLIPPERY_WHEN_DISABLED;
const Boolean NavigationBarView::ANIMATE_HIDE_TRANSITION;
const Boolean NavigationBarView::WORKAROUND_INVALID_LAYOUT;
const Int32 NavigationBarView::MSG_CHECK_INVALID_LAYOUT;

NavigationBarView::NavigationBarView()
    : mBarSize(0)
    , mVertical(FALSE)
    , mScreenOn(FALSE)
    , mHidden(FALSE)
    , mLowProfile(FALSE)
    , mShowMenu(FALSE)
    , mDisabledFlags(0)
    , mNavigationIconHints(0)
{
    mRotatedViews = ArrayOf<IView*>::Alloc(4);
    mSoftVolumeKey = SystemProperties::GetBoolean(String("ro.statusbar.volume"), FALSE);
    mSoftScreenShotKey = SystemProperties::GetBoolean(String("ro.statusbar.screenshot"), FALSE);
}

NavigationBarView::NavigationBarView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : LinearLayout(context, attrs)
    , mBarSize(0)
    , mVertical(FALSE)
    , mScreenOn(FALSE)
    , mHidden(FALSE)
    , mLowProfile(FALSE)
    , mShowMenu(FALSE)
    , mDisabledFlags(0)
    , mNavigationIconHints(0)
{
    mRotatedViews = ArrayOf<IView*>::Alloc(4);
    mSoftVolumeKey = SystemProperties::GetBoolean(String("ro.statusbar.volume"), FALSE);
    mSoftScreenShotKey = SystemProperties::GetBoolean(String("ro.statusbar.screenshot"), FALSE);
}

ECode NavigationBarView::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    LinearLayout::Init(context, attrs);

    mHidden = FALSE;

    AutoPtr<IContext> ctx = GetContext();
    AutoPtr<IWindowManager> wm;
    ctx->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&wm);
    wm->GetDefaultDisplay((IDisplay**)&mDisplay);

    AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::STATUS_BAR_SERVICE);
    mBarService = IIStatusBarService::Probe(obj);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(SystemUIR::dimen::navigation_bar_size, &mBarSize);
    mVertical = FALSE;
    mShowMenu = FALSE;
    mDelegateHelper = new DelegateViewHelper(THIS_PROBE(IView));

    res->GetDrawable(SystemUIR::drawable::ic_sysbar_back, (IDrawable**)&mBackIcon);
    res->GetDrawable(SystemUIR::drawable::ic_sysbar_back_land, (IDrawable**)&mBackLandIcon);
    res->GetDrawable(SystemUIR::drawable::ic_sysbar_back_ime, (IDrawable**)&mBackAltIcon);
    res->GetDrawable(SystemUIR::drawable::ic_sysbar_back_land, (IDrawable**)&mBackAltLandIcon);
    return NOERROR;
}

ECode NavigationBarView::SetDelegateView(
    /* [in] */ IView* view)
{
    mDelegateHelper->SetDelegateView(view);
    return NOERROR;
}

ECode NavigationBarView::SetBar(
    /* [in] */ IBaseStatusBar* phoneStatusBar)
{
    mDelegateHelper->SetBar(phoneStatusBar);
    return NOERROR;
}

Boolean NavigationBarView::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    Int32 action;
    event->GetAction(&action);
    if (mDeadZone != NULL && action == IMotionEvent::ACTION_OUTSIDE) {
        mDeadZone->Poke(event);
    }
    if (mDelegateHelper != NULL) {
        Boolean ret = mDelegateHelper->OnInterceptTouchEvent(event);
        if (ret) return TRUE;
    }
    return LinearLayout::OnTouchEvent(event);
}

Boolean NavigationBarView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    return mDelegateHelper->OnInterceptTouchEvent(event);
}

AutoPtr<IView> NavigationBarView::GetRecentsButton()
{
    AutoPtr<IView> view;
    mCurrentView->FindViewById(SystemUIR::id::recent_apps, (IView**)&view);
    return view;
}

AutoPtr<IView> NavigationBarView::GetMenuButton()
{
    AutoPtr<IView> view;
    mCurrentView->FindViewById(SystemUIR::id::menu, (IView**)&view);
    return view;
}

AutoPtr<IView> NavigationBarView::GetBackButton()
{
    AutoPtr<IView> view;
    mCurrentView->FindViewById(SystemUIR::id::back, (IView**)&view);
    return view;
}

AutoPtr<IView> NavigationBarView::GetHomeButton()
{
    AutoPtr<IView> view;
    mCurrentView->FindViewById(SystemUIR::id::home, (IView**)&view);
    return view;
}

AutoPtr<IView> NavigationBarView::GetScreenShotButton()
{
    AutoPtr<IView> view;
    mCurrentView->FindViewById(SystemUIR::id::screenshot, (IView**)&view);
    return view;
}

AutoPtr<IView> NavigationBarView::GetVolumeDownButton()
{
    AutoPtr<IView> view;
    mCurrentView->FindViewById(SystemUIR::id::volume_down, (IView**)&view);
    return view;
}

AutoPtr<IView> NavigationBarView::GetVolumeUpButton()
{
    AutoPtr<IView> view;
    mCurrentView->FindViewById(SystemUIR::id::volume_up, (IView**)&view);
    return view;
}

AutoPtr<IView> NavigationBarView::GetSearchLight()
{
    AutoPtr<IView> view;
    mCurrentView->FindViewById(SystemUIR::id::search_light, (IView**)&view);
    return view;
}

// Assume this is called from the Handler thread.
void NavigationBarView::TakeScreenshot()
{
    {
        AutoLock lock(mScreenshotLock);
        if (mScreenshotConnection != NULL) {
            return;
        }
        AutoPtr<IComponentName> cn;
        CComponentName::New(
                String("com.android.systemui"),
                String("com.android.systemui.screenshot.TakeScreenshotService"),
                (IComponentName**)&cn);

        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        intent->SetComponent(cn);
        //TODO
//     ServiceConnection conn = new ServiceConnection() {
//         @Override
//         public void onServiceConnected(ComponentName name, IBinder service) {
//             synchronized(mScreenshotLock) {
//                 if (mScreenshotConnection != this) {
//                     return;
//                 }
//                 Messenger messenger = new Messenger(service);
//                 Message msg = Message.obtain(NULL, 1);
//                 final ServiceConnection myConn = this;
//                 Handler h = new Handler(mHandler.getLooper()) {
//                     @Override
//                     public void handleMessage(Message msg) {
//                         synchronized(mScreenshotLock) {
//                             if (mScreenshotConnection == myConn) {
//                                 mContext.unbindService(mScreenshotConnection);
//                                 mScreenshotConnection = NULL;
//                                 mHandler.removeCallbacks(mScreenshotTimeout);
//                             }
//                         }
//                     }
//                 };
//                 msg.replyTo = new Messenger(h);
//                 msg.arg1 = msg.arg2 = 0;
            //  msg.arg1 = 1;
//                 /*if (mStatusBar != NULL && mStatusBar.isVisibleLw())
//                     msg.arg1 = 1;
//                 if (mNavigationBar != NULL && mNavigationBar.isVisibleLw())
//                     msg.arg2 = 1;*/
//                 try {
//                     messenger.send(msg);
//                 } catch (RemoteException e) {
//                 }
//             }
//         }
//         @Override
//         public void onServiceDisconnected(ComponentName name) {}
//     };
//     if (mContext.bindService(intent, conn, Context.BIND_AUTO_CREATE)) {
//         mScreenshotConnection = conn;
//         mHandler.postDelayed(mScreenshotTimeout, 10000);
//     }
 }
}

ECode NavigationBarView::NotifyScreenOn(
    /* [in] */ Boolean screenOn)
{
    mScreenOn = screenOn;
    SetDisabledFlags(mDisabledFlags, TRUE);
    return NOERROR;
}

ECode NavigationBarView::SetNavigationIconHints(
    /* [in] */ Int32 hints)
{
    return SetNavigationIconHints(hints, FALSE);;
}

ECode NavigationBarView::SetNavigationIconHints(
    /* [in] */ Int32 hints,
    /* [in] */ Boolean force)
{
    if (!force && hints == mNavigationIconHints) return NOERROR;

    // if (DEBUG) {
    //     android.widget.Toast.makeText(mContext,
    //         "Navigation icon hints = " + hints,
    //         500).show();
    // }

    mNavigationIconHints = hints;

    AutoPtr<IView> home = GetHomeButton();
    AutoPtr<IView> back = GetBackButton();
    AutoPtr<IView> recent = GetRecentsButton();

    back->SetAlpha((0 != (hints & IStatusBarManager::NAVIGATION_HINT_BACK_NOP)) ? 0.5f : 1.0f);
    home->SetAlpha((0 != (hints & IStatusBarManager::NAVIGATION_HINT_HOME_NOP)) ? 0.5f : 1.0f);
    recent->SetAlpha((0 != (hints & IStatusBarManager::NAVIGATION_HINT_RECENT_NOP)) ? 0.5f : 1.0f);

    IImageView::Probe(back)->SetImageDrawable(
        (0 != (hints & IStatusBarManager::NAVIGATION_HINT_BACK_ALT))
        ? (mVertical ? mBackAltLandIcon : mBackAltIcon)
        : (mVertical ? mBackLandIcon : mBackIcon));

    SetDisabledFlags(mDisabledFlags, TRUE);
    return NOERROR;
}

ECode NavigationBarView::SetDisabledFlags(
    /* [in] */ Int32 disabledFlags)
{
    return SetDisabledFlags(disabledFlags, FALSE);;
}

ECode NavigationBarView::SetDisabledFlags(
    /* [in] */ Int32 disabledFlags,
    /* [in] */ Boolean force)
{
    if (!force && mDisabledFlags == disabledFlags) return NOERROR;

    mDisabledFlags = disabledFlags;

    Boolean disableHome = ((disabledFlags & IView::STATUS_BAR_DISABLE_HOME) != 0);
    Boolean disableRecent = ((disabledFlags & IView::STATUS_BAR_DISABLE_RECENT) != 0);
    Boolean disableBack = ((disabledFlags & IView::STATUS_BAR_DISABLE_BACK) != 0)
         && ((mNavigationIconHints & IStatusBarManager::NAVIGATION_HINT_BACK_ALT) == 0);
    Boolean disableSearch = ((disabledFlags & IView::STATUS_BAR_DISABLE_SEARCH) != 0);

    if (SLIPPERY_WHEN_DISABLED) {
        SetSlippery(disableHome && disableRecent && disableBack && disableSearch);
    }

    if (!mScreenOn && mCurrentView != NULL) {
        AutoPtr<IView> temp;
        mCurrentView->FindViewById(SystemUIR::id::nav_buttons, (IView**)&temp);
        IViewGroup* navButtons = IViewGroup::Probe(temp.Get()) ;
        AutoPtr<ILayoutTransition> lt;
        if (navButtons != NULL) {
            navButtons->GetLayoutTransition((ILayoutTransition**)&lt);
        }
        if (lt != NULL) {
            lt->DisableTransitionType(
                 ILayoutTransition::CHANGE_APPEARING | ILayoutTransition::CHANGE_DISAPPEARING |
                 ILayoutTransition::APPEARING | ILayoutTransition::DISAPPEARING);
        }
    }

    AutoPtr<IView> home = GetHomeButton();
    AutoPtr<IView> back = GetBackButton();
    AutoPtr<IView> recent = GetRecentsButton();

    back->SetVisibility(disableBack ? IView::INVISIBLE : IView::VISIBLE);
    home->SetVisibility(disableHome ? IView::INVISIBLE : IView::VISIBLE);
    recent->SetVisibility(disableRecent ? IView::INVISIBLE : IView::VISIBLE);

    AutoPtr<IView> ssb = GetScreenShotButton();
    if(ssb != NULL){
        ssb->SetVisibility(mSoftScreenShotKey ? (disableHome ? IView::INVISIBLE : IView::VISIBLE): IView::GONE);
    }
    AutoPtr<IView> vdb = GetVolumeDownButton();
    if(vdb != NULL){
        vdb->SetVisibility(mSoftVolumeKey?(disableHome ? IView::INVISIBLE : IView::VISIBLE): IView::GONE);
    }
    AutoPtr<IView> vub = GetVolumeUpButton();
    if(vub != NULL){
        vub->SetVisibility(mSoftVolumeKey?(disableHome ? IView::INVISIBLE : IView::VISIBLE): IView::GONE);
    }

    AutoPtr<IView> sl = GetSearchLight();
    sl->SetVisibility((disableHome && !disableSearch) ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

ECode NavigationBarView::SetSlippery(
    /* [in] */ Boolean newSlippery)
{
    AutoPtr<IViewGroupLayoutParams> vglp = GetLayoutParams();
    IWindowManagerLayoutParams* lp = IWindowManagerLayoutParams::Probe(vglp.Get());

    if (lp != NULL) {
        Int32 flags;
        lp->GetFlags(&flags);
        Boolean oldSlippery = (flags & IWindowManagerLayoutParams::FLAG_SLIPPERY) != 0;
        if (!oldSlippery && newSlippery) {
            lp->SetFlags(flags | IWindowManagerLayoutParams::FLAG_SLIPPERY);
        } else if (oldSlippery && !newSlippery) {
            lp->SetFlags(flags & ~IWindowManagerLayoutParams::FLAG_SLIPPERY);
        } else {
            return NOERROR;
        }

        AutoPtr<IContext> ctx = GetContext();
        AutoPtr<IWindowManager> wm;
        ctx->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&wm);
        wm->UpdateViewLayout(THIS_PROBE(IView), lp);
    }
    return NOERROR;
}

ECode NavigationBarView::SetMenuVisibility(
    /* [in] */ Boolean show)
{
    return SetMenuVisibility(show, FALSE);
}

ECode NavigationBarView::SetMenuVisibility(
    /* [in] */ Boolean show,
    /* [in] */ Boolean force)
{
    if (!force && mShowMenu == show) return NOERROR;

    mShowMenu = show;

    AutoPtr<IView> menuBtn = GetMenuButton();
    menuBtn->SetVisibility(mShowMenu ? IView::VISIBLE : IView::INVISIBLE);
    return NOERROR;
}

ECode NavigationBarView::SetLowProfile(
    /* [in] */ Boolean lightsOut)
{
    return SetLowProfile(lightsOut, TRUE, FALSE);
}

ECode NavigationBarView::SetLowProfile(
    /* [in] */ Boolean lightsOut,
    /* [in] */ Boolean animate,
    /* [in] */ Boolean force)
{
    if (!force && lightsOut == mLowProfile) return NOERROR;

    mLowProfile = lightsOut;

//    if (DEBUG) Slog.d(TAG, "setting lights " + (lightsOut?"out":"on"));

    AutoPtr<IView> navButtons, lowLights;
    mCurrentView->FindViewById(SystemUIR::id::nav_buttons, (IView**)&navButtons);
    mCurrentView->FindViewById(SystemUIR::id::lights_out, (IView**)&lowLights);

    // ok, everyone, stop it right there
    AutoPtr<IViewPropertyAnimator> navAnimator, lightAnimator;
    navButtons->Animate((IViewPropertyAnimator**)&navAnimator);
    navAnimator->Cancel();

    lowLights->Animate((IViewPropertyAnimator**)&lightAnimator);
    lightAnimator->Cancel();

    if (!animate) {
        navButtons->SetAlpha(lightsOut ? 0.0f : 1.0f);

        lowLights->SetAlpha(lightsOut ? 1.0f : 0.0f);
        lowLights->SetVisibility(lightsOut ? IView::VISIBLE : IView::GONE);
    } else {
        navAnimator = NULL;
        navButtons->Animate((IViewPropertyAnimator**)&navAnimator);
        navAnimator->Alpha(lightsOut ? 0.f : 1.f);
        navAnimator->SetDuration(lightsOut ? 750 : 250);
        navAnimator->Start();

        lowLights->SetOnTouchListener(mLightsOutListener);
        Int32 visibility;
        lowLights->GetVisibility(&visibility);
        if (visibility == IView::GONE) {
            lowLights->SetAlpha(0.0f);
            lowLights->SetVisibility(IView::VISIBLE);
        }
        lightAnimator = NULL;
        lowLights->Animate((IViewPropertyAnimator**)&lightAnimator);
        AutoPtr<IAccelerateInterpolator> itp;
        CAccelerateInterpolator::New(2.0f, (IAccelerateInterpolator**)&itp);
        lightAnimator->Alpha(lightsOut ? 1.f : 0.f);
        lightAnimator->SetDuration(lightsOut ? 750 : 250);
        lightAnimator->SetInterpolator(itp);
        AutoPtr<AnimatorListenerAdapter> l = new LowLightsAnimationListener(lowLights);
        lightAnimator->SetListener(lightsOut ? NULL : l);
        lightAnimator->Start();
    }
    return NOERROR;
}

ECode NavigationBarView::SetHidden(
    /* [in] */ Boolean hide)
{
    if (hide == mHidden) return NOERROR;

    mHidden = hide;
//    Slog::D(TAG, (hide ? "HIDING" : "SHOWING") + " navigation bar");

    // bring up the lights no matter what
    SetLowProfile(FALSE);
    return NOERROR;
}

ECode NavigationBarView::OnFinishInflate()
{
    mCurrentView = FindViewById(SystemUIR::id::rot0);
    assert(mCurrentView != NULL && "Please check the ids equals ids in R.h!");

    (*mRotatedViews)[ISurface::ROTATION_0] = mCurrentView;
    (*mRotatedViews)[ISurface::ROTATION_180] = mCurrentView;
    (*mRotatedViews)[ISurface::ROTATION_90] = FindViewById(SystemUIR::id::rot90);
    (*mRotatedViews)[ISurface::ROTATION_270] =
            NAVBAR_ALWAYS_AT_RIGHT ? FindViewById(SystemUIR::id::rot90) : FindViewById(SystemUIR::id::rot270);

    AutoPtr<IView> ssBtn = GetScreenShotButton();
    if(ssBtn != NULL){
        ssBtn->SetOnClickListener(mOnClickListener);
    }
    return NOERROR;
}

ECode NavigationBarView::Reorient()
{
    Int32 rot;
    mDisplay->GetRotation(&rot);
    for (Int32 i=0; i<4; i++) {
        (*mRotatedViews)[i]->SetVisibility(IView::GONE);
    }
    mCurrentView = (*mRotatedViews)[rot];
    mCurrentView->SetVisibility(IView::VISIBLE);

    AutoPtr<IView> tmpView;
    mCurrentView->FindViewById(SystemUIR::id::deadzone, (IView**)&tmpView);
    assert(tmpView != NULL);
    mDeadZone = IDeadZone::Probe(tmpView.Get());

    // force the low profile & disabled states into compliance
    SetLowProfile(mLowProfile, FALSE, TRUE /* force */);
    SetDisabledFlags(mDisabledFlags, TRUE /* force */);
    SetMenuVisibility(mShowMenu, TRUE /* force */);

    // if (DEBUG) {
    //     Slog.d(TAG, "reorient(): rot=" + mDisplay.getRotation());
    // }

    AutoPtr<IView> ssBtn = GetScreenShotButton();

    SetNavigationIconHints(mNavigationIconHints, TRUE);
    if (ssBtn != NULL){
        ssBtn->SetOnClickListener(mOnClickListener);
    }
    if(mSoftScreenShotKey ||mSoftVolumeKey){
        AutoPtr<IView> home = GetHomeButton();
        AutoPtr<IView> back = GetBackButton();
        AutoPtr<IView> recent = GetRecentsButton();

        AutoPtr<ILinearLayoutLayoutParams> lp;
        CLinearLayoutLayoutParams::New(100, IViewGroupLayoutParams::FILL_PARENT, (ILinearLayoutLayoutParams**)&lp);
        back->SetPadding(15, 0, 15, 0);
        back->SetLayoutParams(lp);

        lp = NULL;
        CLinearLayoutLayoutParams::New(100, IViewGroupLayoutParams::FILL_PARENT, (ILinearLayoutLayoutParams**)&lp);
        home->SetPadding(15, 0, 15, 0);
        home->SetLayoutParams(lp);

        lp = NULL;
        CLinearLayoutLayoutParams::New(100, IViewGroupLayoutParams::FILL_PARENT, (ILinearLayoutLayoutParams**)&lp);
        recent->SetPadding(15, 0, 15, 0);
        recent->SetLayoutParams(lp);
    }
    return NOERROR;
}

void NavigationBarView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32  b)
{
    LinearLayout::OnLayout(changed, l, t, r, b);

    AutoPtr<IView> home, back, recent, rotation;
    home = GetHomeButton();
    back = GetBackButton();
    recent = GetRecentsButton();

    AutoPtr<ArrayOf<IView*> > views = ArrayOf<IView*>::Alloc(4);
    views->Set(0, home);
    views->Set(1, back);
    views->Set(2, recent);
    views->Set(3, rotation);
    mDelegateHelper->SetInitialTouchRegion(views);
}

void NavigationBarView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    // if (DEBUG) Slog.d(TAG, String.format(
    //         "onSizeChanged: (%dx%d) old: (%dx%d)", w, h, oldw, oldh));

    Boolean newVertical = w > 0 && h > w;
    if (newVertical != mVertical) {
        mVertical = newVertical;
        //Slog.v(TAG, String.format("onSizeChanged: h=%d, w=%d, vert=%s", h, w, mVertical?"y":"n"));
        Reorient();
    }

    PostCheckForInvalidLayout(String("sizeChanged"));
    LinearLayout::OnSizeChanged(w, h, oldw, oldh);
}

String NavigationBarView::GetResourceName(
    /* [in] */ Int32 resId)
{
    if (resId != 0) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        String name;
        ECode ec = res->GetResourceName(resId, &name);
        if (ec == (ECode)E_NOT_FOUND_EXCEPTION) {
            return String("(unknown)");
        }
        return name;
    } else {
        return String("(null)");
    }
}

void NavigationBarView::PostCheckForInvalidLayout(
    /* [in] */ const String& how)
{
    //TODO
    // mHandler.obtainMessage(MSG_CHECK_INVALID_LAYOUT, 0, 0, how).sendToTarget();
}

String NavigationBarView::VisibilityToString(
    /* [in] */ Int32 vis)
{
    switch (vis) {
        case IView::INVISIBLE:
            return String("INVISIBLE");
        case IView::GONE:
            return String("GONE");
    }
    return String("VISIBLE");
}

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
