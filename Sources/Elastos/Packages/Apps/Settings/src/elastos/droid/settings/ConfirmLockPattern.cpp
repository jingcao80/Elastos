
#include "elastos/droid/settings/ConfirmLockPattern.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Internal::Widget::EIID_IOnPatternListener;
using Elastos::Droid::Internal::Widget::ILinearLayoutWithDefaultTouchRecepient;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  ConfirmLockPattern::ConfirmLockPatternFragment::ClearPatternRunnable
//===============================================================================

ConfirmLockPattern::ConfirmLockPatternFragment::ClearPatternRunnable::ClearPatternRunnable(
    /* [in] */ ConfirmLockPatternFragment* host)
    : mHost(host)
{}

ECode ConfirmLockPattern::ConfirmLockPatternFragment::ClearPatternRunnable::Run()
{
    return mHost->mLockPatternView->ClearPattern();
}

//===============================================================================
//                  ConfirmLockPattern::ConfirmLockPatternFragment::LockPatternViewOnPatternListener
//===============================================================================

CAR_INTERFACE_IMPL(ConfirmLockPattern::ConfirmLockPatternFragment::LockPatternViewOnPatternListener, Object, IOnPatternListener)

ConfirmLockPattern::ConfirmLockPatternFragment::LockPatternViewOnPatternListener::LockPatternViewOnPatternListener(
    /* [in] */ ConfirmLockPatternFragment* host)
    : mHost(host)
{}

ECode ConfirmLockPattern::ConfirmLockPatternFragment::LockPatternViewOnPatternListener::OnPatternStart()
{
    Boolean res;
    return IView::Probe(mHost->mLockPatternView)->RemoveCallbacks(mHost->mClearPatternRunnable, &res);
}

ECode ConfirmLockPattern::ConfirmLockPatternFragment::LockPatternViewOnPatternListener::OnPatternCleared()
{
    Boolean res;
    return IView::Probe(mHost->mLockPatternView)->RemoveCallbacks(mHost->mClearPatternRunnable, &res);
}

ECode ConfirmLockPattern::ConfirmLockPatternFragment::LockPatternViewOnPatternListener::OnPatternCellAdded(
    /* [in] */ IList* pattern)//List<Cell>
{
    return NOERROR;
}

ECode ConfirmLockPattern::ConfirmLockPatternFragment::LockPatternViewOnPatternListener::OnPatternDetected(
    /* [in] */ IList* pattern) //List<LockPatternView.Cell>
{
    Boolean res;
    if (mHost->mLockPatternUtils->CheckPattern(pattern, &res), res) {

        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        AutoPtr<IActivity> activity;
        mHost->GetActivity((IActivity**)&activity);
        if (IConfirmLockPatternInternalActivity::Probe(activity) != NULL) {
            intent->PutExtra(ChooseLockSettingsHelper::EXTRA_KEY_TYPE,
                            IStorageManager::CRYPT_TYPE_PATTERN);
            String str;
            mHost->mLockPatternUtils->PatternToString(pattern, &str);
            intent->PutExtra(ChooseLockSettingsHelper::EXTRA_KEY_PASSWORD, str);
        }

        activity->SetResult(IActivity::RESULT_OK, intent);
        activity->Finish();
    }
    else {
        Int32 size;
        if ((pattern->GetSize(&size), size) >= ILockPatternUtils::MIN_PATTERN_REGISTER_FAIL &&
                ++(mHost->mNumWrongConfirmAttempts)
                >= ILockPatternUtils::FAILED_ATTEMPTS_BEFORE_TIMEOUT) {
            Int64 deadline;
            mHost->mLockPatternUtils->SetLockoutAttemptDeadline(&deadline);
            mHost->HandleAttemptLockout(deadline);
        }
        else {
            mHost->UpdateStage(Stage_NeedToUnlockWrong);
            mHost->PostClearPatternRunnable();
        }
    }
    return NOERROR;
}

//===============================================================================
//                  ConfirmLockPattern::ConfirmLockPatternFragment::HandleAttemptLockoutCountDownTimer
//===============================================================================

ConfirmLockPattern::ConfirmLockPatternFragment::HandleAttemptLockoutCountDownTimer::HandleAttemptLockoutCountDownTimer(
    /* [in] */ ConfirmLockPatternFragment* host)
    : mHost(host)
{}

ECode ConfirmLockPattern::ConfirmLockPatternFragment::HandleAttemptLockoutCountDownTimer::constructor(
    /* [in] */ Int64 millisInFuture,
    /* [in] */ Int64 countDownInterval)
{
    return CountDownTimer::constructor(millisInFuture, countDownInterval);
}

ECode ConfirmLockPattern::ConfirmLockPatternFragment::HandleAttemptLockoutCountDownTimer::OnTick(
    /* [in] */ Int64 millisUntilFinished)
{
    mHost->mHeaderTextView->SetText(R::string::lockpattern_too_many_failed_confirmation_attempts_header);
    Int32 secondsCountdown = (Int32) (millisUntilFinished / 1000);

    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, CoreUtils::Convert(secondsCountdown));
    String str;
    mHost->GetString(R::string::lockpattern_too_many_failed_confirmation_attempts_footer, args, &str);
    mHost->mFooterTextView->SetText(CoreUtils::Convert(str));
    return NOERROR;
}

ECode ConfirmLockPattern::ConfirmLockPatternFragment::HandleAttemptLockoutCountDownTimer::OnFinish()
{
    mHost->mNumWrongConfirmAttempts = 0;
    mHost->UpdateStage(Stage_NeedToUnlock);
    return NOERROR;
}

//===============================================================================
//                  ConfirmLockPattern::ConfirmLockPatternFragment
//===============================================================================

const Int32 ConfirmLockPattern::ConfirmLockPatternFragment::WRONG_PATTERN_CLEAR_TIMEOUT_MS = 2000;

const String ConfirmLockPattern::ConfirmLockPatternFragment::KEY_NUM_WRONG_ATTEMPTS("num_wrong_attempts");

ConfirmLockPattern::ConfirmLockPatternFragment::ConfirmLockPatternFragment()
    : mNumWrongConfirmAttempts(0)
{}

ConfirmLockPattern::ConfirmLockPatternFragment::~ConfirmLockPatternFragment()
{}

ECode ConfirmLockPattern::ConfirmLockPatternFragment::constructor()
{
    mClearPatternRunnable = new ClearPatternRunnable(this);

    mConfirmExistingLockPatternListener = new LockPatternViewOnPatternListener(this);

    return Fragment::constructor();
}

ECode ConfirmLockPattern::ConfirmLockPatternFragment::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Fragment::OnCreate(savedInstanceState);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    CLockPatternUtils::New(IContext::Probe(activity), (ILockPatternUtils**)&mLockPatternUtils);
    return NOERROR;
}

