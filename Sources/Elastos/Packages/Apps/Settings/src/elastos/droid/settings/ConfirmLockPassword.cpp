
#include "Elastos.Droid.InputMethodService.h"
#include "elastos/droid/settings/ConfirmLockPassword.h"
#include "elastos/droid/settings/CSettingsActivity.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Internal::Widget::CPasswordEntryKeyboardHelper;
using Elastos::Droid::InputMethodService::IKeyboardView;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Text::EIID_ITextWatcher;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::EIID_IOnEditorActionListener;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  ConfirmLockPassword::ConfirmLockPasswordFragment::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL_3(ConfirmLockPassword::ConfirmLockPasswordFragment::InnerListener, Object, IViewOnClickListener, IOnEditorActionListener, ITextWatcher)

ConfirmLockPassword::ConfirmLockPasswordFragment::InnerListener::InnerListener(
    /* [in] */ ConfirmLockPasswordFragment* host)
    : mHost(host)
{}

ECode ConfirmLockPassword::ConfirmLockPasswordFragment::InnerListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

ECode ConfirmLockPassword::ConfirmLockPasswordFragment::InnerListener::OnEditorAction(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 actionId,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    return mHost->OnEditorAction(v, actionId, event, result);
}

ECode ConfirmLockPassword::ConfirmLockPasswordFragment::InnerListener::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return mHost->BeforeTextChanged(s, start, count, after);
}

ECode ConfirmLockPassword::ConfirmLockPasswordFragment::InnerListener::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    return mHost->OnTextChanged(s, start, before, count);
}

ECode ConfirmLockPassword::ConfirmLockPasswordFragment::InnerListener::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    return mHost->AfterTextChanged(s);
}

//===============================================================================
//                  ConfirmLockPassword::ConfirmLockPasswordFragment::MyRunnable
//===============================================================================

ConfirmLockPassword::ConfirmLockPasswordFragment::MyRunnable::MyRunnable(
    /* [in] */ ConfirmLockPasswordFragment* host)
    : mHost(host)
{}

ECode ConfirmLockPassword::ConfirmLockPasswordFragment::MyRunnable::Run()
{
    mHost->mHeaderText->SetText(mHost->GetDefaultHeader());
    return NOERROR;
}

//===============================================================================
//                  ConfirmLockPassword::ConfirmLockPasswordFragment::HandleAttemptLockoutCountDownTimer
//===============================================================================

ConfirmLockPassword::ConfirmLockPasswordFragment::HandleAttemptLockoutCountDownTimer::HandleAttemptLockoutCountDownTimer(
    /* [in] */ ConfirmLockPasswordFragment* host)
    : mHost(host)
{}

ECode ConfirmLockPassword::ConfirmLockPasswordFragment::HandleAttemptLockoutCountDownTimer::constructor(
    /* [in] */ Int64 millisInFuture,
    /* [in] */ Int64 countDownInterval)
{
    return CountDownTimer::constructor(millisInFuture, countDownInterval);
}

ECode ConfirmLockPassword::ConfirmLockPasswordFragment::HandleAttemptLockoutCountDownTimer::OnTick(
    /* [in] */ Int64 millisUntilFinished)
{
    const Int32 secondsCountdown = (Int32) (millisUntilFinished / 1000);

    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, CoreUtils::Convert(secondsCountdown));
    String str;
    mHost->GetString(R::string::lockpattern_too_many_failed_confirmation_attempts_footer, args, &str);

    mHost->mHeaderText->SetText(CoreUtils::Convert(str));
    return NOERROR;
}

ECode ConfirmLockPassword::ConfirmLockPasswordFragment::HandleAttemptLockoutCountDownTimer::OnFinish()
{
    IView::Probe(mHost->mPasswordEntry)->SetEnabled(TRUE);
    mHost->mHeaderText->SetText(mHost->GetDefaultHeader());
    mHost->mNumWrongConfirmAttempts = 0;
    return NOERROR;
}

//===============================================================================
//                  ConfirmLockPassword::ConfirmLockPasswordFragment
//===============================================================================

const String ConfirmLockPassword::ConfirmLockPasswordFragment::KEY_NUM_WRONG_CONFIRM_ATTEMPTS("confirm_lock_password_fragment.key_num_wrong_confirm_attempts");
const Int64 ConfirmLockPassword::ConfirmLockPasswordFragment::ERROR_MESSAGE_TIMEOUT = 3000;

ConfirmLockPassword::ConfirmLockPasswordFragment::ConfirmLockPasswordFragment()
    : mNumWrongConfirmAttempts(0)
    , mIsAlpha(FALSE)
{
}

ECode ConfirmLockPassword::ConfirmLockPasswordFragment::constructor()
{
    CHandler::New((IHandler**)&mHandler);
    mResetErrorRunnable = new MyRunnable(this);
    return Fragment::constructor();
}

ECode ConfirmLockPassword::ConfirmLockPasswordFragment::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Fragment::OnCreate(savedInstanceState);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    CLockPatternUtils::New(IContext::Probe(activity), (ILockPatternUtils**)&mLockPatternUtils);
    if (savedInstanceState != NULL) {
        savedInstanceState->GetInt32(
                KEY_NUM_WRONG_CONFIRM_ATTEMPTS, 0, &mNumWrongConfirmAttempts);
    }
    return NOERROR;
}

ECode ConfirmLockPassword::ConfirmLockPasswordFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 storedQuality;
    mLockPatternUtils->GetKeyguardStoredPasswordQuality(&storedQuality);
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::confirm_lock_password, NULL, (IView**)&view);
    // Disable IME on our window since we provide our own keyboard

    AutoPtr<InnerListener> listener = new InnerListener(this);
    AutoPtr<IView> tmp;
    view->FindViewById(R::id::cancel_button, (IView**)&tmp);
    tmp->SetOnClickListener(listener);
    AutoPtr<IView> continueButtonTmp;
    view->FindViewById(R::id::next_button, (IView**)&continueButtonTmp);
    mContinueButton = IButton::Probe(continueButtonTmp);
    continueButtonTmp->SetOnClickListener(listener);
    continueButtonTmp->SetEnabled(FALSE); // disable until the user enters at least one Char32

    tmp = NULL;
    view->FindViewById(R::id::password_entry, (IView**)&tmp);
    mPasswordEntry = ITextView::Probe(tmp);
    mPasswordEntry->SetOnEditorActionListener(listener);
    mPasswordEntry->AddTextChangedListener(listener);

    tmp = NULL;
    view->FindViewById(R::id::keyboard, (IView**)&tmp);
    mKeyboardView = IPasswordEntryKeyboardView::Probe(tmp);
    tmp = NULL;
    view->FindViewById(R::id::headerText, (IView**)&tmp);
    mHeaderText = ITextView::Probe(tmp);
    mIsAlpha = IDevicePolicyManager::PASSWORD_QUALITY_ALPHABETIC == storedQuality
            || IDevicePolicyManager::PASSWORD_QUALITY_ALPHANUMERIC == storedQuality
            || IDevicePolicyManager::PASSWORD_QUALITY_COMPLEX == storedQuality;

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);
    if (intent != NULL) {
        AutoPtr<ICharSequence> headerMessage;
        intent->GetCharSequenceExtra(HEADER_TEXT, (ICharSequence**)&headerMessage);
        if (TextUtils::IsEmpty(headerMessage)) {
            String str;
            GetString(GetDefaultHeader(), &str);
            headerMessage = CoreUtils::Convert(str);
        }
        mHeaderText->SetText(headerMessage);
    }

    CPasswordEntryKeyboardHelper::New(IContext::Probe(activity),
            IKeyboardView::Probe(mKeyboardView), IView::Probe(mPasswordEntry),
            (IPasswordEntryKeyboardHelper**)&mKeyboardHelper);
    mKeyboardHelper->SetKeyboardMode(mIsAlpha ?
            IPasswordEntryKeyboardHelper::KEYBOARD_MODE_ALPHA
            : IPasswordEntryKeyboardHelper::KEYBOARD_MODE_NUMERIC);
    Boolean res;
    IView::Probe(mKeyboardView)->RequestFocus(&res);

    Int32 currentType;
    mPasswordEntry->GetInputType(&currentType);
    mPasswordEntry->SetInputType(mIsAlpha ? currentType
            : (IInputType::TYPE_CLASS_NUMBER | IInputType::TYPE_NUMBER_VARIATION_PASSWORD));

    if (ISettingsActivity::Probe(activity) != NULL) {
        CSettingsActivity* sa = (CSettingsActivity*) ISettingsActivity::Probe(activity);
        Int32 id = GetDefaultHeader();
        AutoPtr<ICharSequence> title;
        GetText(id, (ICharSequence**)&title);
        sa->SetTitle(title);
    }

    *result = view;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

Int32 ConfirmLockPassword::ConfirmLockPasswordFragment::GetDefaultHeader()
{
    return mIsAlpha ? R::string::lockpassword_confirm_your_password_header
            : R::string::lockpassword_confirm_your_pin_header;
}

ECode ConfirmLockPassword::ConfirmLockPasswordFragment::OnPause()
{
    Fragment::OnPause();
    Boolean res;
    IView::Probe(mKeyboardView)->RequestFocus(&res);
    if (mCountdownTimer != NULL) {
        mCountdownTimer->Cancel();
        mCountdownTimer = NULL;
    }
    return NOERROR;
}

ECode ConfirmLockPassword::ConfirmLockPasswordFragment::OnResume()
{
    // TODO Auto-generated method stub
    Fragment::OnResume();
    Boolean res;
    IView::Probe(mKeyboardView)->RequestFocus(&res);
    Int64 deadline;
    mLockPatternUtils->GetLockoutAttemptDeadline(&deadline);
    if (deadline != 0) {
        HandleAttemptLockout(deadline);
    }
    return NOERROR;
}

ECode ConfirmLockPassword::ConfirmLockPasswordFragment::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    Fragment::OnSaveInstanceState(outState);
    outState->PutInt32(KEY_NUM_WRONG_CONFIRM_ATTEMPTS, mNumWrongConfirmAttempts);
    return NOERROR;
}

void ConfirmLockPassword::ConfirmLockPasswordFragment::HandleNext()
{
    AutoPtr<ICharSequence> cs;
    mPasswordEntry->GetText((ICharSequence**)&cs);
    String pin;
    cs->ToString(&pin);
    Boolean res;
    if (mLockPatternUtils->CheckPassword(pin, &res), res) {

        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        if (IConfirmLockPasswordInternalActivity::Probe(activity) != NULL) {
            intent->PutExtra(ChooseLockSettingsHelper::EXTRA_KEY_TYPE,
                            mIsAlpha ? IStorageManager::CRYPT_TYPE_PASSWORD
                                     : IStorageManager::CRYPT_TYPE_PIN);
            intent->PutExtra(ChooseLockSettingsHelper::EXTRA_KEY_PASSWORD, pin);
        }

        activity->SetResult(RESULT_OK, intent);
        activity->Finish();
    }
    else {
        if (++mNumWrongConfirmAttempts >= ILockPatternUtils::FAILED_ATTEMPTS_BEFORE_TIMEOUT) {
            Int64 deadline;
            mLockPatternUtils->SetLockoutAttemptDeadline(&deadline);
            HandleAttemptLockout(deadline);
        }
        else {
            ShowError(R::string::lockpattern_need_to_unlock_wrong);
        }
    }
}

void ConfirmLockPassword::ConfirmLockPasswordFragment::HandleAttemptLockout(
    /* [in] */ Int64 elapsedRealtimeDeadline)
{
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    ShowError(R::string::lockpattern_too_many_failed_confirmation_attempts_header, 0);
    IView::Probe(mPasswordEntry)->SetEnabled(FALSE);
    mCountdownTimer = new HandleAttemptLockoutCountDownTimer(this);
    mCountdownTimer->constructor(elapsedRealtimeDeadline - elapsedRealtime,
            ILockPatternUtils::FAILED_ATTEMPT_COUNTDOWN_INTERVAL_MS);
    mCountdownTimer->Start();
}

ECode ConfirmLockPassword::ConfirmLockPasswordFragment::OnClick(
    /* [in] */ IView* v)
{
    Int32 id;
    v->GetId(&id);
    switch (id) {
        case R::id::next_button:
            HandleNext();
            break;

        case R::id::cancel_button: {
            AutoPtr<IActivity> activity;
            GetActivity((IActivity**)&activity);
            activity->SetResult(RESULT_CANCELED);
            activity->Finish();
            break;
        }
    }
    return NOERROR;
}

void ConfirmLockPassword::ConfirmLockPasswordFragment::ShowError(
    /* [in] */ Int32 msg)
{
    ShowError(msg, ERROR_MESSAGE_TIMEOUT);
}

void ConfirmLockPassword::ConfirmLockPasswordFragment::ShowError(
    /* [in] */ Int32 msg,
    /* [in] */ Int64 timeout)
{
    mHeaderText->SetText(msg);
    AutoPtr<ICharSequence> text;
    mHeaderText->GetText((ICharSequence**)&text);
    IView::Probe(mHeaderText)->AnnounceForAccessibility(text);
    mPasswordEntry->SetText(NULL);
    mHandler->RemoveCallbacks(mResetErrorRunnable);
    if (timeout != 0) {
        Boolean res;
        mHandler->PostDelayed(mResetErrorRunnable, timeout, &res);
    }
}

ECode ConfirmLockPassword::ConfirmLockPasswordFragment::OnEditorAction(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 actionId,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    // Check if this was the result of hitting the enter or "done" key
    if (actionId == IEditorInfo::IME_NULL
            || actionId == IEditorInfo::IME_ACTION_DONE
            || actionId == IEditorInfo::IME_ACTION_NEXT) {
        HandleNext();
        *result = TRUE;
        return NOERROR;
    }
    return NOERROR;
}

ECode ConfirmLockPassword::ConfirmLockPasswordFragment::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return NOERROR;
}

ECode ConfirmLockPassword::ConfirmLockPasswordFragment::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    return NOERROR;
}

ECode ConfirmLockPassword::ConfirmLockPasswordFragment::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    AutoPtr<ICharSequence> cs;
    mPasswordEntry->GetText((ICharSequence**)&cs);
    Int32 length;
    cs->GetLength(&length);
    IView::Probe(mContinueButton)->SetEnabled(length > 0);
    return NOERROR;
}

//===============================================================================
//                  ConfirmLockPassword
//===============================================================================

const String ConfirmLockPassword::PACKAGE("com.android.settings");
const String ConfirmLockPassword::HEADER_TEXT = PACKAGE + ".ConfirmLockPattern.header";

ConfirmLockPassword::ConfirmLockPassword()
{}

ConfirmLockPassword::~ConfirmLockPassword()
{}

ECode ConfirmLockPassword::constructor()
{
    return SettingsActivity::constructor();
}

ECode ConfirmLockPassword::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    // Disable IME on our window since we provide our own keyboard
    //GetWindow()->SetFlags(WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM,
            //WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM);
    SettingsActivity::OnCreate(savedInstanceState);
    AutoPtr<ICharSequence> msg;
    GetText(R::string::lockpassword_confirm_your_password_header, (ICharSequence**)&msg);
    SetTitle(msg);
    return NOERROR;
}

ECode ConfirmLockPassword::GetIntent(
    /* [out] */ IIntent** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IIntent> intentOne;
    SettingsActivity::GetIntent((IIntent**)&intentOne);
    AutoPtr<IIntent> modIntent;
    CIntent::New(intentOne, (IIntent**)&modIntent);
    modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, String("Elastos.Droid.Settings.CConfirmLockPasswordFragment"));
    *result = modIntent;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

Boolean ConfirmLockPassword::IsValidFragment(
    /* [in] */ const String& fragmentName)
{
    if (String("Elastos.Droid.Settings.CConfirmLockPasswordFragment").Equals(fragmentName)) return TRUE;
    return FALSE;
}

//===============================================================================
//                  ConfirmLockPasswordInternalActivity
//===============================================================================

CAR_INTERFACE_IMPL(ConfirmLockPasswordInternalActivity, ConfirmLockPassword, IConfirmLockPasswordInternalActivity)

ConfirmLockPasswordInternalActivity::ConfirmLockPasswordInternalActivity()
{}

ConfirmLockPasswordInternalActivity::~ConfirmLockPasswordInternalActivity()
{}

ECode ConfirmLockPasswordInternalActivity::constructor()
{
    return ConfirmLockPassword::constructor();
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos