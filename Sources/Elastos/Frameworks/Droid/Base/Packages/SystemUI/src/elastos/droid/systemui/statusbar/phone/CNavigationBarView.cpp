
#include "elastos/droid/systemui/statusbar/phone/CNavigationBarView.h"
#include "elastos/droid/systemui/statusbar/phone/CPhoneStatusBar.h"
#include "elastos/droid/systemui/statusbar/phone/NavigationBarViewTaskSwitchHelper.h"
#include "../R.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::EIID_ITransitionListener;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::SystemUI::StatusBar::Policy::IKeyButtonView;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

Boolean CNavigationBarView::DEBUG = FALSE;
String CNavigationBarView::TAG("PhoneStatusBar/NavigationBarView");
Boolean CNavigationBarView::SLIPPERY_WHEN_DISABLED = TRUE;
const Boolean CNavigationBarView::WORKAROUND_INVALID_LAYOUT = TRUE;
const Int32 CNavigationBarView::MSG_CHECK_INVALID_LAYOUT = 8686;

//==========================================================================
// CNavigationBarView::NavTransitionListener
//==========================================================================
CAR_INTERFACE_IMPL(CNavigationBarView::NavTransitionListener, Object, ITransitionListener)

CNavigationBarView::NavTransitionListener::NavTransitionListener(
    /* [in] */ CNavigationBarView* host)
    : mHost(host)
    , mBackTransitioning(FALSE)
    , mHomeAppearing(FALSE)
    , mStartDelay(0)
    , mDuration(0)
{}

ECode CNavigationBarView::NavTransitionListener::StartTransition(
    /* [in] */ ILayoutTransition* transition,
    /* [in] */ IViewGroup* container,
    /* [in] */ IView* view,
    /* [in] */ Int32 transitionType)
{
    Int32 id = 0;
    view->GetId(&id);
    if (id == R::id::back) {
        mBackTransitioning = TRUE;
    }
    else if (id == R::id::home && transitionType == ILayoutTransition::APPEARING) {
        mHomeAppearing = TRUE;
        transition->GetStartDelay(transitionType, &mStartDelay);
        transition->GetDuration(transitionType, &mDuration);
        transition->GetInterpolator(transitionType, (ITimeInterpolator**)&mInterpolator);
    }
    return NOERROR;
}

ECode CNavigationBarView::NavTransitionListener::EndTransition(
    /* [in] */ ILayoutTransition* transition,
    /* [in] */ IViewGroup* container,
    /* [in] */ IView* view,
    /* [in] */ Int32 transitionType)
{
    Int32 id = 0;
    view->GetId(&id);
    if (id == R::id::back) {
        mBackTransitioning = FALSE;
    }
    else if (id == R::id::home && transitionType == ILayoutTransition::APPEARING) {
        mHomeAppearing = FALSE;
    }
    return NOERROR;
}

ECode CNavigationBarView::NavTransitionListener::OnBackAltCleared()
{
    // When dismissing ime during unlock, force the back button to run the same appearance
    // animation as home (if we catch this condition early enough).
    AutoPtr<IView> view;
    mHost->GetBackButton((IView**)&view);
    Int32 v = 0;
    Float alpha = 0;
    AutoPtr<IView> hb;
    mHost->GetHomeButton((IView**)&hb);
    if (!mBackTransitioning && (view->GetVisibility(&v), v) == IView::VISIBLE
            && mHomeAppearing && (hb->GetAlpha(&alpha), alpha) == 0) {
        view->SetAlpha(0);

        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(2);
        (*fvs)[0] = 0;
        (*fvs)[1] = 1;
        AutoPtr<IObjectAnimator> oa;
        helper->OfFloat(view, String("alpha"), fvs, (IObjectAnimator**)&oa);
        AutoPtr<IValueAnimator> a = IValueAnimator::Probe(oa);
        IAnimator::Probe(a)->SetStartDelay(mStartDelay);
        a->SetDuration(mDuration);
        IAnimator::Probe(a)->SetInterpolator(mInterpolator);
        IAnimator::Probe(a)->Start();
    }
    return NOERROR;
}

