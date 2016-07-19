
#include "elastos/droid/systemui/keyguard/CKeyguardSecurityContainer.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

static const String TAG("KeyguardSecurityView");

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
    return mHost->mSecurityCallback->Dismiss(authenticated);
}

ECode CKeyguardSecurityContainer::MyKeyguardSecurityCallback::IsVerifyUnlockOnly(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mIsVerifyUnlockOnly;
    return NOERROR;
}

ECode CKeyguardSecurityContainer::MyKeyguardSecurityCallback::ReportUnlockAttempt(
    /* [in] */ Boolean success)
{
    AutoPtr<KeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mHost->mContext);
    if (success) {
        monitor->ClearFailedUnlockAttempts();
        mHost->mLockPatternUtils->ReportSuccessfulPasswordAttempt();
    }
    else {
        if (mHost->mCurrentSecuritySelection == KeyguardSecurityModel::Biometric) {
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
    mCurrentSecuritySelection = KeyguardSecurityModel::Invalid;
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
    FrameLayout::constructor(context, attrs, defStyle);
    mSecurityModel = new KeyguardSecurityModel(context);
    CLockPatternUtils::New(context, (ILockPatternUtils**)&mLockPatternUtils);
    mUpdateMonitor = KeyguardUpdateMonitor::GetInstance(mContext);
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

ECode CKeyguardSecurityContainer::OnResume(
    /* [in] */ Int32 reason)
{
    if (mCurrentSecuritySelection != KeyguardSecurityModel::None) {
        AutoPtr<IKeyguardSecurityView> view;
        GetSecurityView(mCurrentSecuritySelection, (IKeyguardSecurityView**)&view);
        view->OnResume(reason);
    }
    return NOERROR;
}

ECode CKeyguardSecurityContainer::OnPause()
{
    if (mCurrentSecuritySelection != KeyguardSecurityModel::None) {
        AutoPtr<IKeyguardSecurityView> view;
        GetSecurityView(mCurrentSecuritySelection, (IKeyguardSecurityView**)&view);
        view->OnPause();
    }
    return NOERROR;
}

ECode CKeyguardSecurityContainer::StartAppearAnimation()
{
    if (mCurrentSecuritySelection != KeyguardSecurityModel::None) {
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
    if (mCurrentSecuritySelection != KeyguardSecurityModel::None) {
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
    mSecurityViewFlipper->GetChildCount(&children);
    for (Int32 i = 0; i < children; i++) {
        AutoPtr<IView> view;
        mSecurityViewFlipper->GetChildAt(i, (IView**)&view);
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

AutoPtr<IKeyguardSecurityView> CKeyguardSecurityContainer::GetSecurityView(
    /* [in] */ KeyguardSecurityModel::SecurityMode securityMode)
{
    Int32 securityViewIdForMode = GetSecurityViewIdForMode(securityMode);
    AutoPtr<IKeyguardSecurityView> view;
    Int32 children;
    mSecurityViewFlipper->GetChildCount(&children);
    for (Int32 child = 0; child < children; child++) {
        AutoPtr<IView> _view;
        IViewGroup::Probe(mSecurityViewFlipper)->GetChildAt(child, (IView**)&_view);
        Int32 id;
        _view->GetId(&id);
        if (id == securityViewIdForMode) {
            view = IKeyguardSecurityView::Probe(_view);
            break;
        }
    }
    Int32 layoutId = GetLayoutIdFor(securityMode);
    if (view == NULL && layoutId != 0) {
        AutoPtr<ILayoutInflater> inflater = LayoutInflater::From(mContext);
        if (DEBUG) Logger::V(TAG, "inflating id = %d", layoutId);
        AutoPtr<IView> v;
        inflater->Inflate(layoutId, mSecurityViewFlipper, FALSE, (IView**)&v);
        mSecurityViewFlipper->AddView(v);
        UpdateSecurityView(v, mIsBouncing);
        view = IKeyguardSecurityView::Probe(v);
    }

    return view;
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
    return mSecurityViewFlipper->SetLockPatternUtils(mLockPatternUtils);
}

ECode CKeyguardSecurityContainer::SetLockPatternUtils(
    /* [in] */ ILockPatternUtils* utils)
{
    mLockPatternUtils = utils;
    mSecurityModel->SetLockPatternUtils(utils);
    return mSecurityViewFlipper->SetLockPatternUtils(mLockPatternUtils);
}

void CKeyguardSecurityContainer::ShowDialog(
    /* [in] */ const String& title,
    /* [in] */ const String& message)
{
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilde::New((IAlertDialogBuilder**)&builder);
    builder->SetTitle(title);
    builder->SetMessage(message);
    builder->SetNeutralButton(R::string::ok, NULL);
    AutoPtr<IAlertDialog> dialog;
    builder->Create((IAlertDialog**)&dialog);

    if (IActivity::Probe(mContext) == NULL) {
        AutoPtr<IWindow> window;
        dialog->GetWindow((IWindow**)&window);
        window->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);
    }
    dialog->Show();
}

void CKeyguardSecurityContainer::ShowTimeoutDialog()
{
    Int32 timeoutInSeconds = (Int32) ILockPatternUtils::FAILED_ATTEMPT_TIMEOUT_MS / 1000;
    Int32 messageId = 0;

    Int32 mode;
    mSecurityModel->GetSecurityMode(&mode);
    switch (mode) {
        case Pattern:
            messageId = R::string::kg_too_many_failed_pattern_attempts_dialog_message;
            break;
        case PIN:
            messageId = R::string::kg_too_many_failed_pin_attempts_dialog_message;
            break;
        case Password:
            messageId = R::string::kg_too_many_failed_password_attempts_dialog_message;
            break;
        // These don't have timeout dialogs.
        case Account:
        case Biometric:
        case Invalid:
        case None:
        case SimPin:
        case SimPuk:
            break;
    }

    if (messageId != 0) {
        Int32 times;
        KeyguardUpdateMonitor::GetInstance(mContext)->GetFailedUnlockAttempts(&times);
        String message;
        mContext->GetString(messageId, times, timeoutInSeconds, &message);
        ShowDialog(NULL, message);
    }
}

void CKeyguardSecurityContainer::ShowAlmostAtWipeDialog(
    /* [in] */ Int32 attempts,
    /* [in] */ Int32 remaining)
{
    String message;
    mContext->GetString(R::string::kg_failed_attempts_almost_at_wipe,
            attempts, remaining);
    ShowDialog(NULL, message);
}

void CKeyguardSecurityContainer::ShowWipeDialog(
    /* [in] */ Int32 attempts)
{
    String message;
    mContext->GetString(R::string::kg_failed_attempts_now_wiping, attempts, &message);
    ShowDialog(NULL, message);
}

void CKeyguardSecurityContainer::ShowAlmostAtAccountLoginDialog()
{
    Int32 timeoutInSeconds = (Int32) ILockPatternUtils::FAILED_ATTEMPT_TIMEOUT_MS / 1000;
    Int32 count = ILockPatternUtils::FAILED_ATTEMPTS_BEFORE_RESET
            - ILockPatternUtils::FAILED_ATTEMPTS_BEFORE_TIMEOUT;
    String message;
    mContext->GetString(R::string::kg_failed_attempts_almost_at_login,
            count, ILockPatternUtils::FAILED_ATTEMPTS_BEFORE_TIMEOUT, timeoutInSeconds, &message);
    ShowDialog(NULL, message);
}

void CKeyguardSecurityContainer::ReportFailedUnlockAttempt()
{
    AutoPtr<KeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);

    Int32 tmp;
    monitor->GetFailedUnlockAttempts(&tmp);
    Int32 failedAttempts; = tmp + 1; // +1 for this time

    if (DEBUG) Logger::D(TAG, "reportFailedPatternAttempt: # %d", failedAttempts);

    KeyguardSecurityModel::SecurityMode mode;
    mSecurityModel->GetSecurityMode(&mode);
    Boolean usingPattern = mode == KeyguardSecurityModel::Pattern;

    AutoPtr<IDevicePolicyManager> manager;
    mLockPatternUtils->GetDevicePolicyManager((IDevicePolicyManager**)&manager);
    Int32 user;
    mLockPatternUtils->GetCurrentUser(&user);
    Int32 failedAttemptsBeforeWipe;
    manager->GetMaximumFailedPasswordsForWipe(NULL, user, &failedAttemptsBeforeWipe);

    Int32 failedAttemptWarning = ILockPatternUtils::FAILED_ATTEMPTS_BEFORE_RESET
            - ILockPatternUtils::FAILED_ATTEMPTS_BEFORE_TIMEOUT;

    Int32 remainingBeforeWipe = failedAttemptsBeforeWipe > 0 ?
            (failedAttemptsBeforeWipe - failedAttempts)
            : Integer.MAX_VALUE; // because DPM returns 0 if no restriction

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
                ShowSecurityScreen(KeyguardSecurityModel::Account);
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
    KeyguardSecurityModel::SecurityMode securityMode;
    mSecurityModel->GetSecurityMode(&securityMode);
    if (DEBUG) Logger::V(TAG, "showPrimarySecurityScreen(turningOff=%d)", turningOff);
    Boolean res;
    if (!turningOff &&
            (KeyguardUpdateMonitor::GetInstance(mContext)->IsAlternateUnlockEnabled(&res), res)) {
        // If we're not turning off, then allow biometric alternate.
        // We'll reload it when the device comes back on.
        mSecurityModel->GetAlternateFor(securityMode, &securityMode);
    }
    return ShowSecurityScreen(securityMode);
}

void CKeyguardSecurityContainer::ShowBackupSecurityScreen()
{
    if (DEBUG) Logger::D(TAG, "showBackupSecurity()");
    KeyguardSecurityModel::SecurityMode backup;
    mSecurityModel->GetBackupSecurityMode(mCurrentSecuritySelection, &backup);
    ShowSecurityScreen(backup);
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
    else if (KeyguardSecurityModel::None == mCurrentSecuritySelection) {
        KeyguardSecurityModel::SecurityMode securityMode;
        mSecurityModel->GetSecurityMode(&securityMode);
        // Allow an alternate, such as biometric unlock
        mSecurityModel->GetAlternateFor(securityMode, &securityMode);
        if (KeyguardSecurityModel::None == securityMode) {
            finish = TRUE; // no security required
        }
        else {
            ShowSecurityScreen(securityMode); // switch to the alternate security view
        }
    }
    else if (authenticated) {
        switch (mCurrentSecuritySelection) {
            case Pattern:
            case Password:
            case PIN:
            case Account:
            case Biometric:
                finish = TRUE;
                break;

            case SimPin:
            case SimPuk:
                // Shortcut for SIM PIN/PUK to go to directly to user's security screen or home
                KeyguardSecurityModel::SecurityMode securityMode = mSecurityModel.getSecurityMode();
                if (securityMode != KeyguardSecurityModel::None) {
                    showSecurityScreen(securityMode);
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

void CKeyguardSecurityContainer::ShowSecurityScreen(
    /* [in] */ KeyguardSecurityModel::SecurityMode securityMode)
{
    if (DEBUG) Logger::D(TAG, "showSecurityScreen(%d)", securityMode);

    if (securityMode == mCurrentSecuritySelection) return;

    AutoPtr<IKeyguardSecurityView> oldView;
    GetSecurityView(mCurrentSecuritySelection, (IKeyguardSecurityView**)&oldView);
    AutoPtr<IKeyguardSecurityView> newView;
    GetSecurityView(securityMode, (IKeyguardSecurityView**)&newView);

    // Emulate Activity life cycle
    if (oldView != NULL) {
        oldView->OnPause();
        oldView->SetKeyguardCallback(mNullCallback); // ignore requests from old view
    }
    if (securityMode != KeyguardSecurityModel::None) {
        newView->OnResume(IKeyguardSecurityView::VIEW_REVEALED);
        newView->SetKeyguardCallback(mCallback);
    }

    // Find and show this child.
    Int32 childCount;
    mSecurityViewFlipper->GetChildCount(&childCount);

    Int32 securityViewIdForMode = GetSecurityViewIdForMode(securityMode);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> _view;
        IViewGroup::Probe(mSecurityViewFlipper)->GetChildAt(i, (IView**)&_view);
        Int32 id;
        _view->GetId(&id);
        if (id == securityViewIdForMode) {
            mSecurityViewFlipper->SetDisplayedChild(i);
            break;
        }
    }

    mCurrentSecuritySelection = securityMode;
    Boolean res;
    mSecurityCallback->OnSecurityModeChanged(securityMode,
            securityMode != KeyguardSecurityModel::None && (newView->NeedsInput(&res), res));
}

AutoPtr<IKeyguardSecurityViewFlipper> CKeyguardSecurityContainer::GetFlipper()
{
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        if (IKeyguardSecurityViewFlipper::Probe(child) != NULL) {
            return IKeyguardSecurityViewFlipper::Probe(child);
        }
    }
    return NULL;
}

ECode CKeyguardSecurityContainer::ShowBouncer(
    /* [in] */ Int32 duration)
{
    AutoPtr<IKeyguardSecurityViewFlipper> flipper = GetFlipper();
    if (flipper != NULL) {
        flipper->ShowBouncer(duration);
    }
    return NOERROR;
}

ECode CKeyguardSecurityContainer::HideBouncer(
    /* [in] */ Int32 duration)
{
    AutoPtr<IKeyguardSecurityViewFlipper> flipper = GetFlipper();
    if (flipper != NULL) {
        flipper->HideBouncer(duration);
    }
    return NOERROR;
}

Int32 CKeyguardSecurityContainer::GetSecurityViewIdForMode(
    /* [in] */ KeyguardSecurityModel::SecurityMode securityMode)
{
    switch (securityMode) {
        case Pattern: return R::id::keyguard_pattern_view;
        case PIN: return R::id::keyguard_pin_view;
        case Password: return R::id::keyguard_password_view;
        case Biometric: return R::id::keyguard_face_unlock_view;
        case Account: return R::id::keyguard_account_view;
        case SimPin: return R::id::keyguard_sim_pin_view;
        case SimPuk: return R::id::keyguard_sim_puk_view;
    }
    return 0;
}

Int32 CKeyguardSecurityContainer::GetLayoutIdFor(
    /* [in] */ KeyguardSecurityModel::SecurityMode securityMode)
{
    switch (securityMode) {
        case Pattern: return R::layout::keyguard_pattern_view;
        case PIN: return R::layout::keyguard_pin_view;
        case Password: return R::layout::keyguard_password_view;
        case Biometric: return R::layout::keyguard_face_unlock_view;
        case Account: return R::layout::keyguard_account_view;
        case SimPin: return R::layout::keyguard_sim_pin_view;
        case SimPuk: return R::layout::keyguard_sim_puk_view;
        default:
            return 0;
    }
}

ECode CKeyguardSecurityContainer::GetSecurityMode(
    /* [out] */ KeyguardSecurityModel::SecurityMode* mode)
{
    VALIDATE_NOT_NULL(mode)

    return mSecurityModel->GetSecurityMode(&mode);
}

ECode CKeyguardSecurityContainer::VerifyUnlock()
{
    mIsVerifyUnlockOnly = TRUE;
    KeyguardSecurityModel::SecurityMode mode;
    GetSecurityMode(&mode)
    return ShowSecurityScreen(mode);
}

ECode CKeyguardSecurityContainer::GetCurrentSecuritySelection(
    /* [out] */ KeyguardSecurityModel::SecurityMode* mode)
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
    VALIDATE_NOT_NULL(result)

    return mSecurityViewFlipper->NeedsInput(&result);
}

ECode CKeyguardSecurityContainer::SetKeyguardCallback(
    /* [in] */ IKeyguardSecurityCallback* callback)
{
    return mSecurityViewFlipper->SetKeyguardCallback(callback);
}

ECode CKeyguardSecurityContainer::Reset()
{
    return mSecurityViewFlipper->Reset();
}

ECode CKeyguardSecurityContainer::GetCallback(
    /* [in] */ IKeyguardSecurityCallback** callback)
{
    return mSecurityViewFlipper->GetCallback(callback);
}

ECode CKeyguardSecurityContainer::ShowUsabilityHint()
{
    return mSecurityViewFlipper->ShowUsabilityHint();
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
