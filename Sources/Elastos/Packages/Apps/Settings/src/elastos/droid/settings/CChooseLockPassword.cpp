
#include "elastos/droid/settings/CChooseLockPassword.h"
#include "elastos/droid/settings/CChooseLockGeneric.h"
#include "elastos/droid/settings/CSettingsActivity.h"
#include "elastos/droid/settings/CEncryptionInterstitial.h"
#include "elastos/droid/settings/notification/CRedactionInterstitial.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Settings::CEncryptionInterstitial;
using Elastos::Droid::Settings::Notification::CRedactionInterstitial;

using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Internal::Widget::IPasswordEntryKeyboardView;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Internal::Widget::ILockPatternUtilsHelper;
using Elastos::Droid::Internal::Widget::CLockPatternUtilsHelper;
using Elastos::Droid::Internal::Widget::CPasswordEntryKeyboardHelper;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::ISelection;
using Elastos::Droid::Text::CSelection;
using Elastos::Droid::Text::EIID_ITextWatcher;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::EIID_IOnEditorActionListener;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  CChooseLockPassword::ChooseLockPasswordFragment::Stage
//===============================================================================

AutoPtr<CChooseLockPassword::ChooseLockPasswordFragment::Stage>
CChooseLockPassword::ChooseLockPasswordFragment::Stage::INTRODUCTION = new Stage(
        String("INTRODUCTION"), 0,
        R::string::lockpassword_choose_your_password_header,
        R::string::lockpassword_choose_your_pin_header,
        R::string::lockpassword_continue_label);

AutoPtr<CChooseLockPassword::ChooseLockPasswordFragment::Stage>
CChooseLockPassword::ChooseLockPasswordFragment::Stage::NEED_TO_CONFIRM = new Stage(
        String("NEED_TO_CONFIRM"), 1,
        R::string::lockpassword_confirm_your_password_header,
        R::string::lockpassword_confirm_your_pin_header,
        R::string::lockpassword_ok_label);

AutoPtr<CChooseLockPassword::ChooseLockPasswordFragment::Stage>
CChooseLockPassword::ChooseLockPasswordFragment::Stage::CONFIRM_WRONG = new Stage(
        String("CONFIRM_WRONG"), 2,
        R::string::lockpassword_confirm_passwords_dont_match,
        R::string::lockpassword_confirm_pins_dont_match,
        R::string::lockpassword_continue_label);

CAR_INTERFACE_IMPL(CChooseLockPassword::ChooseLockPasswordFragment::Stage, Object, IPasswordStage)

CChooseLockPassword::ChooseLockPasswordFragment::Stage::Stage(
    /* [in] */ const String& name,
    /* [in] */ Int32 ordinal,
    /* [in] */ Int32 hintInAlpha,
    /* [in] */ Int32 hintInNumeric,
    /* [in] */ Int32 nextButtonText)
    : mAlphaHint(hintInAlpha)
    , mNumericHint(hintInNumeric)
    , mButtonText(nextButtonText)
    , mName(name)
    , mOrdinal(ordinal)
{}

String CChooseLockPassword::ChooseLockPasswordFragment::Stage::GetName()
{
    return mName;
}

Int32 CChooseLockPassword::ChooseLockPasswordFragment::Stage::GetOrdinal()
{
    return mOrdinal;
}

AutoPtr<CChooseLockPassword::ChooseLockPasswordFragment::Stage> CChooseLockPassword::ChooseLockPasswordFragment::Stage::ValueOf(
    /* [in] */ const String& name)
{
    if (name.Equals("INTRODUCTION")) {
        return INTRODUCTION;
    }
    else if (name.Equals("NEED_TO_CONFIRM")) {
        return NEED_TO_CONFIRM;
    }
    else if (name.Equals("CONFIRM_WRONG")) {
        return CONFIRM_WRONG;
    }
    else {
        return NULL;
    }
}

//===============================================================================
//                  CChooseLockPassword::ChooseLockPasswordFragment::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL_3(CChooseLockPassword::ChooseLockPasswordFragment::InnerListener, Object, IViewOnClickListener, IOnEditorActionListener, ITextWatcher)

CChooseLockPassword::ChooseLockPasswordFragment::InnerListener::InnerListener(
    /* [in] */ ChooseLockPasswordFragment* host)
    : mHost(host)
{}

ECode CChooseLockPassword::ChooseLockPasswordFragment::InnerListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

ECode CChooseLockPassword::ChooseLockPasswordFragment::InnerListener::OnEditorAction(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 actionId,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    return mHost->OnEditorAction(v, actionId, event, result);
}

ECode CChooseLockPassword::ChooseLockPasswordFragment::InnerListener::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return mHost->BeforeTextChanged(s, start, count, after);
}

ECode CChooseLockPassword::ChooseLockPasswordFragment::InnerListener::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    return mHost->OnTextChanged(s, start, before, count);
}

ECode CChooseLockPassword::ChooseLockPasswordFragment::InnerListener::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    return mHost->AfterTextChanged(s);
}

//===============================================================================
//                  CChooseLockPassword::ChooseLockPasswordFragment::MyHandler
//===============================================================================

CChooseLockPassword::ChooseLockPasswordFragment::MyHandler::MyHandler(
    /* [in] */ ChooseLockPasswordFragment* host)
    : mHost(host)
{}

ECode CChooseLockPassword::ChooseLockPasswordFragment::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    if (what == MSG_SHOW_ERROR) {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        mHost->UpdateStage((Stage*)IPasswordStage::Probe(obj));
    }
    return NOERROR;
}

//===============================================================================
//                  CChooseLockPassword::ChooseLockPasswordFragment
//===============================================================================

const Int32 CChooseLockPassword::ChooseLockPasswordFragment::RESULT_FINISHED = RESULT_FIRST_USER;
const String CChooseLockPassword::ChooseLockPasswordFragment::KEY_FIRST_PIN("first_pin");
const String CChooseLockPassword::ChooseLockPasswordFragment::KEY_UI_STAGE("ui_stage");
const Int32 CChooseLockPassword::ChooseLockPasswordFragment::CONFIRM_EXISTING_REQUEST = 58;
const Int64 CChooseLockPassword::ChooseLockPasswordFragment::ERROR_MESSAGE_TIMEOUT = 3000;
const Int32 CChooseLockPassword::ChooseLockPasswordFragment::MSG_SHOW_ERROR = 1;

CChooseLockPassword::ChooseLockPasswordFragment::ChooseLockPasswordFragment()
    : mPasswordMinLength(4)
    , mPasswordMaxLength(16)
    , mPasswordMinLetters(0)
    , mPasswordMinUpperCase(0)
    , mPasswordMinLowerCase(0)
    , mPasswordMinSymbols(0)
    , mPasswordMinNumeric(0)
    , mPasswordMinNonLetter(0)
    , mRequestedQuality(IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC)
    , mDone(FALSE)
    , mIsAlphaMode(FALSE)
{}

CChooseLockPassword::ChooseLockPasswordFragment::~ChooseLockPasswordFragment()
{}

ECode CChooseLockPassword::ChooseLockPasswordFragment::constructor()
{
    mUiStage = Stage::INTRODUCTION;
    mHandler = new MyHandler(this);
    mHandler->constructor();
    return Fragment::constructor();
}

ECode CChooseLockPassword::ChooseLockPasswordFragment::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Fragment::OnCreate(savedInstanceState);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    CLockPatternUtils::New(IContext::Probe(activity), (ILockPatternUtils**)&mLockPatternUtils);
    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);
    if (IChooseLockPassword::Probe(activity) == NULL) {
        Slogger::E("CChooseLockPassword::ChooseLockPasswordFragment", "Fragment contained in wrong activity");
        return E_SECURITY_EXCEPTION;
    }

    Int32 intentData, lpuData;
    using Elastos::Core::Math;
    mRequestedQuality = Math::Max(
            (intent->GetInt32Extra(ILockPatternUtils::PASSWORD_TYPE_KEY, mRequestedQuality, &intentData), intentData),
            (mLockPatternUtils->GetRequestedPasswordQuality(&lpuData), lpuData));
    mPasswordMinLength = Math::Max(
            (intent->GetInt32Extra(PASSWORD_MIN_KEY, mPasswordMinLength, &intentData), intentData),
            (mLockPatternUtils->GetRequestedMinimumPasswordLength(&lpuData), lpuData));
    intent->GetInt32Extra(PASSWORD_MAX_KEY, mPasswordMaxLength, &mPasswordMaxLength);
    mPasswordMinLetters = Math::Max(
            (intent->GetInt32Extra(PASSWORD_MIN_LETTERS_KEY, mPasswordMinLetters, &intentData), intentData),
            (mLockPatternUtils->GetRequestedPasswordMinimumLetters(&lpuData), lpuData));
    mPasswordMinUpperCase = Math::Max(
            (intent->GetInt32Extra(PASSWORD_MIN_UPPERCASE_KEY, mPasswordMinUpperCase, &intentData), intentData),
            (mLockPatternUtils->GetRequestedPasswordMinimumUpperCase(&lpuData), lpuData));
    mPasswordMinLowerCase = Math::Max(
            (intent->GetInt32Extra(PASSWORD_MIN_LOWERCASE_KEY, mPasswordMinLowerCase, &intentData), intentData),
            (mLockPatternUtils->GetRequestedPasswordMinimumLowerCase(&lpuData), lpuData));
    mPasswordMinNumeric = Math::Max(
            (intent->GetInt32Extra(PASSWORD_MIN_NUMERIC_KEY, mPasswordMinNumeric, &intentData), intentData),
            (mLockPatternUtils->GetRequestedPasswordMinimumNumeric(&lpuData), lpuData));
    mPasswordMinSymbols = Math::Max(
            (intent->GetInt32Extra(PASSWORD_MIN_SYMBOLS_KEY, mPasswordMinSymbols, &intentData), intentData),
            (mLockPatternUtils->GetRequestedPasswordMinimumSymbols(&lpuData), lpuData));
    mPasswordMinNonLetter = Math::Max(
            (intent->GetInt32Extra(PASSWORD_MIN_NONLETTER_KEY, mPasswordMinNonLetter, &intentData), intentData),
            (mLockPatternUtils->GetRequestedPasswordMinimumNonLetter(&lpuData), lpuData));

    mChooseLockSettingsHelper = new ChooseLockSettingsHelper();
    mChooseLockSettingsHelper->constructor(activity);
    return NOERROR;
}

ECode CChooseLockPassword::ChooseLockPasswordFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IView> view;
    inflater->Inflate(R::layout::choose_lock_password, NULL, (IView**)&view);

    AutoPtr<InnerListener> listener = new InnerListener(this);
    AutoPtr<IView> cancelButtonTmp;
    view->FindViewById(R::id::cancel_button, (IView**)&cancelButtonTmp);
    mCancelButton = IButton::Probe(cancelButtonTmp);
    cancelButtonTmp->SetOnClickListener(listener);
    AutoPtr<IView> nextButtonTmp;
    view->FindViewById(R::id::next_button, (IView**)&nextButtonTmp);
    mNextButton = IButton::Probe(nextButtonTmp);
    nextButtonTmp->SetOnClickListener(listener);

    mIsAlphaMode = IDevicePolicyManager::PASSWORD_QUALITY_ALPHABETIC == mRequestedQuality
            || IDevicePolicyManager::PASSWORD_QUALITY_ALPHANUMERIC == mRequestedQuality
            || IDevicePolicyManager::PASSWORD_QUALITY_COMPLEX == mRequestedQuality;

    AutoPtr<IView> tmp;
    view->FindViewById(R::id::keyboard, (IView**)&tmp);
    mKeyboardView = IKeyboardView::Probe(tmp);
    tmp = NULL;
    view->FindViewById(R::id::password_entry, (IView**)&tmp);
    mPasswordEntry = ITextView::Probe(tmp);
    mPasswordEntry->SetOnEditorActionListener(listener);
    mPasswordEntry->AddTextChangedListener(listener);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    CPasswordEntryKeyboardHelper::New(IContext::Probe(activity), mKeyboardView,
            IView::Probe(mPasswordEntry), (IPasswordEntryKeyboardHelper**)&mKeyboardHelper);
    mKeyboardHelper->SetKeyboardMode(mIsAlphaMode ?
            IPasswordEntryKeyboardHelper::KEYBOARD_MODE_ALPHA
            : IPasswordEntryKeyboardHelper::KEYBOARD_MODE_NUMERIC);

    tmp = NULL;
    view->FindViewById(R::id::headerText, (IView**)&tmp);
    mHeaderText = ITextView::Probe(tmp);
    Boolean res;
    IView::Probe(mKeyboardView)->RequestFocus(&res);

    Int32 currentType;
    mPasswordEntry->GetInputType(&currentType);
    mPasswordEntry->SetInputType(mIsAlphaMode ? currentType
            : (IInputType::TYPE_CLASS_NUMBER | IInputType::TYPE_NUMBER_VARIATION_PASSWORD));

    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);
    Boolean confirmCredentials;
    intent->GetBooleanExtra(String("confirm_credentials"), TRUE, &confirmCredentials);
    if (savedInstanceState == NULL) {
        UpdateStage(Stage::INTRODUCTION);
        if (confirmCredentials) {
            mChooseLockSettingsHelper->LaunchConfirmationActivity(CONFIRM_EXISTING_REQUEST,
                    NULL, NULL);
        }
    }
    else {
        savedInstanceState->GetString(KEY_FIRST_PIN, &mFirstPin);
        String state;
        savedInstanceState->GetString(KEY_UI_STAGE, &state);
        if (state != NULL) {
            mUiStage = Stage::ValueOf(state);
            UpdateStage(mUiStage);
        }
    }
    mDone = FALSE;
    if (ISettingsActivity::Probe(activity) != NULL) {
        CSettingsActivity* sa = (CSettingsActivity*) ISettingsActivity::Probe(activity);
        Int32 id = mIsAlphaMode ? R::string::lockpassword_choose_your_password_header
                : R::string::lockpassword_choose_your_pin_header;
        AutoPtr<ICharSequence> title;
        GetText(id, (ICharSequence**)&title);
        sa->SetTitle(title);
    }

    *result = view;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CChooseLockPassword::ChooseLockPasswordFragment::OnResume()
{
    Fragment::OnResume();
    UpdateStage(mUiStage);
    Boolean res;
    return IView::Probe(mKeyboardView)->RequestFocus(&res);
}

ECode CChooseLockPassword::ChooseLockPasswordFragment::OnPause()
{
    mHandler->RemoveMessages(MSG_SHOW_ERROR);

    return Fragment::OnPause();
}

ECode CChooseLockPassword::ChooseLockPasswordFragment::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    Fragment::OnSaveInstanceState(outState);
    outState->PutString(KEY_UI_STAGE, mUiStage->GetName());
    outState->PutString(KEY_FIRST_PIN, mFirstPin);
    return NOERROR;
}

ECode CChooseLockPassword::ChooseLockPasswordFragment::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    Fragment::OnActivityResult(requestCode, resultCode, data);
    switch (requestCode) {
        case CONFIRM_EXISTING_REQUEST: {
            if (resultCode != IActivity::RESULT_OK) {
                AutoPtr<IActivity> activity;
                GetActivity((IActivity**)&activity);
                activity->SetResult(RESULT_FINISHED);
                activity->Finish();
            }
            break;
        }
    }
    return NOERROR;
}

void CChooseLockPassword::ChooseLockPasswordFragment::UpdateStage(
    /* [in] */ Stage* stage)
{
    AutoPtr<Stage> previousStage = mUiStage;
    mUiStage = stage;
    UpdateUi();

    // If the stage changed, announce the header for accessibility. This
    // is a no-op when accessibility is disabled.
    if (previousStage.Get() != stage) {
        AutoPtr<ICharSequence> cs;
        mHeaderText->GetText((ICharSequence**)&cs);
        IView::Probe(mHeaderText)->AnnounceForAccessibility(cs);
    }
}

String CChooseLockPassword::ChooseLockPasswordFragment::ValidatePassword(
    /* [in] */ const String& password)
{
    String result;
    if (password.GetLength() < mPasswordMinLength) {
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(mPasswordMinLength));
        GetString(mIsAlphaMode ?
                R::string::lockpassword_password_too_short
                : R::string::lockpassword_pin_too_short, args, &result);
        return result;
    }
    if (password.GetLength() > mPasswordMaxLength) {
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(mPasswordMaxLength + 1));
        GetString(mIsAlphaMode ?
                R::string::lockpassword_password_too_long
                : R::string::lockpassword_pin_too_long, args, &result);
        return result;
    }
    Int32 letters = 0;
    Int32 numbers = 0;
    Int32 lowercase = 0;
    Int32 symbols = 0;
    Int32 uppercase = 0;
    Int32 nonletter = 0;
    for (Int32 i = 0; i < password.GetLength(); i++) {
        Char32 c = password.GetChar(i);
        // allow non control Latin-1 characters only
        if (c < 32 || c > 127) {
            GetString(R::string::lockpassword_illegal_character, &result);
            return result;
        }
        if (c >= '0' && c <= '9') {
            numbers++;
            nonletter++;
        }
        else if (c >= 'A' && c <= 'Z') {
            letters++;
            uppercase++;
        }
        else if (c >= 'a' && c <= 'z') {
            letters++;
            lowercase++;
        }
        else {
            symbols++;
            nonletter++;
        }
    }
    if (IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC == mRequestedQuality
            || IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC_COMPLEX == mRequestedQuality) {
        if (letters > 0 || symbols > 0) {
            // This shouldn't be possible unless user finds some way to bring up
            // soft keyboard
            GetString(R::string::lockpassword_pin_contains_non_digits, &result);
            return result;
        }
        // Check for repeated characters or sequences (e.g. '1234', '0000', '2468')
        AutoPtr<ILockPatternUtilsHelper> helper;
        CLockPatternUtilsHelper::AcquireSingleton((ILockPatternUtilsHelper**)&helper);
        Int32 sequence;
        helper->MaxLengthSequence(password, &sequence);
        if (IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC_COMPLEX == mRequestedQuality
                && sequence > ILockPatternUtils::MAX_ALLOWED_SEQUENCE) {
            GetString(R::string::lockpassword_pin_no_sequential_digits, &result);
            return result;
        }
    }
    else if (IDevicePolicyManager::PASSWORD_QUALITY_COMPLEX == mRequestedQuality) {
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        String str;
        if (letters < mPasswordMinLetters) {
            resources->GetQuantityString(
                    R::plurals::lockpassword_password_requires_letters, mPasswordMinLetters, &str);
            args->Set(0, CoreUtils::Convert(mPasswordMinLetters));
            return StringUtils::Format(str, args);
        }
        else if (numbers < mPasswordMinNumeric) {
            resources->GetQuantityString(
                    R::plurals::lockpassword_password_requires_numeric, mPasswordMinNumeric, &str);
            args->Set(0, CoreUtils::Convert(mPasswordMinNumeric));
            return StringUtils::Format(str, args);
        }
        else if (lowercase < mPasswordMinLowerCase) {
            resources->GetQuantityString(
                    R::plurals::lockpassword_password_requires_lowercase, mPasswordMinLowerCase, &str);
            args->Set(0, CoreUtils::Convert(mPasswordMinLowerCase));
            return StringUtils::Format(str, args);
        }
        else if (uppercase < mPasswordMinUpperCase) {
            resources->GetQuantityString(
                    R::plurals::lockpassword_password_requires_uppercase, mPasswordMinUpperCase, &str);
            args->Set(0, CoreUtils::Convert(mPasswordMinUpperCase));
            return StringUtils::Format(str, args);
        }
        else if (symbols < mPasswordMinSymbols) {
            resources->GetQuantityString(
                    R::plurals::lockpassword_password_requires_symbols, mPasswordMinSymbols, &str);
            args->Set(0, CoreUtils::Convert(mPasswordMinSymbols));
            return StringUtils::Format(str, args);
        }
        else if (nonletter < mPasswordMinNonLetter) {
            resources->GetQuantityString(
                    R::plurals::lockpassword_password_requires_nonletter, mPasswordMinNonLetter, &str);
            args->Set(0, CoreUtils::Convert(mPasswordMinNonLetter));
            return StringUtils::Format(str, args);
        }
    }
    else {
        const Boolean alphabetic = IDevicePolicyManager::PASSWORD_QUALITY_ALPHABETIC
                == mRequestedQuality;
        const Boolean alphanumeric = IDevicePolicyManager::PASSWORD_QUALITY_ALPHANUMERIC
                == mRequestedQuality;
        if ((alphabetic || alphanumeric) && letters == 0) {
            GetString(R::string::lockpassword_password_requires_alpha, &result);
            return result;
        }
        if (alphanumeric && numbers == 0) {
            GetString(R::string::lockpassword_password_requires_digit, &result);
            return result;
        }
    }
    Boolean res;
    if (mLockPatternUtils->CheckPasswordHistory(password, &res), res) {
        GetString(mIsAlphaMode ? R::string::lockpassword_password_recently_used
                : R::string::lockpassword_pin_recently_used, &result);
        return result;
    }

    return String(NULL);
}

void CChooseLockPassword::ChooseLockPasswordFragment::HandleNext()
{
    Slogger::I("CChooseLockPassword", " >> HandleNext: %s", TO_CSTR(mUiStage));
    if (mDone) return;

    AutoPtr<ICharSequence> cs;
    mPasswordEntry->GetText((ICharSequence**)&cs);
    String pin;
    cs->ToString(&pin);
    if (TextUtils::IsEmpty(pin)) {
        return;
    }
    String errorMsg;
    if (mUiStage == Stage::INTRODUCTION) {
        errorMsg = ValidatePassword(pin);
        if (errorMsg.IsNull()) {
            mFirstPin = pin;
            mPasswordEntry->SetText(CoreUtils::Convert(""));
            UpdateStage(Stage::NEED_TO_CONFIRM);
        }
    }
    else if (mUiStage == Stage::NEED_TO_CONFIRM) {
        if (mFirstPin.Equals(pin)) {
            AutoPtr<IActivity> activity;
            GetActivity((IActivity**)&activity);
            AutoPtr<IIntent> intent;
            activity->GetIntent((IIntent**)&intent);
            Boolean isFallback;
            intent->GetBooleanExtra(
                    ILockPatternUtils::LOCKSCREEN_BIOMETRIC_WEAK_FALLBACK, FALSE, &isFallback);
            mLockPatternUtils->ClearLock(isFallback);
            Boolean required;
            intent->GetBooleanExtra(
                    CEncryptionInterstitial::EXTRA_REQUIRE_PASSWORD, TRUE, &required);
            mLockPatternUtils->SetCredentialRequiredToDecrypt(required);
            mLockPatternUtils->SaveLockPassword(pin, mRequestedQuality, isFallback);
            activity->SetResult(RESULT_FINISHED);
            activity->Finish();
            mDone = TRUE;
            StartActivity(CRedactionInterstitial::CreateStartIntent(IContext::Probe(activity)));
        }
        else {
            AutoPtr<ICharSequence> tmp;
            mPasswordEntry->GetText((ICharSequence**)&tmp);
            if (tmp != NULL) {
                AutoPtr<ISelection> selection;
                CSelection::AcquireSingleton((ISelection**)&selection);
                Int32 len;
                selection->SetSelection(ISpannable::Probe(tmp), 0, (tmp->GetLength(&len), len));
            }
            UpdateStage(Stage::CONFIRM_WRONG);
        }
    }
    if (!errorMsg.IsNull()) {
        ShowError(errorMsg, mUiStage);
    }
}