//==========================================================================
// CNavigationBarView::H
//==========================================================================
CNavigationBarView::H::H(
    /* [in] */ CNavigationBarView* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CNavigationBarView::H::HandleMessage(
    /* [in] */ IMessage* m)
{
    Int32 what = 0;
    m->GetWhat(&what);
    switch (what) {
        case MSG_CHECK_INVALID_LAYOUT: {
            AutoPtr<IInterface> obj;
            m->GetObj((IInterface**)&obj);
            String how = String("") + TO_STR(obj);
            Int32 w = 0;
            mHost->GetWidth(&w);
            Int32 h = 0;
            mHost->GetHeight(&h);
            Int32 vw = 0;
            mHost->mCurrentView->GetWidth(&vw);
            Int32 vh = 0;
            mHost->mCurrentView->GetHeight(&vh);

            if (h != vh || w != vw) {
                Logger::W(TAG, "*** Invalid layout in navigation bar (%s this=%dx%d cur=%dx%d)",
                    how.string(), w, h, vw, vh);
                if (WORKAROUND_INVALID_LAYOUT) {
                    mHost->RequestLayout();
                }
            }
            break;
        }
    }
    return NOERROR;
}

//==========================================================================
// CNavigationBarView::ImeSwitcherClickListener
//==========================================================================
CAR_INTERFACE_IMPL(CNavigationBarView::ImeSwitcherClickListener, Object, IViewOnClickListener)

CNavigationBarView::ImeSwitcherClickListener::ImeSwitcherClickListener(
    /* [in] */ CNavigationBarView* host)
    : mHost(host)
{}

ECode CNavigationBarView::ImeSwitcherClickListener::OnClick(
    /* [in] */ IView* view)
{
    AutoPtr<IInterface> obj;
    mHost->mContext->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
    IInputMethodManager::Probe(obj)->ShowInputMethodPicker();
    return NOERROR;
}

//==========================================================================
// CNavigationBarView
//==========================================================================
CAR_OBJECT_IMPL(CNavigationBarView)

CAR_INTERFACE_IMPL(CNavigationBarView, LinearLayout, INavigationBarView)

CNavigationBarView::CNavigationBarView()
    : mBarSize(0)
    , mVertical(FALSE)
    , mScreenOn(FALSE)
    , mShowMenu(FALSE)
    , mDisabledFlags(0)
    , mNavigationIconHints(0)
    , mIsLayoutRtl(FALSE)
    , mDelegateIntercepted(FALSE)
{
}

ECode CNavigationBarView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    LinearLayout::constructor(context, attrs);

    mHandler = new H(this);
    mRotatedViews = ArrayOf<IView*>::Alloc(4);
    mTransitionListener = new NavTransitionListener(this);
    mImeSwitcherClickListener = new ImeSwitcherClickListener(this);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&obj);
    IWindowManager::Probe(obj)->GetDefaultDisplay((IDisplay**)&mDisplay);

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::navigation_bar_size, &mBarSize);
    mVertical = FALSE;
    mShowMenu = FALSE;
    mDelegateHelper = new DelegateViewHelper(this);
    mTaskSwitchHelper = new NavigationBarViewTaskSwitchHelper(context);

    GetIcons(res);

    mBarTransitions = new NavigationBarTransitions(this);
    return NOERROR;
}

ECode CNavigationBarView::GetBarTransitions(
    /* [out] */ IBarTransitions** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBarTransitions;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CNavigationBarView::SetDelegateView(
    /* [in] */ IView* view)
{
    mDelegateHelper->SetDelegateView(view);
    return NOERROR;
}

ECode CNavigationBarView::SetBar(
    /* [in] */ IBaseStatusBar* phoneStatusBar)
{
    mTaskSwitchHelper->SetBar(phoneStatusBar);
    mDelegateHelper->SetBar(phoneStatusBar);
    return NOERROR;
}

ECode CNavigationBarView::SetOnVerticalChangedListener(
    /* [in] */ IOnVerticalChangedListener* onVerticalChangedListener)
{
    mOnVerticalChangedListener = onVerticalChangedListener;
    NotifyVerticalChangedListener(mVertical);
    return NOERROR;
}

ECode CNavigationBarView::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    InitDownStates(event);
    Boolean tmp = FALSE;
    if (!mDelegateIntercepted && (mTaskSwitchHelper->OnTouchEvent(event, &tmp), tmp)) {
        *result = TRUE;
        return NOERROR;
    }

    Int32 action = 0;
    if (mDeadZone != NULL && (event->GetAction(&action), action) == IMotionEvent::ACTION_OUTSIDE) {
        mDeadZone->Poke(event);
    }
    if (mDelegateHelper != NULL && mDelegateIntercepted) {
        Boolean ret = mDelegateHelper->OnInterceptTouchEvent(event);
        if (ret) {
            *result = TRUE;
            return NOERROR;
        }
    }
    return LinearLayout::OnTouchEvent(event, result);
}

void CNavigationBarView::InitDownStates(
    /* [in] */ IMotionEvent* ev)
{
    Int32 action = 0;
    ev->GetAction(&action);
    if (action == IMotionEvent::ACTION_DOWN) {
        mDelegateIntercepted = FALSE;
    }
}

ECode CNavigationBarView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    InitDownStates(event);
    Boolean intercept = FALSE;
    mTaskSwitchHelper->OnInterceptTouchEvent(event, &intercept);
    if (!intercept) {
        mDelegateIntercepted = mDelegateHelper->OnInterceptTouchEvent(event);
        intercept = mDelegateIntercepted;
    }
    else {
        AutoPtr<IMotionEventHelper> helper;
        CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
        AutoPtr<IMotionEvent> cancelEvent;
        helper->Obtain(event, (IMotionEvent**)&cancelEvent);
        cancelEvent->SetAction(IMotionEvent::ACTION_CANCEL);
        mDelegateHelper->OnInterceptTouchEvent(cancelEvent);
        IInputEvent::Probe(cancelEvent)->Recycle();
    }
    *result = intercept;
    return NOERROR;
}

ECode CNavigationBarView::GetCurrentView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = mCurrentView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CNavigationBarView::GetRecentsButton(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    return mCurrentView->FindViewById(R::id::recent_apps, view);
}

ECode CNavigationBarView::GetMenuButton(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    return mCurrentView->FindViewById(R::id::menu, view);
}

ECode CNavigationBarView::GetBackButton(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    return mCurrentView->FindViewById(R::id::back, view);
}

ECode CNavigationBarView::GetHomeButton(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    return mCurrentView->FindViewById(R::id::home, view);
}

ECode CNavigationBarView::GetImeSwitchButton(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    return mCurrentView->FindViewById(R::id::ime_switcher, view);
}

void CNavigationBarView::GetIcons(
    /* [in] */ IResources* res)
{
    res->GetDrawable(R::drawable::ic_sysbar_back, (IDrawable**)&mBackIcon);
    res->GetDrawable(R::drawable::ic_sysbar_back_land, (IDrawable**)&mBackLandIcon);
    res->GetDrawable(R::drawable::ic_sysbar_back_ime, (IDrawable**)&mBackAltIcon);
    res->GetDrawable(R::drawable::ic_sysbar_back_ime, (IDrawable**)&mBackAltLandIcon);
    res->GetDrawable(R::drawable::ic_sysbar_recent, (IDrawable**)&mRecentIcon);
    res->GetDrawable(R::drawable::ic_sysbar_recent_land, (IDrawable**)&mRecentLandIcon);
}

ECode CNavigationBarView::SetLayoutDirection(
    /* [in] */ Int32 layoutDirection)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    GetIcons(res);

    LinearLayout::SetLayoutDirection(layoutDirection);
    return NOERROR;
}

ECode CNavigationBarView::NotifyScreenOn(
    /* [in] */ Boolean screenOn)
{
    mScreenOn = screenOn;
    SetDisabledFlags(mDisabledFlags, TRUE);
    return NOERROR;
}

ECode CNavigationBarView::SetNavigationIconHints(
    /* [in] */ Int32 hints)
{
    SetNavigationIconHints(hints, FALSE);
    return NOERROR;
}

ECode CNavigationBarView::SetNavigationIconHints(
    /* [in] */ Int32 hints,
    /* [in] */ Boolean force)
{
    if (!force && hints == mNavigationIconHints) return NOERROR;
    const Boolean backAlt = (hints & IStatusBarManager::NAVIGATION_HINT_BACK_ALT) != 0;
    if ((mNavigationIconHints & IStatusBarManager::NAVIGATION_HINT_BACK_ALT) != 0 && !backAlt) {
        mTransitionListener->OnBackAltCleared();
    }
    if (DEBUG) {
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<ICharSequence> txt;
        CString::New(String("Navigation icon hints = ") + StringUtils::ToString(hints), (ICharSequence**)&txt);
        AutoPtr<IToast> t;
        helper->MakeText(ctx, txt, 500, (IToast**)&t);
        t->Show();
    }

    mNavigationIconHints = hints;

    AutoPtr<IView> view;
    GetBackButton((IView**)&view);
    IImageView::Probe(view)->SetImageDrawable(backAlt
            ? (mVertical ? mBackAltLandIcon : mBackAltIcon)
            : (mVertical ? mBackLandIcon : mBackIcon));

    view = NULL;
    GetRecentsButton((IView**)&view);
    IImageView::Probe(view)->SetImageDrawable(mVertical ? mRecentLandIcon : mRecentIcon);

    const Boolean showImeButton = ((hints & IStatusBarManager::NAVIGATION_HINT_IME_SHOWN) != 0);

    view = NULL;
    GetImeSwitchButton((IView**)&view);
    view->SetVisibility(showImeButton ? IView::VISIBLE : IView::INVISIBLE);
    // Update menu button in case the IME state has changed.
    SetMenuVisibility(mShowMenu, TRUE);

    return SetDisabledFlags(mDisabledFlags, TRUE);
}

ECode CNavigationBarView::SetDisabledFlags(
    /* [in] */ Int32 disabledFlags)
{
    return SetDisabledFlags(disabledFlags, FALSE);
}

ECode CNavigationBarView::SetDisabledFlags(
    /* [in] */ Int32 disabledFlags,
    /* [in] */ Boolean force)
{
    if (!force && mDisabledFlags == disabledFlags) return NOERROR;

    mDisabledFlags = disabledFlags;

    const Boolean disableHome = ((disabledFlags & IView::STATUS_BAR_DISABLE_HOME) != 0);
    const Boolean disableRecent = ((disabledFlags & IView::STATUS_BAR_DISABLE_RECENT) != 0);
    const Boolean disableBack = ((disabledFlags & IView::STATUS_BAR_DISABLE_BACK) != 0)
            && ((mNavigationIconHints & IStatusBarManager::NAVIGATION_HINT_BACK_ALT) == 0);
    const Boolean disableSearch = ((disabledFlags & IView::STATUS_BAR_DISABLE_SEARCH) != 0);

    if (SLIPPERY_WHEN_DISABLED) {
        SetSlippery(disableHome && disableRecent && disableBack && disableSearch);
    }

    AutoPtr<IView> view;
    mCurrentView->FindViewById(R::id::nav_buttons, (IView**)&view);
    AutoPtr<IViewGroup> navButtons = IViewGroup::Probe(view);
    if (navButtons != NULL) {
        AutoPtr<ILayoutTransition> lt;
        navButtons->GetLayoutTransition((ILayoutTransition**)&lt);
        if (lt != NULL) {
            AutoPtr<IList> tls;
            lt->GetTransitionListeners((IList**)&tls);
            Boolean contains = FALSE;
            if (tls->Contains(mTransitionListener->Probe(EIID_IInterface), &contains), !contains) {
                lt->AddTransitionListener(mTransitionListener);
            }
            if (!mScreenOn && mCurrentView != NULL) {
                lt->DisableTransitionType(
                        ILayoutTransition::CHANGE_APPEARING |
                        ILayoutTransition::CHANGE_DISAPPEARING |
                        ILayoutTransition::APPEARING |
                        ILayoutTransition::DISAPPEARING);
            }
        }
    }

    view = NULL;
    GetBackButton((IView**)&view);
    view->SetVisibility(disableBack ? IView::INVISIBLE : IView::VISIBLE);

    view = NULL;
    GetHomeButton((IView**)&view);
    view->SetVisibility(disableHome ? IView::INVISIBLE : IView::VISIBLE);

    view = NULL;
    GetRecentsButton((IView**)&view);
    view->SetVisibility(disableRecent ? IView::INVISIBLE : IView::VISIBLE);

    Int32 mode = 0;
    mBarTransitions->GetMode(&mode);
    mBarTransitions->ApplyBackButtonQuiescentAlpha(mode, TRUE /*animate*/);
    return NOERROR;
}

