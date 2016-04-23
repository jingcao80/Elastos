#include <Elastos.Droid.Provider.h>
#include "elastos/droid/app/CActivityManagerHelper.h"
#include "elastos/droid/animation/CArgbEvaluator.h"
#include "elastos/droid/animation/CValueAnimatorHelper.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/graphics/drawable/CColorDrawable.h"
#include "elastos/droid/internal/policy/impl/ImmersiveModeConfirmation.h"
#include "elastos/droid/view/WindowManagerPolicyControl.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#include "elastos/droid/utility/CSparseBooleanArray.h"
#include "elastos/droid/view/CWindowManagerLayoutParams.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/view/animation/CDecelerateInterpolator.h"
#include "elastos/droid/view/animation/CAnimationUtils.h"
#include "elastos/droid/widget/CFrameLayoutLayoutParams.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::ITypeEvaluator;
using Elastos::Droid::Animation::CArgbEvaluator;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::IArgbEvaluator;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::CSparseBooleanArray;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewHelper;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::View;
using Elastos::Droid::View::WindowManagerPolicyControl;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::CFrameLayoutLayoutParams;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

//=====================================================================
//                     ImmersiveModeConfirmation::H
//=====================================================================
const Int32 ImmersiveModeConfirmation::H::SHOW;
const Int32 ImmersiveModeConfirmation::H::HIDE;

ImmersiveModeConfirmation::H::H(
    /* [in] */ ImmersiveModeConfirmation* owner)
    :mOwner(owner)
{
}

ECode ImmersiveModeConfirmation::H::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    Int32 what;
    msg->GetWhat(&what);
    switch(what) {
        case SHOW:
            mOwner->HandleShow();
            break;
        case HIDE:
            mOwner->HandleHide();
            break;
    }
    return NOERROR;
}

//=====================================================================
//  ImmersiveModeConfirmation::ClingWindowView::InnerOnClickListener1
//=====================================================================
CAR_INTERFACE_IMPL(ImmersiveModeConfirmation::ClingWindowView::InnerOnClickListener1, Object, IViewOnClickListener)
ImmersiveModeConfirmation::ClingWindowView::InnerOnClickListener1::InnerOnClickListener1(
    /* [in] */ ClingWindowView* owner)
    : mOwner(owner)
{
}

ECode ImmersiveModeConfirmation::ClingWindowView::InnerOnClickListener1::OnClick(
    /* [in] */ IView* v)
{
    VALIDATE_NOT_NULL(v);
    mOwner->mConfirm->Run();
    return NOERROR;
}

//=====================================================================
//ImmersiveModeConfirmation::ClingWindowView::InnerValueAnimatorAnimatorUpdateListener1
//=====================================================================
CAR_INTERFACE_IMPL(ImmersiveModeConfirmation::ClingWindowView::InnerValueAnimatorAnimatorUpdateListener1, Object, IAnimatorUpdateListener)
ImmersiveModeConfirmation::ClingWindowView::InnerValueAnimatorAnimatorUpdateListener1::InnerValueAnimatorAnimatorUpdateListener1(
    /* [in] */ ClingWindowView* owner)
    : mOwner(owner)
{
}

ECode ImmersiveModeConfirmation::ClingWindowView::InnerValueAnimatorAnimatorUpdateListener1::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    VALIDATE_NOT_NULL(animation);
    AutoPtr<IInterface> iif;
    animation->GetAnimatedValue((IInterface**)&iif);
    IInteger32* iint32 = IInteger32::Probe(iif);
    Int32 c;
    iint32->GetValue(&c);
    mOwner->mColor->SetColor(c);
    return NOERROR;
}
//=====================================================================
//               ImmersiveModeConfirmation::InnerRunnable
//=====================================================================
CAR_INTERFACE_IMPL(ImmersiveModeConfirmation::ClingWindowView::InnerRunnable, Object, IRunnable)
ImmersiveModeConfirmation::ClingWindowView::InnerRunnable::InnerRunnable(
    /* [in] */ ClingWindowView* owner)
    : mOwner(owner)
{
}