ECode CChooseLockPassword::ChooseLockPasswordFragment::OnClick(
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
            activity->Finish();
            break;
        }
    }
    return NOERROR;
}

void CChooseLockPassword::ChooseLockPasswordFragment::ShowError(
    /* [in] */ const String& msg,
    /* [in] */ Stage* next)
{
    mHeaderText->SetText(CoreUtils::Convert(msg));
    AutoPtr<ICharSequence> cs;
    mHeaderText->GetText((ICharSequence**)&cs);
    IView::Probe(mHeaderText)->AnnounceForAccessibility(cs);
    AutoPtr<IMessage> mesg;
    mHandler->ObtainMessage(MSG_SHOW_ERROR, (IPasswordStage*)next, (IMessage**)&mesg);
    mHandler->RemoveMessages(MSG_SHOW_ERROR);
    Boolean res;
    mHandler->SendMessageDelayed(mesg, ERROR_MESSAGE_TIMEOUT, &res);
}

ECode CChooseLockPassword::ChooseLockPasswordFragment::OnEditorAction(
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

void CChooseLockPassword::ChooseLockPasswordFragment::UpdateUi()
{
    AutoPtr<ICharSequence> cs;
    mPasswordEntry->GetText((ICharSequence**)&cs);
    String password;
    cs->ToString(&password);
    const Int32 length = password.GetLength();
    if (mUiStage == Stage::INTRODUCTION && length > 0) {
        if (length < mPasswordMinLength) {
            AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, CoreUtils::Convert(mPasswordMinLength));
            String msg;
            GetString(mIsAlphaMode ? R::string::lockpassword_password_too_short
                    : R::string::lockpassword_pin_too_short, args, &msg);
            mHeaderText->SetText(CoreUtils::Convert(msg));
            IView::Probe(mNextButton)->SetEnabled(FALSE);
        }
        else {
            String error = ValidatePassword(password);
            if (error != NULL) {
                mHeaderText->SetText(CoreUtils::Convert(error));
                IView::Probe(mNextButton)->SetEnabled(FALSE);
            }

            else {
                mHeaderText->SetText(R::string::lockpassword_press_continue);
                IView::Probe(mNextButton)->SetEnabled(TRUE);
            }
        }
    }
    else {
        mHeaderText->SetText(mIsAlphaMode ? mUiStage->mAlphaHint : mUiStage->mNumericHint);
        IView::Probe(mNextButton)->SetEnabled(length > 0);
    }
    ITextView::Probe(mNextButton)->SetText(mUiStage->mButtonText);
}

ECode CChooseLockPassword::ChooseLockPasswordFragment::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    // Changing the text while error displayed resets to NeedToConfirm state
    if (mUiStage == Stage::CONFIRM_WRONG) {
        mUiStage = Stage::NEED_TO_CONFIRM;
    }
    UpdateUi();
    return NOERROR;
}

ECode CChooseLockPassword::ChooseLockPasswordFragment::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return NOERROR;
}

ECode CChooseLockPassword::ChooseLockPasswordFragment::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    return NOERROR;
}

//===============================================================================
//                  CChooseLockPassword
//===============================================================================

const String CChooseLockPassword::PASSWORD_MIN_KEY("lockscreen.password_min");
const String CChooseLockPassword::PASSWORD_MAX_KEY("lockscreen.password_max");
const String CChooseLockPassword::PASSWORD_MIN_LETTERS_KEY("lockscreen.password_min_letters");
const String CChooseLockPassword::PASSWORD_MIN_LOWERCASE_KEY("lockscreen.password_min_lowercase");
const String CChooseLockPassword::PASSWORD_MIN_UPPERCASE_KEY("lockscreen.password_min_uppercase");
const String CChooseLockPassword::PASSWORD_MIN_NUMERIC_KEY("lockscreen.password_min_numeric");
const String CChooseLockPassword::PASSWORD_MIN_SYMBOLS_KEY("lockscreen.password_min_symbols");
const String CChooseLockPassword::PASSWORD_MIN_NONLETTER_KEY("lockscreen.password_min_nonletter");

CAR_INTERFACE_IMPL(CChooseLockPassword, SettingsActivity, IChooseLockPassword)

CAR_OBJECT_IMPL(CChooseLockPassword)

CChooseLockPassword::CChooseLockPassword()
{}

CChooseLockPassword::~CChooseLockPassword()
{}

ECode CChooseLockPassword::constructor()
{
    return SettingsActivity::constructor();
}

ECode CChooseLockPassword::GetIntent(
    /* [out] */ IIntent** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IIntent> intentOne;
    SettingsActivity::GetIntent((IIntent**)&intentOne);
    AutoPtr<IIntent> modIntent;
    CIntent::New(intentOne, (IIntent**)&modIntent);
    modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, String("Elastos.Droid.Settings.CChooseLockPasswordFragment"));
    *result = modIntent;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

 AutoPtr<IIntent> CChooseLockPassword::CreateIntent(
    /* [in] */ IContext* context,
    /* [in] */ Int32 quality,
    /* [in] */ Boolean isFallback,
    /* [in] */ Int32 minLength,
    /* [in] */ Int32 maxLength,
    /* [in] */ Boolean requirePasswordToDecrypt,
    /* [in] */ Boolean confirmCredentials)
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetClass(context, ECLSID_CChooseLockPassword);
    intent->PutExtra(ILockPatternUtils::PASSWORD_TYPE_KEY, quality);
    intent->PutExtra(PASSWORD_MIN_KEY, minLength);
    intent->PutExtra(PASSWORD_MAX_KEY, maxLength);
    intent->PutBooleanExtra(CChooseLockGeneric::CONFIRM_CREDENTIALS, confirmCredentials);
    intent->PutBooleanExtra(ILockPatternUtils::LOCKSCREEN_BIOMETRIC_WEAK_FALLBACK, isFallback);
    intent->PutBooleanExtra(CEncryptionInterstitial::EXTRA_REQUIRE_PASSWORD, requirePasswordToDecrypt);
    return intent;
}

Boolean CChooseLockPassword::IsValidFragment(
    /* [in] */ const String& fragmentName)
{
    if (String("Elastos.Droid.Settings.CChooseLockPasswordFragment").Equals(fragmentName)) return TRUE;
    return FALSE;
}

ECode CChooseLockPassword::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    // TODO: Fix on phones
    // Disable IME on our window since we provide our own keyboard
    //GetWindow()->SetFlags(WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM,
            //WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM);
    SettingsActivity::OnCreate(savedInstanceState);
    AutoPtr<ICharSequence> msg;
    GetText(R::string::lockpassword_choose_your_password_header, (ICharSequence**)&msg);
    SetTitle(msg);
    return NOERROR;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos