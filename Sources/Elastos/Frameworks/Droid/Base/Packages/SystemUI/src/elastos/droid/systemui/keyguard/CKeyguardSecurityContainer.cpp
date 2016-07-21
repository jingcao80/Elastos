
#include "elastos/droid/systemui/keyguard/CKeyguardSecurityContainer.h"
#include "elastos/droid/systemui/keyguard/KeyguardSecurityModel.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitor.h"
#include "../R.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include <Elastos.Droid.Internal.h>
#include <elastos/droid/view/LayoutInflater.h>
#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Widget::IViewAnimator;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

static const String TAG("KeyguardSecurityView");

//===========================================================================
// CKeyguardSecurityContainer::MyKeyguardSecurityCallback
//===========================================================================
ECode CKeyguardSecurityContainer::MyKeyguardUpdateMonitorCallback::OnUserSwitchComplete(
    /* [in] */ Int32 userId)
{
    IViewGroup::Probe(mHost->mSecurityViewFlipper)->RemoveAllViews();
    mHost->mCurrentSecuritySelection = SecurityMode_Invalid;
    mHost->ShowPrimarySecurityScreen(FALSE);
    return NOERROR;
}

//===========================================================================
// CKeyguardSecurityContainer::MyKeyguardSecurityCallback
//===========================================================================
CAR_INTERFACE_IMPL(CKeyguardSecurityContainer::MyKeyguardSecurityCallback, Object, IKeyguardSecurityCallback)

ECode CKeyguardSecurityContainer::MyKeyguardSecurityCallback::UserActivity()
{
    if (mHost->mSecurityCallback != NULL) {
        mHost->mSecurityCallback->UserActivity();
    }
    return NOERROR;
}

ECode CKeyguardSecurityContainer::MyKeyguardSecurityCallback::Dismiss(
    /* [in] */ Boolean authenticated)
{
    Boolean bval;
    return mHost->mSecurityCallback->Dismiss(authenticated, &bval);
}

ECode CKeyguardSecurityContainer::MyKeyguardSecurityCallback::IsVerifyUnlockOnly(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->mIsVerifyUnlockOnly;
    return NOERROR;
}

ECode CKeyguardSecurityContainer::MyKeyguardSecurityCallback::ReportUnlockAttempt(
    /* [in] */ Boolean success)
{
    AutoPtr<IKeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mHost->mContext);
    if (success) {
        monitor->ClearFailedUnlockAttempts();
        mHost->mLockPatternUtils->ReportSuccessfulPasswordAttempt();
    }
    else {
        if (mHost->mCurrentSecuritySelection == SecurityMode_Biometric) {
            monitor->ReportFailedBiometricUnlockAttempt();
        }
        else {
            mHost->ReportFailedUnlockAttempt();
        }
    }
    return NOERROR;
}

ECode CKeyguardSecurityContainer::MyKeyguardSecurityCallback::ShowBackupSecurity()
{
    return mHost->ShowBackupSecurityScreen();
}

//===========================================================================
// CKeyguardSecurityContainer::NullKeyguardSecurityCallback
//===========================================================================
CAR_INTERFACE_IMPL(CKeyguardSecurityContainer::NullKeyguardSecurityCallback, Object, IKeyguardSecurityCallback)

ECode CKeyguardSecurityContainer::NullKeyguardSecurityCallback::UserActivity()
{
    return NOERROR;
}

ECode CKeyguardSecurityContainer::NullKeyguardSecurityCallback::Dismiss(
    /* [in] */ Boolean authenticated)
{
    return NOERROR;
}

ECode CKeyguardSecurityContainer::NullKeyguardSecurityCallback::IsVerifyUnlockOnly(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode CKeyguardSecurityContainer::NullKeyguardSecurityCallback::ReportUnlockAttempt(
    /* [in] */ Boolean success)
{
    return NOERROR;
}

ECode CKeyguardSecurityContainer::NullKeyguardSecurityCallback::ShowBackupSecurity()
{
    return NOERROR;
}

//===========================================================================
// CKeyguardSecurityContainer
//===========================================================================
const Boolean CKeyguardSecurityContainer::DEBUG = TRUE;

CAR_OBJECT_IMPL(CKeyguardSecurityContainer)

CAR_INTERFACE_IMPL_2(CKeyguardSecurityContainer, FrameLayout, IKeyguardSecurityContainer, IKeyguardSecurityView)

CKeyguardSecurityContainer::CKeyguardSecurityContainer()
    : mEnableFallback(FALSE)
    , mIsVerifyUnlockOnly(FALSE)
    , mIsBouncing(FALSE)
{
    mCurrentSecuritySelection = SecurityMode_Invalid;
}

ECode CKeyguardSecurityContainer::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL, 0);
}

ECode CKeyguardSecurityContainer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CKeyguardSecurityContainer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    FrameLayout::constructor(context, attrs, defStyleAttr);
    CLockPatternUtils::New(context, (ILockPatternUtils**)&mLockPatternUtils);
    mSecurityModel = new KeyguardSecurityModel(context);
    mUpdateMonitor = KeyguardUpdateMonitor::GetInstance(mContext);
    mUpdateMonitorCallbacks = new MyKeyguardUpdateMonitorCallback(this);
    mCallback = new MyKeyguardSecurityCallback(this);
    mNullCallback = new NullKeyguardSecurityCallback(this);
    return NOERROR;
}

ECode CKeyguardSecurityContainer::SetSecurityCallback(
    /* [in] */ IKeyguardSecurityContainerSecurityCallback* callback)
{
    mSecurityCallback = callback;
    return NOERROR;
}

ECode CKeyguardSecurityContainer::OnAttachedToWindow()
{
    FrameLayout::OnAttachedToWindow();
    mUpdateMonitor->RegisterCallback(mUpdateMonitorCallbacks);
    return NOERROR;
}

ECode CKeyguardSecurityContainer::OnDetachedFromWindow()
{
    FrameLayout::OnDetachedFromWindow();
    mUpdateMonitor->RemoveCallback(mUpdateMonitorCallbacks);
    return NOERROR;
}

ECode CKeyguardSecurityContainer::OnResume(
    /* [in] */ Int32 reason)
{
    if (mCurrentSecuritySelection != SecurityMode_None) {
        AutoPtr<IKeyguardSecurityView> view;
        GetSecurityView(mCurrentSecuritySelection, (IKeyguardSecurityView**)&view);
        view->OnResume(reason);
    }
    return NOERROR;
}

ECode CKeyguardSecurityContainer::OnPause()
{
    if (mCurrentSecuritySelection != SecurityMode_None) {
        AutoPtr<IKeyguardSecurityView> view;
        GetSecurityView(mCurrentSecuritySelection, (IKeyguardSecurityView**)&view);
        view->OnPause();
    }
    return NOERROR;
}

ECode CKeyguardSecurityContainer::StartAppearAnimation()
{
    if (mCurrentSecuritySelection != SecurityMode_None) {
        AutoPtr<IKeyguardSecurityView> view;
        GetSecurityView(mCurrentSecuritySelection, (IKeyguardSecurityView**)&view);
        view->StartAppearAnimation();
    }
    return NOERROR;
}

ECode CKeyguardSecurityContainer::StartDisappearAnimation(
    /* [in] */ IRunnable* onFinishRunnable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mCurrentSecuritySelection != SecurityMode_None) {
        AutoPtr<IKeyguardSecurityView> view;
        GetSecurityView(mCurrentSecuritySelection, (IKeyguardSecurityView**)&view);
        view->StartDisappearAnimation(onFinishRunnable, result);
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CKeyguardSecurityContainer::UpdateSecurityViews(
    /* [in] */ Boolean isBouncing)
{
    Int32 children;
    IViewGroup* vg = IViewGroup::Probe(mSecurityViewFlipper);
    vg->GetChildCount(&children);
    for (Int32 i = 0; i < children; i++) {
        AutoPtr<IView> view;
        vg->GetChildAt(i, (IView**)&view);
        UpdateSecurityView(view, isBouncing);
    }
    return NOERROR;
}

ECode CKeyguardSecurityContainer::AnnounceCurrentSecurityMethod()
{
    AutoPtr<IKeyguardSecurityView> view;
    GetSecurityView(mCurrentSecuritySelection, (IKeyguardSecurityView**)&view);
    AutoPtr<IView> v = IView::Probe(view);
    if (v != NULL) {
        AutoPtr<ICharSequence> cchar;
        v->GetContentDescription((ICharSequence**)&cchar);
        v->AnnounceForAccessibility(cchar);
    }
    return NOERROR;
}

ECode CKeyguardSecurityContainer::GetSecurityView(
    /* [in] */ SecurityMode securityMode,
    /* [out] */ IKeyguardSecurityView** result)
{
    VALIDATE_NOT_NULL(result)

    IViewGroup* vg = IViewGroup::Probe(mSecurityViewFlipper);
    Int32 securityViewIdForMode = GetSecurityViewIdForMode(securityMode);
    AutoPtr<IKeyguardSecurityView> view;
    Int32 children;
    vg->GetChildCount(&children);
    for (Int32 child = 0; child < children; child++) {
        AutoPtr<IView> v;
        vg->GetChildAt(child, (IView**)&v);
        Int32 id;
        v->GetId(&id);
        if (id == securityViewIdForMode) {
            view = IKeyguardSecurityView::Probe(v);
            break;
        }
    }
    Int32 layoutId = GetLayoutIdFor(securityMode);
    if (view == NULL && layoutId != 0) {
        AutoPtr<ILayoutInflater> inflater;
        LayoutInflater::From(mContext, (ILayoutInflater**)&inflater);
        if (DEBUG) Logger::V(TAG, "inflating id = %d", layoutId);
        AutoPtr<IView> v;
        inflater->Inflate(layoutId, vg, FALSE, (IView**)&v);
        vg->AddView(v);
        UpdateSecurityView(v, mIsBouncing);
        view = IKeyguardSecurityView::Probe(v);
    }

    *result = view;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void CKeyguardSecurityContainer::UpdateSecurityView(
    /* [in] */ IView* view,
    /* [in] */ Boolean isBouncing)
{
    mIsBouncing = isBouncing;
    if (IKeyguardSecurityView::Probe(view) != NULL) {
        AutoPtr<IKeyguardSecurityView> ksv = IKeyguardSecurityView::Probe(view);
        ksv->SetKeyguardCallback(mCallback);
        ksv->SetLockPatternUtils(mLockPatternUtils);
        if (isBouncing) {
            ksv->ShowBouncer(0);
        }
        else {
            ksv->HideBouncer(0);
        }
    }
    else {
        Logger::W(TAG, "View %s is not a KeyguardSecurityView", TO_CSTR(view));
    }
}

ECode CKeyguardSecurityContainer::OnFinishInflate()
{
    AutoPtr<IView> view;
    FindViewById(R::id::view_flipper, (IView**)&view);
    mSecurityViewFlipper = IKeyguardSecurityViewFlipper::Probe(view);
    return IKeyguardSecurityView::Probe(mSecurityViewFlipper)->SetLockPatternUtils(mLockPatternUtils);
}

ECode CKeyguardSecurityContainer::SetLockPatternUtils(
    /* [in] */ ILockPatternUtils* utils)
{
    mLockPatternUtils = utils;
    mSecurityModel->SetLockPatternUtils(utils);
    return IKeyguardSecurityView::Probe(mSecurityViewFlipper)->SetLockPatternUtils(utils);
}

void CKeyguardSecurityContainer::ShowDialog(
    /* [in] */ const String& title,
    /* [in] */ const String& message)
{
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(mContext, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(CoreUtils::Convert(title));
    builder->SetMessage(CoreUtils::Convert(message));
    builder->SetNeutralButton(R::string::ok, NULL);
    AutoPtr<IAlertDialog> dialog;
    builder->Create((IAlertDialog**)&dialog);

    if (IActivity::Probe(mContext) == NULL) {
        AutoPtr<IWindow> window;
        IDialog::Probe(dialog)->GetWindow((IWindow**)&window);
        window->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);
    }
    IDialog::Probe(dialog)->Show();
}

void CKeyguardSecurityContainer::ShowTimeoutDialog()
{
    Int32 timeoutInSeconds = (Int32) ILockPatternUtils::FAILED_ATTEMPT_TIMEOUT_MS / 1000;
    Int32 messageId = 0;

    SecurityMode mode;
    mSecurityModel->GetSecurityMode(&mode);
    switch (mode) {
        case SecurityMode_Pattern:
            messageId = R::string::kg_too_many_failed_pattern_attempts_dialog_message;
            break;
        case SecurityMode_PIN:
            messageId = R::string::kg_too_many_failed_pin_attempts_dialog_message;
            break;
        case SecurityMode_Password:
            messageId = R::string::kg_too_many_failed_password_attempts_dialog_message;
            break;
        // These don't have timeout dialogs.
        case SecurityMode_Account:
        case SecurityMode_Biometric:
        case SecurityMode_Invalid:
        case SecurityMode_None:
        case SecurityMode_SimPin:
        case SecurityMode_SimPuk:
            break;
    }

    if (messageId != 0) {
        AutoPtr<IKeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
        Int32 times;
        monitor->GetFailedUnlockAttempts(&times);
        AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
        args->Set(0, CoreUtils::Convert(times));
        args->Set(1, CoreUtils::Convert(timeoutInSeconds));
        String message;
        mContext->GetString(messageId, args, &message);
        ShowDialog(String(NULL), message);
    }
}

void CKeyguardSecurityContainer::ShowAlmostAtWipeDialog(
    /* [in] */ Int32 attempts,
    /* [in] */ Int32 remaining)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, CoreUtils::Convert(attempts));
    args->Set(1, CoreUtils::Convert(remaining));
    String message;
    mContext->GetString(R::string::kg_failed_attempts_almost_at_wipe, args, &message);
    ShowDialog(String(NULL), message);
}

void CKeyguardSecurityContainer::ShowWipeDialog(
    /* [in] */ Int32 attempts)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, CoreUtils::Convert(attempts));
    String message;
    mContext->GetString(R::string::kg_failed_attempts_now_wiping, args, &message);
    ShowDialog(String(NULL), message);
}

void CKeyguardSecurityContainer::ShowAlmostAtAccountLoginDialog()
{
    Int32 timeoutInSeconds = (Int32) ILockPatternUtils::FAILED_ATTEMPT_TIMEOUT_MS / 1000;
    Int32 count = ILockPatternUtils::FAILED_ATTEMPTS_BEFORE_RESET
            - ILockPatternUtils::FAILED_ATTEMPTS_BEFORE_TIMEOUT;
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, CoreUtils::Convert(count));
    args->Set(1, CoreUtils::Convert(ILockPatternUtils::FAILED_ATTEMPTS_BEFORE_TIMEOUT));
    args->Set(2, CoreUtils::Convert(timeoutInSeconds));
    String message;
    mContext->GetString(R::string::kg_failed_attempts_almost_at_login, args, &message);
    ShowDialog(String(NULL), message);
}

void CKeyguardSecurityContainer::ReportFailedUnlockAttempt()
{
    AutoPtr<IKeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
    Int32 tmp;
    monitor->GetFailedUnlockAttempts(&tmp);
    Int32 failedAttempts = tmp + 1; // +1 for this time

    if (DEBUG) Logger::D(TAG, "reportFailedPatternAttempt: # %d", failedAttempts);

    SecurityMode mode;
    mSecurityModel->GetSecurityMode(&mode);
    Boolean usingPattern = mode == SecurityMode_Pattern;

    AutoPtr<IDevicePolicyManager> manager;
    mLockPatternUtils->GetDevicePolicyManager((IDevicePolicyManager**)&manager);
    Int32 user;
    mLockPatternUtils->GetCurrentUser(&user);
    Int32 failedAttemptsBeforeWipe = 0;
    manager->GetMaximumFailedPasswordsForWipe(NULL, user, &failedAttemptsBeforeWipe);

    Int32 failedAttemptWarning = ILockPatternUtils::FAILED_ATTEMPTS_BEFORE_RESET
            - ILockPatternUtils::FAILED_ATTEMPTS_BEFORE_TIMEOUT;

    Int32 remainingBeforeWipe = failedAttemptsBeforeWipe > 0 ?
            (failedAttemptsBeforeWipe - failedAttempts)
            : Elastos::Core::Math::INT32_MAX_VALUE; // because DPM returns 0 if no restriction

    Boolean showTimeout = FALSE;
    if (remainingBeforeWipe < ILockPatternUtils::FAILED_ATTEMPTS_BEFORE_WIPE_GRACE) {
        // The user has installed a DevicePolicyManager that requests a user/profile to be wiped
        // N attempts. Once we get below the grace period, we post this dialog every time as a
        // clear warning until the deletion fires.
        //
        // TODO: Show a different dialog depending on whether the device will be completely
        // wiped (i.e. policy is set for the primary profile of the USER_OWNER) or a single
        // secondary user or managed profile will be removed.
        if (remainingBeforeWipe > 0) {
            ShowAlmostAtWipeDialog(failedAttempts, remainingBeforeWipe);
        }
        else {
            // Too many attempts. The device will be wiped shortly.
            Slogger::I(TAG, "Too many unlock attempts; device will be wiped!");
            ShowWipeDialog(failedAttempts);
        }
    } else {
        showTimeout =
            (failedAttempts % ILockPatternUtils::FAILED_ATTEMPTS_BEFORE_TIMEOUT) == 0;
        if (usingPattern && mEnableFallback) {
            if (failedAttempts == failedAttemptWarning) {
                ShowAlmostAtAccountLoginDialog();
                showTimeout = FALSE; // don't show both dialogs
            }
            else if (failedAttempts >= ILockPatternUtils::FAILED_ATTEMPTS_BEFORE_RESET) {
                mLockPatternUtils->SetPermanentlyLocked(TRUE);
                ShowSecurityScreen(SecurityMode_Account);
                // don't show timeout dialog because we show account unlock screen next
                showTimeout = FALSE;
            }
        }
    }
    monitor->ReportFailedUnlockAttempt();
    mLockPatternUtils->ReportFailedPasswordAttempt();
    if (showTimeout) {
        ShowTimeoutDialog();
    }
}

ECode CKeyguardSecurityContainer::ShowPrimarySecurityScreen(
    /* [in] */ Boolean turningOff)
{
    SecurityMode securityMode;
    mSecurityModel->GetSecurityMode(&securityMode);
    if (DEBUG) Logger::V(TAG, "showPrimarySecurityScreen(turningOff=%d)", turningOff);
    AutoPtr<IKeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
    Boolean res;
    if (!turningOff && (monitor->IsAlternateUnlockEnabled(&res), res)) {
        // If we're not turning off, then allow biometric alternate.
        // We'll reload it when the device comes back on.
        mSecurityModel->GetAlternateFor(securityMode, &securityMode);
    }
    return ShowSecurityScreen(securityMode);
}

ECode CKeyguardSecurityContainer::ShowBackupSecurityScreen()
{
    if (DEBUG) Logger::D(TAG, "showBackupSecurity()");
    SecurityMode backup;
    mSecurityModel->GetBackupSecurityMode(mCurrentSecuritySelection, &backup);
    ShowSecurityScreen(backup);
    return NOERROR;
}

ECode CKeyguardSecurityContainer::ShowNextSecurityScreenOrFinish(
    /* [in] */ Boolean authenticated,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (DEBUG) Logger::D(TAG, "showNextSecurityScreenOrFinish(%d)", authenticated);
    Boolean finish = FALSE;
    Int32 user;
    mLockPatternUtils->GetCurrentUser(&user);
    Boolean res;
    if (mUpdateMonitor->GetUserHasTrust(user, &res), res) {
        finish = TRUE;
    }
    else if (SecurityMode_None == mCurrentSecuritySelection) {
        SecurityMode securityMode;
        mSecurityModel->GetSecurityMode(&securityMode);
        // Allow an alternate, such as biometric unlock
        mSecurityModel->GetAlternateFor(securityMode, &securityMode);
        if (SecurityMode_None == securityMode) {
            finish = TRUE; // no security required
        }
        else {
            ShowSecurityScreen(securityMode); // switch to the alternate security view
        }
    }
    else if (authenticated) {
        switch (mCurrentSecuritySelection) {
            case SecurityMode_Pattern:
            case SecurityMode_Password:
            case SecurityMode_PIN:
            case SecurityMode_Account:
            case SecurityMode_Biometric:
                finish = TRUE;
                break;

            case SecurityMode_SimPin:
            case SecurityMode_SimPuk:
                // Shortcut for SIM PIN/PUK to go to directly to user's security screen or home
                SecurityMode securityMode;
                mSecurityModel->GetSecurityMode(&securityMode);
                if (securityMode != SecurityMode_None) {
                    ShowSecurityScreen(securityMode);
                }
                else {
                    finish = TRUE;
                }
                break;

            default:
                Logger::V(TAG, "Bad security screen %d, fail safe", mCurrentSecuritySelection);
                ShowPrimarySecurityScreen(FALSE);
                break;
        }
    }
    else {
        ShowPrimarySecurityScreen(false);
    }
    if (finish) {
        mSecurityCallback->Finish();
    }
    *result = finish;
    return NOERROR;
}

ECode CKeyguardSecurityContainer::ShowSecurityScreen(
    /* [in] */ SecurityMode securityMode)
{
    if (DEBUG) Logger::D(TAG, "showSecurityScreen(%d)", securityMode);

    if (securityMode == mCurrentSecuritySelection) return NOERROR;

    AutoPtr<IKeyguardSecurityView> oldView, newView;
    GetSecurityView(mCurrentSecuritySelection, (IKeyguardSecurityView**)&oldView);
    GetSecurityView(securityMode, (IKeyguardSecurityView**)&newView);

    // Emulate Activity life cycle
    if (oldView != NULL) {
        oldView->OnPause();
        oldView->SetKeyguardCallback(mNullCallback); // ignore requests from old view
    }
    if (securityMode != SecurityMode_None) {
        newView->OnResume(IKeyguardSecurityView::VIEW_REVEALED);
        newView->SetKeyguardCallback(mCallback);
    }

    IViewGroup* vg = IViewGroup::Probe(mSecurityViewFlipper);
    // Find and show this child.
    Int32 childCount;
    vg->GetChildCount(&childCount);

    Int32 securityViewIdForMode = GetSecurityViewIdForMode(securityMode);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> _view;
        vg->GetChildAt(i, (IView**)&_view);
        Int32 id;
        _view->GetId(&id);
        if (id == securityViewIdForMode) {
            IViewAnimator::Probe(mSecurityViewFlipper)->SetDisplayedChild(i);
            break;
        }
    }

    mCurrentSecuritySelection = securityMode;
    Boolean res;
    mSecurityCallback->OnSecurityModeChanged(securityMode,
            securityMode != SecurityMode_None && (newView->NeedsInput(&res), res));
    return NOERROR;
}

AutoPtr<IKeyguardSecurityViewFlipper> CKeyguardSecurityContainer::GetFlipper()
{
    AutoPtr<IKeyguardSecurityViewFlipper> flipper;
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        flipper = IKeyguardSecurityViewFlipper::Probe(child);
        if (flipper != NULL) {
            return flipper;
        }
    }
    return NULL;
}

ECode CKeyguardSecurityContainer::ShowBouncer(
    /* [in] */ Int32 duration)
{
    AutoPtr<IKeyguardSecurityViewFlipper> flipper = GetFlipper();
    if (flipper != NULL) {
        IKeyguardSecurityView::Probe(flipper)->ShowBouncer(duration);
    }
    return NOERROR;
}

ECode CKeyguardSecurityContainer::HideBouncer(
    /* [in] */ Int32 duration)
{
    AutoPtr<IKeyguardSecurityViewFlipper> flipper = GetFlipper();
    if (flipper != NULL) {
        IKeyguardSecurityView::Probe(flipper)->HideBouncer(duration);
    }
    return NOERROR;
}

Int32 CKeyguardSecurityContainer::GetSecurityViewIdForMode(
    /* [in] */ SecurityMode securityMode)
{
    switch (securityMode) {
        case SecurityMode_Pattern: return R::id::keyguard_pattern_view;
        case SecurityMode_PIN: return R::id::keyguard_pin_view;
        case SecurityMode_Password: return R::id::keyguard_password_view;
        case SecurityMode_Biometric: return R::id::keyguard_face_unlock_view;
        case SecurityMode_Account: return R::id::keyguard_account_view;
        case SecurityMode_SimPin: return R::id::keyguard_sim_pin_view;
        case SecurityMode_SimPuk: return R::id::keyguard_sim_puk_view;
    }
    return 0;
}

Int32 CKeyguardSecurityContainer::GetLayoutIdFor(
    /* [in] */ SecurityMode securityMode)
{
    switch (securityMode) {
        case SecurityMode_Pattern: return R::layout::keyguard_pattern_view;
        case SecurityMode_PIN: return R::layout::keyguard_pin_view;
        case SecurityMode_Password: return R::layout::keyguard_password_view;
        case SecurityMode_Biometric: return R::layout::keyguard_face_unlock_view;
        case SecurityMode_Account: return R::layout::keyguard_account_view;
        case SecurityMode_SimPin: return R::layout::keyguard_sim_pin_view;
        case SecurityMode_SimPuk: return R::layout::keyguard_sim_puk_view;
        default:
            return 0;
    }
}

ECode CKeyguardSecurityContainer::GetSecurityMode(
    /* [out] */ SecurityMode* mode)
{
    return mSecurityModel->GetSecurityMode(mode);
}

ECode CKeyguardSecurityContainer::VerifyUnlock()
{
    mIsVerifyUnlockOnly = TRUE;
    SecurityMode mode;
    GetSecurityMode(&mode);
    return ShowSecurityScreen(mode);
}

ECode CKeyguardSecurityContainer::GetCurrentSecuritySelection(
    /* [out] */ SecurityMode* mode)
{
    VALIDATE_NOT_NULL(mode)
    *mode = mCurrentSecuritySelection;
    return NOERROR;
}

ECode CKeyguardSecurityContainer::Dismiss(
    /* [in] */ Boolean authenticated)
{
    return mCallback->Dismiss(authenticated);
}

ECode CKeyguardSecurityContainer::NeedsInput(
    /* [out] */ Boolean* result)
{
    return IKeyguardSecurityView::Probe(mSecurityViewFlipper)->NeedsInput(result);
}

ECode CKeyguardSecurityContainer::SetKeyguardCallback(
    /* [in] */ IKeyguardSecurityCallback* callback)
{
    return IKeyguardSecurityView::Probe(mSecurityViewFlipper)->SetKeyguardCallback(callback);
}

ECode CKeyguardSecurityContainer::Reset()
{
    return IKeyguardSecurityView::Probe(mSecurityViewFlipper)->Reset();
}

ECode CKeyguardSecurityContainer::GetCallback(
    /* [in] */ IKeyguardSecurityCallback** callback)
{
    return IKeyguardSecurityView::Probe(mSecurityViewFlipper)->GetCallback(callback);
}

ECode CKeyguardSecurityContainer::ShowUsabilityHint()
{
    return IKeyguardSecurityView::Probe(mSecurityViewFlipper)->ShowUsabilityHint();
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