ECode ImmersiveModeConfirmation::ClingWindowView::InnerRunnable::Run()
{
    IViewParent* vp = IViewParent::Probe(mOwner->mClingLayout);
    AutoPtr<IViewParent> vParent;
    vp->GetParent((IViewParent**)&vParent);
    if (mOwner->mClingLayout != NULL && vParent != NULL)
    {
        AutoPtr<IFrameLayoutLayoutParams> iflp;
        mOwner->mOwner->GetBubbleLayoutParams((IFrameLayoutLayoutParams**)&iflp);
        IView* view = IView::Probe(mOwner->mClingLayout);
        IViewGroupLayoutParams* vgLP = IViewGroupLayoutParams::Probe(iflp);
        view->SetLayoutParams(vgLP);
    }
    return NOERROR;
}

//=====================================================================
//          ImmersiveModeConfirmation::InnerBroadcastReceiver
//=====================================================================
ImmersiveModeConfirmation::ClingWindowView::InnerBroadcastReceiver::InnerBroadcastReceiver(
    /* [in] */ ClingWindowView* owner)
    : mOwner(owner)
{
}

ECode ImmersiveModeConfirmation::ClingWindowView::InnerBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_CONFIGURATION_CHANGED))
    {
        Boolean res;
        mOwner->Post(mOwner->mUpdateLayoutRunnable, &res);
    }
    return NOERROR;
}


//=====================================================================
//              ImmersiveModeConfirmation::ClingWindowView
//=====================================================================
const Int32 ImmersiveModeConfirmation::ClingWindowView::BGCOLOR;
const Int32 ImmersiveModeConfirmation::ClingWindowView::OFFSET_DP;

ImmersiveModeConfirmation::ClingWindowView::ClingWindowView(
    /* [in] */ ImmersiveModeConfirmation* owner,
    /* [in] */ IContext* context,
    /* [in] */ IRunnable* confirm)
    :mOwner(owner)
{
    FrameLayout::constructor(context);
    CColorDrawable::New(0, (IColorDrawable**)&mColor);
    mUpdateLayoutRunnable = new ClingWindowView::InnerRunnable(this);
    mReceiver = new ClingWindowView::InnerBroadcastReceiver(this);
    mConfirm = confirm;
    SetClickable(TRUE);
    IDrawable* da = IDrawable::Probe(mColor);
    SetBackground(da);
}

ECode ImmersiveModeConfirmation::ClingWindowView::OnAttachedToWindow()
{
    FrameLayout::OnAttachedToWindow();

    AutoPtr<IDisplayMetrics> metrics;
    CDisplayMetrics::New((IDisplayMetrics**)&metrics);
    AutoPtr<IDisplay> display;
    mOwner->mWindowManager->GetDefaultDisplay((IDisplay**)&display);
    display->GetMetrics(metrics);
    Float density;
    metrics->GetDensity(&density);

    // create the confirmation cling
    AutoPtr<IView> view;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    View::Inflate(context, R::layout::immersive_mode_cling, NULL, (IView**)&view);
    mClingLayout = IViewGroup::Probe(view);

    AutoPtr<IButton> ok;
    AutoPtr<IView> buttonView;
    view->FindViewById(R::id::ok, (IView**)&buttonView);
    ok = IButton::Probe(buttonView);
    AutoPtr<IViewOnClickListener> onClickL = new ClingWindowView::InnerOnClickListener1(this);
    buttonView->SetOnClickListener(onClickL);
    AutoPtr<IFrameLayoutLayoutParams> iflp;
    mOwner->GetBubbleLayoutParams((IFrameLayoutLayoutParams**)&iflp);
    IViewGroupLayoutParams* vgLP = IViewGroupLayoutParams::Probe(iflp);
    AddView(view, vgLP);

    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    Boolean isHighEndGfx;
    amHelper->IsHighEndGfx(&isHighEndGfx);
    if (isHighEndGfx) {
        AutoPtr<IView> bubble;
        view->FindViewById(R::id::text, (IView**)&bubble);
        bubble->SetAlpha(0);
        bubble->SetTranslationY(-OFFSET_DP*density);
        AutoPtr<IViewPropertyAnimator> viewPropertyAnimator;
        bubble->Animate((IViewPropertyAnimator**)&viewPropertyAnimator);
        viewPropertyAnimator->Alpha(1);
        viewPropertyAnimator->TranslationY(0);
        viewPropertyAnimator->SetDuration(300);
        AutoPtr<IDecelerateInterpolator> decelerateInterpolator;
        CDecelerateInterpolator::New((IDecelerateInterpolator**)&decelerateInterpolator);
        ITimeInterpolator* timeInterpolator = ITimeInterpolator::Probe(decelerateInterpolator);
        viewPropertyAnimator->SetInterpolator(timeInterpolator);
        viewPropertyAnimator->Start();

        buttonView->SetAlpha(0);
        buttonView->SetTranslationY(-OFFSET_DP*density);
        AutoPtr<IViewPropertyAnimator> okViewPropertyAnimator;
        buttonView->Animate((IViewPropertyAnimator**)&okViewPropertyAnimator);
        okViewPropertyAnimator->Alpha(1);
        okViewPropertyAnimator->TranslationY(0);
        okViewPropertyAnimator->SetDuration(300);
        okViewPropertyAnimator->SetStartDelay(200);
        AutoPtr<IDecelerateInterpolator> decelerateInterpolator2;
        CDecelerateInterpolator::New((IDecelerateInterpolator**)&decelerateInterpolator2);
        ITimeInterpolator* timeInterpolator2 = ITimeInterpolator::Probe(decelerateInterpolator2);
        okViewPropertyAnimator->SetInterpolator(timeInterpolator2);
        okViewPropertyAnimator->Start();

        AutoPtr<IArgbEvaluator> argbEvaluator;
        CArgbEvaluator::New((IArgbEvaluator**)&argbEvaluator);
        AutoPtr<ITypeEvaluator> typeEvaluator = ITypeEvaluator::Probe(argbEvaluator);
        AutoPtr<IValueAnimatorHelper> valueAniHelper;
        CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&valueAniHelper);
        valueAniHelper->OfObject(typeEvaluator, NULL, /*BGCOLOR,*/ (IValueAnimator**)&mColorAnim);//TODO in java here the length of arguments is variable
        AutoPtr<IAnimatorUpdateListener> lis = new ClingWindowView::InnerValueAnimatorAnimatorUpdateListener1(this);
        mColorAnim->AddUpdateListener(lis);
        IAnimator* ani = IAnimator::Probe(mColorAnim);
        ani->SetDuration(1000);
        ani->Start();
    } else {
        mColor->SetColor(BGCOLOR);
    }

    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(IIntent::ACTION_CONFIGURATION_CHANGED, (IIntentFilter**)&intentFilter);
    AutoPtr<IIntent> intent;
    mOwner->mContext->RegisterReceiver(mReceiver, intentFilter, (IIntent**)&intent);
    return NOERROR;
}

ECode ImmersiveModeConfirmation::ClingWindowView::OnDetachedFromWindow()
{
    mOwner->mContext->UnregisterReceiver(mReceiver);
    return NOERROR;
}

ECode ImmersiveModeConfirmation::ClingWindowView::OnTouchEvent(
    /* [in] */ IMotionEvent* motion,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = TRUE;
    return NOERROR;
}

//=====================================================================
//              ImmersiveModeConfirmation::InnerRunnable1
//=====================================================================
CAR_INTERFACE_IMPL(ImmersiveModeConfirmation::InnerRunnable1, Object, IRunnable)
ImmersiveModeConfirmation::InnerRunnable1::InnerRunnable1(
    /* [in] */ ImmersiveModeConfirmation* owner)
    : mOwner(owner)
{
}

ECode ImmersiveModeConfirmation::InnerRunnable1::Run()
{
    if (DEBUG) Slogger::D(TAG, "mConfirm.run()");
    if (!mOwner->mConfirmed)
    {
        mOwner->mConfirmed = TRUE;
        mOwner->SaveSetting();
    }
    mOwner->HandleHide();
    return NOERROR;
}

//=====================================================================
//                      ImmersiveModeConfirmation
//=====================================================================
const String ImmersiveModeConfirmation::TAG("ImmersiveModeConfirmation");
const Boolean ImmersiveModeConfirmation::DEBUG = false;
const Boolean ImmersiveModeConfirmation::DEBUG_SHOW_EVERY_TIME = false;
const String ImmersiveModeConfirmation::CONFIRMED("confirmed");

CAR_INTERFACE_IMPL(ImmersiveModeConfirmation, Object, IImmersiveModeConfirmation)

ImmersiveModeConfirmation::ImmersiveModeConfirmation()
{
}

ECode ImmersiveModeConfirmation::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    mHandler = new H(this);
    mHandler->constructor();

    mConfirm = new InnerRunnable1(this);
    mShowDelayMs = GetNavBarExitDuration() * 3;
    AutoPtr<IInterface> iService;
    mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&iService);
    mWindowManager = IWindowManager::Probe(iService);
    return NOERROR;
}

ECode ImmersiveModeConfirmation::LoadSetting(
    /* [in] */ Int32 currentUserId)
{
    mConfirmed = FALSE;
    mCurrentUserId = currentUserId;
    String value(NULL);
    //try {
    AutoPtr<IContentResolver> cResolver;
    mContext->GetContentResolver((IContentResolver**)&cResolver);
    Settings::Secure::GetStringForUser(cResolver, ISettingsSecure::IMMERSIVE_MODE_CONFIRMATIONS, IUserHandle::USER_CURRENT, &value);
    mConfirmed = CONFIRMED.Equals(value);
    if (DEBUG) Slogger::D(TAG, "Loaded mConfirmed=%d", mConfirmed);
    //} catch (Throwable t) {
    //    Slog.w(TAG, "Error loading confirmations, value=" + value, t);
    //}
    return NOERROR;
}

ECode ImmersiveModeConfirmation::ImmersiveModeChanged(
    /* [in] */ const String& pkg,
    /* [in] */ Boolean isImmersiveMode,
    /* [in] */ Boolean userSetupComplete)
{
    mHandler->RemoveMessages(H::SHOW);
    if (isImmersiveMode)
    {
        Boolean disabled = WindowManagerPolicyControl::DisableImmersiveConfirmation(pkg);
        if (DEBUG) Slogger::D(TAG, "immersiveModeChanged() disabled=%d mConfirmed=%d", disabled, mConfirmed);
        if (!disabled && (DEBUG_SHOW_EVERY_TIME || !mConfirmed) && userSetupComplete)
        {
            Boolean res;
            mHandler->SendEmptyMessageDelayed(H::SHOW, mShowDelayMs, &res);
        }
    } else {
        Boolean res;
        mHandler->SendEmptyMessage(H::HIDE, &res);
    }
    return NOERROR;
}

ECode ImmersiveModeConfirmation::ConfirmCurrentPrompt()
{
    if (mClingWindow != NULL)
    {
        if (DEBUG) Slogger::D(TAG, "confirmCurrentPrompt()");
        Boolean res;
        mHandler->Post(mConfirm, &res);
    }
    return NOERROR;
}

ECode ImmersiveModeConfirmation::GetClingWindowLayoutParams(
    /* [out] */ IWindowManagerLayoutParams** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IWindowManagerLayoutParams> lp;
    CWindowManagerLayoutParams::New(
            IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::MATCH_PARENT,
            IWindowManagerLayoutParams::TYPE_TOAST,
            0
            | IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN
            | IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
            | IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED
            ,
            IPixelFormat::TRANSLUCENT,
            (IWindowManagerLayoutParams**)&lp);
    Int32 privateFlags;
    lp->GetPrivateFlags(&privateFlags);
    lp->SetPrivateFlags(privateFlags |IWindowManagerLayoutParams::PRIVATE_FLAG_SHOW_FOR_ALL_USERS);
    AutoPtr<ICharSequence> title;
    CString::New(String("ImmersiveModeConfirmation"), (ICharSequence**)&title);
    lp->SetTitle(title);
    lp->SetWindowAnimations(R::style::Animation_RecentApplications);
    lp->SetGravity(IGravity::FILL);

    *result = lp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ImmersiveModeConfirmation::GetBubbleLayoutParams(
    /* [out] */ IFrameLayoutLayoutParams** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    Int32 pixelSize;
    resources->GetDimensionPixelSize(R::dimen::immersive_mode_cling_width, &pixelSize);
    AutoPtr<IFrameLayoutLayoutParams> fl;
    CFrameLayoutLayoutParams::New(
            pixelSize,
            IViewGroupLayoutParams::WRAP_CONTENT,
            IGravity::CENTER_HORIZONTAL | IGravity::TOP,
            (IFrameLayoutLayoutParams**)&fl);

    *result = fl;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

Int64 ImmersiveModeConfirmation::GetNavBarExitDuration()
{
    AutoPtr<IAnimationUtils> animationUtils;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&animationUtils);
    AutoPtr<IAnimation> exit;
    animationUtils->LoadAnimation(mContext, R::anim::dock_bottom_exit, (IAnimation**)&exit);
    Int64 time = 0;
    if (exit != NULL) {
        exit->GetDuration(&time);
    }

    return time;
}

void ImmersiveModeConfirmation::SaveSetting()
{
    if (DEBUG) Slogger::D(TAG, "saveSetting()");
    //try {
    String value = mConfirmed ? CONFIRMED : String(NULL);
    AutoPtr<IContentResolver> cResolver;
    mContext->GetContentResolver((IContentResolver**)&cResolver);
    Boolean res = FALSE;
    Settings::Secure::PutStringForUser(cResolver, ISettingsSecure::IMMERSIVE_MODE_CONFIRMATIONS, value, IUserHandle::USER_CURRENT, &res);
    if (DEBUG) Slogger::D(TAG, "Saved value=%s", value.string());
    //} catch (Throwable t) {
    //    Slog.w(TAG, "Error saving confirmations, mConfirmed=" + mConfirmed, t);
    //}
}

void ImmersiveModeConfirmation::HandleHide()
{
    if (mClingWindow != NULL)
    {
        if (DEBUG) Slogger::D(TAG, "Hiding immersive mode confirmation");
        IViewManager* vm = IViewManager::Probe(mWindowManager);
        IView* view = IView::Probe(mClingWindow);
        vm->RemoveView(view);
        mClingWindow = NULL;
    }
}

void ImmersiveModeConfirmation::HandleShow()
{
    if (DEBUG) Slogger::D(TAG, "Showing immersive mode confirmation");

    mClingWindow = new ClingWindowView(this, mContext, mConfirm);

    // we will be hiding the nav bar, so layout as if it's already hidden
    mClingWindow->SetSystemUiVisibility(
            IView::SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION | IView::SYSTEM_UI_FLAG_LAYOUT_STABLE);

    // show the confirmation
    AutoPtr<IWindowManagerLayoutParams> lp;
    GetClingWindowLayoutParams((IWindowManagerLayoutParams**)&lp);
    IViewGroupLayoutParams* vglp = IViewGroupLayoutParams::Probe(lp);
    IViewManager* vm = IViewManager::Probe(mWindowManager);
    IView* view = IView::Probe(mClingWindow);
    vm->AddView(view, vglp);
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