void CNavigationBarView::SetVisibleOrGone(
    /* [in] */ IView* view,
    /* [in] */ Boolean visible)
{
    if (view != NULL) {
        view->SetVisibility(visible ? IView::VISIBLE : IView::GONE);
    }
}

ECode CNavigationBarView::SetSlippery(
    /* [in] */ Boolean newSlippery)
{
    AutoPtr<IViewGroupLayoutParams> vp;
    GetLayoutParams((IViewGroupLayoutParams**)&vp);
    AutoPtr<IWindowManagerLayoutParams> lp = IWindowManagerLayoutParams::Probe(vp);
    if (lp != NULL) {
        Int32 flags = 0;
        lp->GetFlags(&flags);
        Boolean oldSlippery = (flags & IWindowManagerLayoutParams::FLAG_SLIPPERY) != 0;
        if (!oldSlippery && newSlippery) {
            lp->SetFlags(flags | IWindowManagerLayoutParams::FLAG_SLIPPERY);
        }
        else if (oldSlippery && !newSlippery) {
            lp->SetFlags(flags & ~IWindowManagerLayoutParams::FLAG_SLIPPERY);
        }
        else {
            return NOERROR;
        }

        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<IInterface> obj;
        ctx->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&obj);
        AutoPtr<IWindowManager> wm = IWindowManager::Probe(obj);
        IViewManager::Probe(wm)->UpdateViewLayout(this, IViewGroupLayoutParams::Probe(lp));
    }
    return NOERROR;
}

ECode CNavigationBarView::SetMenuVisibility(
    /* [in] */ Boolean show)
{
    SetMenuVisibility(show, FALSE);
    return NOERROR;
}

ECode CNavigationBarView::SetMenuVisibility(
    /* [in] */ Boolean show,
    /* [in] */ Boolean force)
{
    if (!force && mShowMenu == show) return NOERROR;

    mShowMenu = show;

    // Only show Menu if IME switcher not shown.
    const Boolean shouldShow = mShowMenu &&
            ((mNavigationIconHints & IStatusBarManager::NAVIGATION_HINT_IME_SHOWN) == 0);

    AutoPtr<IView> view;
    GetMenuButton((IView**)&view);
    view->SetVisibility(shouldShow ? IView::VISIBLE : IView::INVISIBLE);
    return NOERROR;
}

ECode CNavigationBarView::OnFinishInflate()
{
    AutoPtr<IView> view;
    FindViewById(R::id::rot0, (IView**)&view);
    mRotatedViews->Set(ISurface::ROTATION_0, view);
    mRotatedViews->Set(ISurface::ROTATION_180, view);

    view = NULL;
    FindViewById(R::id::rot90, (IView**)&view);
    mRotatedViews->Set(ISurface::ROTATION_90, view);

    mRotatedViews->Set(ISurface::ROTATION_270, (*mRotatedViews)[ISurface::ROTATION_90]);

    mCurrentView = (*mRotatedViews)[ISurface::ROTATION_0];

    view = NULL;
    GetImeSwitchButton((IView**)&view);
    view->SetOnClickListener(mImeSwitcherClickListener);

    UpdateRTLOrder();
    return NOERROR;
}

ECode CNavigationBarView::IsVertical(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mVertical;
    return NOERROR;
}

ECode CNavigationBarView::Reorient()
{
    Int32 rot = 0;
    mDisplay->GetRotation(&rot);
    for (Int32 i=0; i<4; i++) {
        (*mRotatedViews)[i]->SetVisibility(IView::GONE);
    }
    mCurrentView = (*mRotatedViews)[rot];
    mCurrentView->SetVisibility(IView::VISIBLE);

    AutoPtr<IView> view;
    GetImeSwitchButton((IView**)&view);
    view->SetOnClickListener(mImeSwitcherClickListener);

    view = NULL;
    mCurrentView->FindViewById(R::id::deadzone, (IView**)&view);
    mDeadZone = IDeadZone::Probe(view);

    // force the low profile & disabled states into compliance
    mBarTransitions->Init(mVertical);
    SetDisabledFlags(mDisabledFlags, TRUE /* force */);
    SetMenuVisibility(mShowMenu, TRUE /* force */);

    if (DEBUG) {
        Logger::D(TAG, "reorient(): rot=%d", rot);
    }

    // swap to x coordinate if orientation is not in vertical
    if (mDelegateHelper != NULL) {
        mDelegateHelper->SetSwapXY(mVertical);
    }
    UpdateTaskSwitchHelper();

    SetNavigationIconHints(mNavigationIconHints, TRUE);
    return NOERROR;
}

void CNavigationBarView::UpdateTaskSwitchHelper()
{
    Int32 ld = 0;
    GetLayoutDirection(&ld);
    Boolean isRtl = ld == IView::LAYOUT_DIRECTION_RTL;
    mTaskSwitchHelper->SetBarState(mVertical, isRtl);
}

ECode CNavigationBarView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    LinearLayout::OnLayout(changed, l, t, r, b);

    AutoPtr<ArrayOf<IView*> > views = ArrayOf<IView*>::Alloc(3);
    AutoPtr<IView> view;
    GetHomeButton((IView**)&view);
    views->Set(0, view);

    view = NULL;
    GetBackButton((IView**)&view);
    views->Set(1, view);

    view = NULL;
    GetRecentsButton((IView**)&view);
    views->Set(2, view);

    mDelegateHelper->SetInitialTouchRegion(views);
    return NOERROR;
}

ECode CNavigationBarView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    if (DEBUG) Logger::D(TAG, "onSizeChanged: (%dx%d) old: (%dx%d)", w, h, oldw, oldh);

    const Boolean newVertical = w > 0 && h > w;
    if (newVertical != mVertical) {
        mVertical = newVertical;
        //Log.v(TAG, String.format("onSizeChanged: h=%d, w=%d, vert=%s", h, w, mVertical?"y":"n"));
        Reorient();
        NotifyVerticalChangedListener(newVertical);
    }

    PostCheckForInvalidLayout(String("sizeChanged"));
    LinearLayout::OnSizeChanged(w, h, oldw, oldh);
    return NOERROR;
}

void CNavigationBarView::NotifyVerticalChangedListener(
    /* [in] */ Boolean newVertical)
{
    if (mOnVerticalChangedListener != NULL) {
        mOnVerticalChangedListener->OnVerticalChanged(newVertical);
    }
}

ECode CNavigationBarView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    LinearLayout::OnConfigurationChanged(newConfig);
    UpdateRTLOrder();
    UpdateTaskSwitchHelper();
    return NOERROR;
}

void CNavigationBarView::UpdateRTLOrder()
{
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    Int32 dir = 0;
    config->GetLayoutDirection(&dir);
    Boolean isLayoutRtl = dir == IView::LAYOUT_DIRECTION_RTL;
    if (mIsLayoutRtl != isLayoutRtl) {
        // We swap all children of the 90 and 270 degree layouts, since they are vertical
        AutoPtr<IView> rotation90 = (*mRotatedViews)[ISurface::ROTATION_90];
        AutoPtr<IView> view;
        rotation90->FindViewById(R::id::nav_buttons, (IView**)&view);
        SwapChildrenOrderIfVertical(view);

        AutoPtr<IView> rotation270 = (*mRotatedViews)[ISurface::ROTATION_270];
        if (rotation90 != rotation270) {
            view = NULL;
            rotation270->FindViewById(R::id::nav_buttons, (IView**)&view);
            SwapChildrenOrderIfVertical(view);
        }
        mIsLayoutRtl = isLayoutRtl;
    }
}

void CNavigationBarView::SwapChildrenOrderIfVertical(
    /* [in] */ IView* group)
{
    if (ILinearLayout::Probe(group)) {
        AutoPtr<ILinearLayout> linearLayout = ILinearLayout::Probe(group);
        Int32 o = 0;
        linearLayout->GetOrientation(&o);
        if (o == ILinearLayout::VERTICAL) {
            IViewGroup* vg = IViewGroup::Probe(linearLayout);
            Int32 childCount = 0;
            vg->GetChildCount(&childCount);
            AutoPtr<ArrayOf<IView*> > childList = ArrayOf<IView*>::Alloc(childCount);
            for (Int32 i = 0; i < childCount; i++) {
                AutoPtr<IView> child;
                vg->GetChildAt(i, (IView**)&child);
                childList->Set(i, child);
            }
            vg->RemoveAllViews();
            for (Int32 i = childCount - 1; i >= 0; i--) {
                vg->AddView((*childList)[i]);
            }
        }
    }
}

String CNavigationBarView::GetResourceName(
    /* [in] */ Int32 resId)
{
    if (resId != 0) {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<IResources> res;
        ctx->GetResources((IResources**)&res);

        String result;
        if (FAILED(res->GetResourceName(resId, &result))) {
            return String("(unknown)");
        }
        return result;
    }

    return String("(NULL)");
}

void CNavigationBarView::PostCheckForInvalidLayout(
    /* [in] */ const String& how)
{
    AutoPtr<ICharSequence> obj;
    CString::New(how, (ICharSequence**)&obj);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_CHECK_INVALID_LAYOUT, 0, 0, obj, (IMessage**)&msg);
    msg->SendToTarget();
}

String CNavigationBarView::VisibilityToString(
    /* [in] */ Int32 vis)
{
    switch (vis) {
        case IView::INVISIBLE:
            return String("INVISIBLE");
        case IView::GONE:
            return String("GONE");
        default:
            return String("VISIBLE");
    }
    return String(NULL);
}

ECode CNavigationBarView::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("NavigationBarView {"));
    AutoPtr<IRect> r;
    CRect::New((IRect**)&r);
    AutoPtr<IPoint> size;
    CPoint::New((IPoint**)&size);
    mDisplay->GetRealSize(size);

    assert(0 && "TODO");
    String psInfo = CPhoneStatusBar::ViewInfo(this);
    Int32 v = 0;
    GetVisibility(&v);
    pw->Println(String("      this: ") + psInfo + String(" ") + VisibilityToString(v));

    GetWindowVisibleDisplayFrame(r);
    Int32 right = 0, x = 0, bottom = 0, y = 0;
    r->GetRight(&right);
    r->GetBottom(&bottom);
    size->GetX(&x);
    size->GetY(&y);
    Boolean offscreen = right > x || bottom > y;
    String sv;
    r->ToShortString(&sv);
    GetWindowVisibility(&v);
    pw->Println(String("      window: ") + sv + String(" ") + VisibilityToString(v)
            + (offscreen ? String(" OFFSCREEN!") : String("")));

    String format;
    Int32 id = 0;
    mCurrentView->GetId(&id);
    Int32 w = 0, h = 0;
    mCurrentView->GetWidth(&w);
    mCurrentView->GetHeight(&h);
    mCurrentView->GetVisibility(&v);
    format.AppendFormat("      mCurrentView: id=%s (%dx%d) %s",
                        GetResourceName(id).string(), w, h,
                        VisibilityToString(v).string());
    pw->Println(format);

    format = NULL;
    format.AppendFormat("      disabled=0x%08x vertical=%s menu=%s",
                    mDisabledFlags,
                    mVertical ? "TRUE" : "FALSE",
                    mShowMenu ? "TRUE" : "FALSE");
    pw->Println(format);

    AutoPtr<IView> view;
    DumpButton(pw, String("back"), (GetBackButton((IView**)&view), view));
    view = NULL;
    DumpButton(pw, String("home"), (GetHomeButton((IView**)&view), view));
    view = NULL;
    DumpButton(pw, String("rcnt"), (GetRecentsButton((IView**)&view), view));
    view = NULL;
    DumpButton(pw, String("menu"), (GetMenuButton((IView**)&view), view));

    pw->Println(String("    }"));
    return NOERROR;
}

void CNavigationBarView::DumpButton(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& caption,
    /* [in] */ IView* button)
{
    pw->Print(String("      ") + caption + String(": "));
    if (button == NULL) {
        pw->Print(String("NULL"));
    }
    else {
        String viewInfo = CPhoneStatusBar::ViewInfo(button);
        Int32 v = 0;
        Float alpha = 0;
        button->GetVisibility(&v);
        button->GetAlpha(&alpha);
        pw->Print(viewInfo + String(" ") + VisibilityToString(v)
                + String(" alpha=") + StringUtils::ToString(alpha));
        if (IKeyButtonView::Probe(button)) {
            Float fv = 0;
            IKeyButtonView::Probe(button)->GetDrawingAlpha(&fv);
            pw->Print(String(" drawingAlpha=") + StringUtils::ToString(fv));
            IKeyButtonView::Probe(button)->GetQuiescentAlpha(&fv);
            pw->Print(String(" quiescentAlpha=") + StringUtils::ToString(fv));
        }
    }
    pw->Println();
}

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