ECode ConfirmLockPattern::ConfirmLockPatternFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IView> view;
    inflater->Inflate(R::layout::confirm_lock_pattern, NULL, (IView**)&view);

    AutoPtr<IView> tmp;
    view->FindViewById(R::id::headerText, (IView**)&tmp);
    mHeaderTextView = ITextView::Probe(tmp);
    AutoPtr<IView> lockPatternViewTmp;
    view->FindViewById(R::id::lockPattern, (IView**)&lockPatternViewTmp);
    mLockPatternView = ILockPatternView::Probe(lockPatternViewTmp);
    tmp = NULL;
    view->FindViewById(R::id::footerText, (IView**)&tmp);
    mFooterTextView = ITextView::Probe(tmp);

    // make it so unhandled touch events within the unlock screen go to the
    // lock pattern view.
    tmp = NULL;
    view->FindViewById(R::id::topLayout, (IView**)&tmp);
    AutoPtr<ILinearLayoutWithDefaultTouchRecepient> topLayout = ILinearLayoutWithDefaultTouchRecepient::Probe(tmp);
    topLayout->SetDefaultTouchRecepient(lockPatternViewTmp);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);

    if (intent != NULL) {
        intent->GetCharSequenceExtra(HEADER_TEXT, (ICharSequence**)&mHeaderText);
        intent->GetCharSequenceExtra(FOOTER_TEXT, (ICharSequence**)&mFooterText);
        intent->GetCharSequenceExtra(HEADER_WRONG_TEXT, (ICharSequence**)&mHeaderWrongText);
        intent->GetCharSequenceExtra(FOOTER_WRONG_TEXT, (ICharSequence**)&mFooterWrongText);
    }

    Boolean res;
    mLockPatternUtils->IsTactileFeedbackEnabled(&res);
    mLockPatternView->SetTactileFeedbackEnabled(res);
    mLockPatternView->SetOnPatternListener(mConfirmExistingLockPatternListener);
    UpdateStage(Stage_NeedToUnlock);

    if (savedInstanceState != NULL) {
        savedInstanceState->GetInt32(KEY_NUM_WRONG_ATTEMPTS, &mNumWrongConfirmAttempts);
    }
    else {
        // on first launch, if no lock pattern is set, then finish with
        // success (don't want user to get stuck confirming something that
        // doesn't exist).
        if (mLockPatternUtils->SavedPatternExists(&res), !res) {
            activity->SetResult(IActivity::RESULT_OK);
            activity->Finish();
        }
    }
    *result = view;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ConfirmLockPattern::ConfirmLockPatternFragment::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    // deliberately not calling super since we are managing this in full
    outState->PutInt32(KEY_NUM_WRONG_ATTEMPTS, mNumWrongConfirmAttempts);
    return NOERROR;
}

ECode ConfirmLockPattern::ConfirmLockPatternFragment::OnPause()
{
    Fragment::OnPause();

    if (mCountdownTimer != NULL) {
        mCountdownTimer->Cancel();
    }
    return NOERROR;
}

ECode ConfirmLockPattern::ConfirmLockPatternFragment::OnResume()
{
    Fragment::OnResume();

    // if the user is currently locked out, enforce it.
    Int64 deadline;
    mLockPatternUtils->GetLockoutAttemptDeadline(&deadline);
    Boolean res;
    if (deadline != 0) {
        HandleAttemptLockout(deadline);
    }
    else if (IView::Probe(mLockPatternView)->IsEnabled(&res), !res) {
        // The deadline has passed, but the timer was cancelled...
        // Need to clean up.
        mNumWrongConfirmAttempts = 0;
        UpdateStage(Stage_NeedToUnlock);
    }
    return NOERROR;
}

void ConfirmLockPattern::ConfirmLockPatternFragment::UpdateStage(
    /* [in] */ Stage stage)
{
    switch (stage) {
        case Stage_NeedToUnlock:
            if (mHeaderText != NULL) {
                mHeaderTextView->SetText(mHeaderText);
            }
            else {
                mHeaderTextView->SetText(R::string::lockpattern_need_to_unlock);
            }
            if (mFooterText != NULL) {
                mFooterTextView->SetText(mFooterText);
            }
            else {
                mFooterTextView->SetText(R::string::lockpattern_need_to_unlock_footer);
            }

            IView::Probe(mLockPatternView)->SetEnabled(TRUE);
            mLockPatternView->EnableInput();
            break;
        case Stage_NeedToUnlockWrong:
            if (mHeaderWrongText != NULL) {
                mHeaderTextView->SetText(mHeaderWrongText);
            }
            else {
                mHeaderTextView->SetText(R::string::lockpattern_need_to_unlock_wrong);
            }
            if (mFooterWrongText != NULL) {
                mFooterTextView->SetText(mFooterWrongText);
            }
            else {
                mFooterTextView->SetText(R::string::lockpattern_need_to_unlock_wrong_footer);
            }

            mLockPatternView->SetDisplayMode(Elastos::Droid::Internal::Widget::DisplayMode_Wrong);
            IView::Probe(mLockPatternView)->SetEnabled(TRUE);
            mLockPatternView->EnableInput();
            break;
        case Stage_LockedOut:
            mLockPatternView->ClearPattern();
            // enabled = FALSE means: disable input, and have the
            // appearance of being disabled.
            IView::Probe(mLockPatternView)->SetEnabled(FALSE); // appearance of being disabled
            break;
    }

    // Always announce the header for accessibility. This is a no-op
    // when accessibility is disabled.
    AutoPtr<ICharSequence> cs;
    mHeaderTextView->GetText((ICharSequence**)&cs);
    IView::Probe(mHeaderTextView)->AnnounceForAccessibility(cs);
}

