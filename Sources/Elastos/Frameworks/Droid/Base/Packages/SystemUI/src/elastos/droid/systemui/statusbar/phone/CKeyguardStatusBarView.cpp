
#include "elastos/droid/systemui/statusbar/phone/CKeyguardStatusBarView.h"
#include "elastos/droid/systemui/statusbar/phone/CPhoneStatusBar.h"
#include "../R.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/view/animation/AnimationUtils.h>
#include <elastos/droid/R.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_IBatteryStateChangeCallback;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_IUserInfoControllerOnUserInfoChangedListener;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::EIID_IOnPreDrawListener;
using Elastos::Droid::View::EIID_IViewParent;
using Elastos::Droid::View::IViewOverlay;
using Elastos::Droid::View::IViewGroupOverlay;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CAR_INTERFACE_IMPL(CKeyguardStatusBarView::OnUserInfoChangedListener, Object, IUserInfoControllerOnUserInfoChangedListener)
CKeyguardStatusBarView::OnUserInfoChangedListener::OnUserInfoChangedListener(
    /* [in] */ CKeyguardStatusBarView* host)
    : mHost(host)
{}

ECode CKeyguardStatusBarView::OnUserInfoChangedListener::OnUserInfoChanged(
    /* [in] */ const String& name,
    /* [in] */ IDrawable* picture)
{
    mHost->mMultiUserAvatar->SetImageDrawable(picture);
    return NOERROR;
}

CKeyguardStatusBarView::OnPreDrawListener::Runnable1::Runnable1(
    /* [in] */ CKeyguardStatusBarView* host)
    : mHost(host)
{}

ECode CKeyguardStatusBarView::OnPreDrawListener::Runnable1::Run()
{
    IView::Probe(mHost->mMultiUserSwitch)->SetAlpha(1.f);
    AutoPtr<IViewOverlay> vo;
    mHost->GetOverlay((IViewOverlay**)&vo);
    IViewGroupOverlay::Probe(vo)->Remove(IView::Probe(mHost->mMultiUserSwitch));
    return NOERROR;
}

CAR_INTERFACE_IMPL(CKeyguardStatusBarView::OnPreDrawListener, Object, IOnPreDrawListener)
CKeyguardStatusBarView::OnPreDrawListener::OnPreDrawListener(
    /* [in] */ CKeyguardStatusBarView* host,
    /* [in] */ Boolean userSwitcherVisible,
    /* [in] */ Int32 systemIconsCurrentX)
    : mHost(host)
    , mUserSwitcherVisible(userSwitcherVisible)
    , mSystemIconsCurrentX(systemIconsCurrentX)
{}

ECode CKeyguardStatusBarView::OnPreDrawListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IViewTreeObserver> vto;
    mHost->GetViewTreeObserver((IViewTreeObserver**)&vto);
    vto->RemoveOnPreDrawListener(this);
    AutoPtr<IViewParent> vp;
    Boolean tmp = (IView::Probe(mHost->mMultiUserSwitch)->GetParent((IViewParent**)&vp), vp.Get())
                    != ((IViewParent*)mHost->Probe(EIID_IViewParent));
    Boolean userSwitcherHiding = mUserSwitcherVisible && tmp;

    mHost->mSystemIconsSuperContainer->SetX(mSystemIconsCurrentX);
    AutoPtr<IViewPropertyAnimator> va;
    mHost->mSystemIconsSuperContainer->Animate((IViewPropertyAnimator**)&va);
    va->TranslationX(0);
    va->SetDuration(400);
    va->SetStartDelay(userSwitcherHiding ? 300 : 0);
    IAnimator::Probe(va)->SetInterpolator(ITimeInterpolator::Probe(mHost->mFastOutSlowInInterpolator));
    va->Start();
    if (userSwitcherHiding) {
        AutoPtr<IViewOverlay> vo;
        mHost->GetOverlay((IViewOverlay**)&vo);
        IViewGroupOverlay::Probe(vo)->Add(IView::Probe(mHost->mMultiUserSwitch));

        va = NULL;
        IView::Probe(mHost->mMultiUserSwitch)->Animate((IViewPropertyAnimator**)&va);
        va->Alpha(0.f);
        va->SetDuration(300);
        va->SetStartDelay(0);
        IAnimator::Probe(va)->SetInterpolator(ITimeInterpolator::Probe(CPhoneStatusBar::ALPHA_OUT));
        AutoPtr<Runnable1> run = new Runnable1(mHost);
        va->WithEndAction(run);
        va->Start();
    }
    else {
        IView::Probe(mHost->mMultiUserSwitch)->SetAlpha(0.f);
        va = NULL;
        IView::Probe(mHost->mMultiUserSwitch)->Animate((IViewPropertyAnimator**)&va);
        va->Alpha(1.f);
        va->SetDuration(300);
        va->SetStartDelay(200);
        IAnimator::Probe(va)->SetInterpolator(ITimeInterpolator::Probe(CPhoneStatusBar::ALPHA_IN));
    }
    *result = TRUE;
    return NOERROR;
}

CAR_OBJECT_IMPL(CKeyguardStatusBarView)
CAR_INTERFACE_IMPL_2(CKeyguardStatusBarView, RelativeLayout, IKeyguardStatusBarView, IBatteryStateChangeCallback);
CKeyguardStatusBarView::CKeyguardStatusBarView()
    : mBatteryCharging(FALSE)
    , mKeyguardUserSwitcherShowing(FALSE)
    , mBatteryListening(FALSE)
    , mSystemIconsSwitcherHiddenExpandedMargin(0)
{}

ECode CKeyguardStatusBarView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return RelativeLayout::constructor(context, attrs);
}

ECode CKeyguardStatusBarView::OnFinishInflate()
{
    RelativeLayout::OnFinishInflate();
    FindViewById(R::id::system_icons_super_container, (IView**)&mSystemIconsSuperContainer);
    AutoPtr<IView> view;
    FindViewById(R::id::multi_user_switch, (IView**)&view);
    mMultiUserSwitch = IMultiUserSwitch::Probe(view);

    view = NULL;
    FindViewById(R::id::multi_user_avatar, (IView**)&view);
    mMultiUserAvatar = IImageView::Probe(view);

    view = NULL;
    FindViewById(R::id::battery_level, (IView**)&view);
    mBatteryLevel = ITextView::Probe(view);

    view = NULL;
    Logger::D("CKeyguardStatusBarView", "TODO : Not Implement com.android.keyguard.CarrierText");
    // FindViewById(R::id::keyguard_carrier_text, (IView**)&view);
    mCarrierLabel = ITextView::Probe(view);
    LoadDimens();

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AnimationUtils::LoadInterpolator(ctx, Elastos::Droid::R::interpolator::fast_out_slow_in,
            (IInterpolator**)&mFastOutSlowInInterpolator);
    UpdateUserSwitcher();
    return NOERROR;
}

void CKeyguardStatusBarView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    RelativeLayout::OnConfigurationChanged(newConfig);

    // Respect font size setting.
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Int32 size = 0;
    res->GetDimensionPixelSize(Elastos::Droid::R::dimen::text_size_small_material, &size);
    if (mCarrierLabel != NULL) {
        mCarrierLabel->SetTextSize(ITypedValue::COMPLEX_UNIT_PX, size);
    }

    res->GetDimensionPixelSize(R::dimen::battery_level_text_size, &size);
    mBatteryLevel->SetTextSize(ITypedValue::COMPLEX_UNIT_PX, size);
}

void CKeyguardStatusBarView::LoadDimens()
{
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::system_icons_switcher_hidden_expanded_margin
            , &mSystemIconsSwitcherHiddenExpandedMargin);
}

void CKeyguardStatusBarView::UpdateVisibilities()
{
    AutoPtr<IViewParent> vp;
    IView::Probe(mMultiUserSwitch)->GetParent((IViewParent**)&vp);
    if (vp.Get() != (IViewParent*)this && !mKeyguardUserSwitcherShowing) {
        if (vp != NULL) {
            AutoPtr<IViewOverlay> vo;
            GetOverlay((IViewOverlay**)&vo);
            IViewGroupOverlay::Probe(vo)->Remove(IView::Probe(mMultiUserSwitch));
        }
        AddView(IView::Probe(mMultiUserSwitch), 0);
    }
    else if (vp.Get() != (IViewParent*)this && mKeyguardUserSwitcherShowing) {
        RemoveView(IView::Probe(mMultiUserSwitch));
    }
    IView::Probe(mBatteryLevel)->SetVisibility(mBatteryCharging ? IView::VISIBLE : IView::GONE);
}

void CKeyguardStatusBarView::UpdateSystemIconsLayoutParams()
{
    AutoPtr<IViewGroupLayoutParams> lp;
    mSystemIconsSuperContainer->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    Int32 marginEnd = mKeyguardUserSwitcherShowing ? mSystemIconsSwitcherHiddenExpandedMargin : 0;
    Int32 value = 0;
    IViewGroupMarginLayoutParams::Probe(lp)->GetMarginEnd(&value);
    if (marginEnd != value) {
        IViewGroupMarginLayoutParams::Probe(lp)->SetMarginEnd(marginEnd);
        mSystemIconsSuperContainer->SetLayoutParams(lp);
    }
}

ECode CKeyguardStatusBarView::SetListening(
    /* [in] */ Boolean listening)
{
    if (listening == mBatteryListening) {
        return NOERROR;
    }
    mBatteryListening = listening;
    if (mBatteryListening) {
        mBatteryController->AddStateChangedCallback(this);
    }
    else {
        mBatteryController->RemoveStateChangedCallback(this);
    }
    return NOERROR;
}

void CKeyguardStatusBarView::UpdateUserSwitcher()
{
    Boolean keyguardSwitcherAvailable = mKeyguardUserSwitcher != NULL;
    IView::Probe(mMultiUserSwitch)->SetClickable(keyguardSwitcherAvailable);
    IView::Probe(mMultiUserSwitch)->SetFocusable(keyguardSwitcherAvailable);
    mMultiUserSwitch->SetKeyguardMode(keyguardSwitcherAvailable);
}

ECode CKeyguardStatusBarView::SetBatteryController(
    /* [in] */ IBatteryController* batteryController)
{
    mBatteryController = batteryController;
    AutoPtr<IView> view;
    FindViewById(R::id::battery, (IView**)&view);
    IBatteryMeterView::Probe(view)->SetBatteryController(batteryController);
    return NOERROR;
}

ECode CKeyguardStatusBarView::SetUserInfoController(
    /* [in] */ IUserInfoController* userInfoController)
{
    AutoPtr<OnUserInfoChangedListener> listener = new OnUserInfoChangedListener(this);
    userInfoController->AddListener(listener);
    return NOERROR;
}

ECode CKeyguardStatusBarView::OnBatteryLevelChanged(
    /* [in] */ Int32 level,
    /* [in] */ Boolean pluggedIn,
    /* [in] */ Boolean charging)
{
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    String value;
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<IInteger32> obj;
    CInteger32::New(level, (IInteger32**)&obj);
    args->Set(0, obj);
    res->GetString(R::string::battery_level_template, args, &value);
    AutoPtr<ICharSequence> text;
    CString::New(value, (ICharSequence**)&text);
    mBatteryLevel->SetText(text);
    Boolean changed = mBatteryCharging != charging;
    mBatteryCharging = charging;
    if (changed) {
        UpdateVisibilities();
    }
    return NOERROR;
}

ECode CKeyguardStatusBarView::OnPowerSaveChanged()
{
    // could not care less
    return NOERROR;
}

ECode CKeyguardStatusBarView::SetKeyguardUserSwitcher(
    /* [in] */ IKeyguardUserSwitcher* keyguardUserSwitcher)
{
    mKeyguardUserSwitcher = keyguardUserSwitcher;
    mMultiUserSwitch->SetKeyguardUserSwitcher(keyguardUserSwitcher);
    UpdateUserSwitcher();
    return NOERROR;
}

ECode CKeyguardStatusBarView::SetKeyguardUserSwitcherShowing(
    /* [in] */ Boolean showing,
    /* [in] */ Boolean animate)
{
    mKeyguardUserSwitcherShowing = showing;
    if (animate) {
        AnimateNextLayoutChange();
    }
    UpdateVisibilities();
    UpdateSystemIconsLayoutParams();
    return NOERROR;
}

void CKeyguardStatusBarView::AnimateNextLayoutChange()
{
    Int32 systemIconsCurrentX = 0;
    mSystemIconsSuperContainer->GetLeft(&systemIconsCurrentX);
    AutoPtr<IViewParent> vp;
    IView::Probe(mMultiUserSwitch)->GetParent((IViewParent**)&vp);
    Boolean userSwitcherVisible = vp.Get() == (IViewParent*)this;

    AutoPtr<IViewTreeObserver> vto;
    GetViewTreeObserver((IViewTreeObserver**)&vto);
    AutoPtr<OnPreDrawListener> listener = new OnPreDrawListener(this, userSwitcherVisible, systemIconsCurrentX);
    vto->AddOnPreDrawListener(listener);
}

ECode CKeyguardStatusBarView::SetVisibility(
    /* [in] */ Int32 visibility)
{
    RelativeLayout::SetVisibility(visibility);
    if (visibility != IView::VISIBLE) {
        AutoPtr<IViewPropertyAnimator> va;
        mSystemIconsSuperContainer->Animate((IViewPropertyAnimator**)&va);
        va->Cancel();

        va = NULL;
        IView::Probe(mMultiUserSwitch)->Animate((IViewPropertyAnimator**)&va);
        va->Cancel();
        IView::Probe(mMultiUserSwitch)->SetAlpha(1.f);
    }
    return NOERROR;
}

ECode CKeyguardStatusBarView::HasOverlappingRendering(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