void ConfirmLockPattern::ConfirmLockPatternFragment::PostClearPatternRunnable()
{
    IView* lockPatternView = IView::Probe(mLockPatternView);
    Boolean res;
    lockPatternView->RemoveCallbacks(mClearPatternRunnable, &res);
    lockPatternView->PostDelayed(mClearPatternRunnable, WRONG_PATTERN_CLEAR_TIMEOUT_MS, &res);
}

void ConfirmLockPattern::ConfirmLockPatternFragment::HandleAttemptLockout(
    /* [in] */ Int64 elapsedRealtimeDeadline)
{
    UpdateStage(Stage_LockedOut);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    mCountdownTimer = new HandleAttemptLockoutCountDownTimer(this);
    mCountdownTimer->constructor(elapsedRealtimeDeadline - elapsedRealtime,
            ILockPatternUtils::FAILED_ATTEMPT_COUNTDOWN_INTERVAL_MS);
    mCountdownTimer->Start();
}

//===============================================================================
//                  ConfirmLockPattern
//===============================================================================

const String ConfirmLockPattern::PACKAGE("com.android.settings");
const String ConfirmLockPattern::HEADER_TEXT = PACKAGE + ".ConfirmLockPattern.header";
const String ConfirmLockPattern::FOOTER_TEXT = PACKAGE + ".ConfirmLockPattern.footer";
const String ConfirmLockPattern::HEADER_WRONG_TEXT = PACKAGE + ".ConfirmLockPattern.header_wrong";
const String ConfirmLockPattern::FOOTER_WRONG_TEXT = PACKAGE + ".ConfirmLockPattern.footer_wrong";

ConfirmLockPattern::ConfirmLockPattern()
{}

ConfirmLockPattern::~ConfirmLockPattern()
{}

ECode ConfirmLockPattern::constructor()
{
    return SettingsActivity::constructor();
}

ECode ConfirmLockPattern::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    SettingsActivity::OnCreate(savedInstanceState);
    AutoPtr<ICharSequence> msg;
    GetText(R::string::lockpassword_confirm_your_pattern_header, (ICharSequence**)&msg);
    SetTitle(msg);
    return NOERROR;
}

ECode ConfirmLockPattern::GetIntent(
    /* [out] */ IIntent** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IIntent> intentOne;
    SettingsActivity::GetIntent((IIntent**)&intentOne);
    AutoPtr<IIntent> modIntent;
    CIntent::New(intentOne, (IIntent**)&modIntent);
    modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, String("Elastos.Droid.Settings.CConfirmLockPatternFragment"));
    *result = modIntent;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

Boolean ConfirmLockPattern::IsValidFragment(
    /* [in] */ const String& fragmentName)
{
    if (String("Elastos.Droid.Settings.CConfirmLockPatternFragment").Equals(fragmentName)) return TRUE;
    return FALSE;
}

//===============================================================================
//                  ConfirmLockPatternInternalActivity
//===============================================================================

CAR_INTERFACE_IMPL(ConfirmLockPatternInternalActivity, ConfirmLockPattern, IConfirmLockPatternInternalActivity)

ConfirmLockPatternInternalActivity::ConfirmLockPatternInternalActivity()
{}

ConfirmLockPatternInternalActivity::~ConfirmLockPatternInternalActivity()
{}

ECode ConfirmLockPatternInternalActivity::constructor()
{
    return ConfirmLockPattern::constructor();
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos