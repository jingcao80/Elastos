
#include "elastos/droid/settings/CIccLockSettings.h"
#include "elastos/droid/settings/CEditPinPreference.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/os/AsyncResult.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::CPhoneFactory;
using Elastos::Droid::Internal::Telephony::IIccCard;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Preference::IDialogPreference;
using Elastos::Droid::Preference::IEditTextPreference;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  CIccLockSettings::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(CIccLockSettings::InnerListener, Object, IEditPinPreferenceOnPinEnteredListener)

CIccLockSettings::InnerListener::InnerListener(
    /* [in] */ CIccLockSettings* host)
    : mHost(host)
{}

ECode CIccLockSettings::InnerListener::OnPinEntered(
    /* [in] */ IEditPinPreference* preference,
    /* [in] */ Boolean positiveResult)
{
    return mHost->OnPinEntered(preference, positiveResult);
}

//===============================================================================
//                  CIccLockSettings::MyHandler
//===============================================================================

CIccLockSettings::MyHandler::MyHandler(
    /* [in] */ CIccLockSettings* host)
    : mHost(host)
{}

ECode CIccLockSettings::MyHandler::constructor()
{
    return Handler::constructor();
}

ECode CIccLockSettings::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    IAsyncResult* ar = IAsyncResult::Probe(obj);
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_ENABLE_ICC_PIN_COMPLETE: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->IccLockChanged(((AsyncResult*)ar)->mException == NULL, arg1);
            break;
        }
        case MSG_CHANGE_ICC_PIN_COMPLETE:{
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->IccPinChanged(((AsyncResult*)ar)->mException == NULL, arg1);
            break;
        }
        case MSG_SIM_STATE_CHANGED:
            mHost->UpdatePreferences();
            break;
    }

    return NOERROR;
}

//===============================================================================
//                  CIccLockSettings::MyBroadcastReceiver
//===============================================================================

CIccLockSettings::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ CIccLockSettings* host)
    : mHost(host)
{}

ECode CIccLockSettings::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (ITelephonyIntents::ACTION_SIM_STATE_CHANGED.Equals(action)) {
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_SIM_STATE_CHANGED, (IMessage**)&msg);
        Boolean res;
        mHost->mHandler->SendMessage(msg, &res);
    }
    return NOERROR;
}

//===============================================================================
//                  CIccLockSettings
//===============================================================================

const String CIccLockSettings::TAG("CIccLockSettings");
const Boolean CIccLockSettings::DBG = TRUE;

const Int32 CIccLockSettings::OFF_MODE = 0;
const Int32 CIccLockSettings::ICC_LOCK_MODE = 1;
const Int32 CIccLockSettings::ICC_OLD_MODE = 2;
const Int32 CIccLockSettings::ICC_NEW_MODE = 3;
const Int32 CIccLockSettings::ICC_REENTER_MODE = 4;

const String CIccLockSettings::PIN_DIALOG("sim_pin");
const String CIccLockSettings::PIN_TOGGLE("sim_toggle");
const String CIccLockSettings::DIALOG_STATE("dialogState");
const String CIccLockSettings::DIALOG_PIN("dialogPin");
const String CIccLockSettings::DIALOG_ERROR("dialogError");
const String CIccLockSettings::ENABLE_TO_STATE("enableState");

const String CIccLockSettings::OLD_PINCODE("oldPinCode");
const String CIccLockSettings::NEW_PINCODE("newPinCode");

const Int32 CIccLockSettings::MIN_PIN_LENGTH = 4;
const Int32 CIccLockSettings::MAX_PIN_LENGTH = 8;

const Int32 CIccLockSettings::MSG_ENABLE_ICC_PIN_COMPLETE;
const Int32 CIccLockSettings::MSG_CHANGE_ICC_PIN_COMPLETE;
const Int32 CIccLockSettings::MSG_SIM_STATE_CHANGED;

CAR_OBJECT_IMPL(CIccLockSettings)

CIccLockSettings::CIccLockSettings()
    : mDialogState(OFF_MODE)
    , mToState(FALSE)
{}

CIccLockSettings::~CIccLockSettings()
{}

ECode CIccLockSettings::constructor()
{
    mHandler = new MyHandler(this);
    mHandler->constructor();
    mSimStateReceiver = new MyBroadcastReceiver(this);

    return PreferenceActivity::constructor();
}

Boolean CIccLockSettings::IsIccLockEnabled()
{
    AutoPtr<IPhoneFactory> pf;
    CPhoneFactory::AcquireSingleton((IPhoneFactory**)&pf);
    AutoPtr<IPhone> phone;
    pf->GetDefaultPhone((IPhone**)&phone);
    AutoPtr<IIccCard> iccCard;
    phone->GetIccCard((IIccCard**)&iccCard);
    Boolean res;
    iccCard->GetIccLockEnabled(&res);
    return res;
}

String CIccLockSettings::GetSummary(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    String summary;
    if (IsIccLockEnabled()) {
        res->GetString(R::string::sim_lock_on, &summary);
    }
    else {
        res->GetString(R::string::sim_lock_off, &summary);
    }
    return summary;
}

ECode CIccLockSettings::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    PreferenceActivity::OnCreate(savedInstanceState);

    if (Utils::IsMonkeyRunning()) {
        Finish();
        return NOERROR;
    }

    AddPreferencesFromResource(R::xml::sim_lock_settings);

    AutoPtr<IPreference> pref;
    FindPreference(PIN_DIALOG, (IPreference**)&pref);
    mPinDialog = IEditPinPreference::Probe(pref);
    pref = NULL;
    FindPreference(PIN_TOGGLE, (IPreference**)&pref);
    mPinToggle = ICheckBoxPreference::Probe(pref);
    Boolean res;
    if (savedInstanceState != NULL && (savedInstanceState->ContainsKey(DIALOG_STATE, &res), res)) {
        savedInstanceState->GetInt32(DIALOG_STATE, &mDialogState);
        savedInstanceState->GetString(DIALOG_PIN, &mPin);
        savedInstanceState->GetString(DIALOG_ERROR, &mError);
        savedInstanceState->GetBoolean(ENABLE_TO_STATE, &mToState);

        // Restore inputted PIN code
        switch (mDialogState) {
            case ICC_NEW_MODE:
                savedInstanceState->GetString(OLD_PINCODE, &mOldPin);
                break;

            case ICC_REENTER_MODE:
                savedInstanceState->GetString(OLD_PINCODE, &mOldPin);
                savedInstanceState->GetString(NEW_PINCODE, &mNewPin);
                break;

            case ICC_LOCK_MODE:
            case ICC_OLD_MODE:
            default:
                break;
        }
    }

    AutoPtr<InnerListener> listener = new InnerListener(this);
    ((CEditPinPreference*)mPinDialog.Get())->SetOnPinEnteredListener(listener);

    // Don't need any changes to be remembered
    AutoPtr<IPreferenceScreen> screen;
    GetPreferenceScreen((IPreferenceScreen**)&screen);
    IPreference::Probe(screen)->SetPersistent(FALSE);

    AutoPtr<IPhoneFactory> pf;
    CPhoneFactory::AcquireSingleton((IPhoneFactory**)&pf);
    pf->GetDefaultPhone((IPhone**)&mPhone);
    GetResources((IResources**)&mRes);
    UpdatePreferences();
    return NOERROR;
}

void CIccLockSettings::UpdatePreferences()
{
    AutoPtr<IIccCard> iccCard;
    mPhone->GetIccCard((IIccCard**)&iccCard);
    Boolean res;
    iccCard->GetIccLockEnabled(&res);

    ITwoStatePreference::Probe(mPinToggle)->SetChecked(res);
}

ECode CIccLockSettings::OnResume()
{
    PreferenceActivity::OnResume();

    // ACTION_SIM_STATE_CHANGED is sticky, so we'll receive current state after this call,
    // which will call UpdatePreferences().
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(ITelephonyIntents::ACTION_SIM_STATE_CHANGED, (IIntentFilter**)&filter);
    AutoPtr<IIntent> resultIntent;
    RegisterReceiver(mSimStateReceiver, filter, (IIntent**)&resultIntent);

    if (mDialogState != OFF_MODE) {
        ShowPinDialog();
    }
    else {
        // Prep for standard click on "Change PIN"
        ResetDialogState();
    }
    return NOERROR;
}

ECode CIccLockSettings::OnPause()
{
    PreferenceActivity::OnPause();
    UnregisterReceiver(mSimStateReceiver);
    return NOERROR;
}

ECode CIccLockSettings::OnSaveInstanceState(
    /* [in] */ IBundle* out)
{
    // Need to store this state for slider open/close
    // There is one case where the dialog is popped up by the preference
    // framework. In that case, let the preference framework store the
    // dialog state. In other cases, where this activity manually launches
    // the dialog, store the state of the dialog.
    if (((CEditPinPreference*)mPinDialog.Get())->IsDialogOpen()) {
        out->PutInt32(DIALOG_STATE, mDialogState);
        AutoPtr<IEditText> editText;
        IEditTextPreference::Probe(mPinDialog)->GetEditText((IEditText**)&editText);
        AutoPtr<ICharSequence> cs;
        ITextView::Probe(editText)->GetText((ICharSequence**)&cs);
        String str;
        cs->ToString(&str);
        out->PutString(DIALOG_PIN, str);
        out->PutString(DIALOG_ERROR, mError);
        out->PutBoolean(ENABLE_TO_STATE, mToState);

        // Save inputted PIN code
        switch (mDialogState) {
            case ICC_NEW_MODE:
                out->PutString(OLD_PINCODE, mOldPin);
                break;

            case ICC_REENTER_MODE:
                out->PutString(OLD_PINCODE, mOldPin);
                out->PutString(NEW_PINCODE, mNewPin);
                break;

            case ICC_LOCK_MODE:
            case ICC_OLD_MODE:
            default:
                break;
        }
    }
    else {
        PreferenceActivity::OnSaveInstanceState(out);
    }
    return NOERROR;
}

void CIccLockSettings::ShowPinDialog()
{
    if (mDialogState == OFF_MODE) {
        return;
    }
    SetDialogValues();

    ((CEditPinPreference*)mPinDialog.Get())->ShowPinDialog();
}

void CIccLockSettings::SetDialogValues()
{
    IEditTextPreference::Probe(mPinDialog)->SetText(mPin);
    String message("");
    String str;
    IDialogPreference* pinDialog = IDialogPreference::Probe(mPinDialog);
    switch (mDialogState) {
        case ICC_LOCK_MODE:
            mRes->GetString(R::string::sim_enter_pin, &message);
            if (mToState) {
                mRes->GetString(R::string::sim_enable_sim_lock, &str);
            }
            else {
                mRes->GetString(R::string::sim_disable_sim_lock, &str);
            }
            pinDialog->SetDialogTitle(CoreUtils::Convert(str));
            break;
        case ICC_OLD_MODE:
            mRes->GetString(R::string::sim_enter_old, &message);
            mRes->GetString(R::string::sim_change_pin, &str);
            pinDialog->SetDialogTitle(CoreUtils::Convert(str));
            break;
        case ICC_NEW_MODE:
            mRes->GetString(R::string::sim_enter_new, &message);
            mRes->GetString(R::string::sim_change_pin, &str);
            pinDialog->SetDialogTitle(CoreUtils::Convert(str));
            break;
        case ICC_REENTER_MODE:
            mRes->GetString(R::string::sim_reenter_new, &message);
            mRes->GetString(R::string::sim_change_pin, &str);
            pinDialog->SetDialogTitle(CoreUtils::Convert(str));
            break;
    }
    if (!mError.IsNull()) {
        message = mError + "\n" + message;
        mError = String(NULL);
    }
    pinDialog->SetDialogMessage(CoreUtils::Convert(message));
}

ECode CIccLockSettings::OnPinEntered(
    /* [in] */ IEditPinPreference* preference,
    /* [in] */ Boolean positiveResult)
{
    if (!positiveResult) {
        ResetDialogState();
        return NOERROR;
    }

    IEditTextPreference::Probe(preference)->GetText(&mPin);
    if (!ReasonablePin(mPin)) {
        // inject error message and display dialog again
        mRes->GetString(R::string::sim_bad_pin, &mError);
        ShowPinDialog();
        return NOERROR;
    }
    switch (mDialogState) {
        case ICC_LOCK_MODE:
            TryChangeIccLockState();
            break;
        case ICC_OLD_MODE:
            mOldPin = mPin;
            mDialogState = ICC_NEW_MODE;
            mError = NULL;
            mPin = NULL;
            ShowPinDialog();
            break;
        case ICC_NEW_MODE:
            mNewPin = mPin;
            mDialogState = ICC_REENTER_MODE;
            mPin = NULL;
            ShowPinDialog();
            break;
        case ICC_REENTER_MODE:
            if (!mPin.Equals(mNewPin)) {
                mRes->GetString(R::string::sim_pins_dont_match, &mError);
                mDialogState = ICC_NEW_MODE;
                mPin = NULL;
                ShowPinDialog();
            }
            else {
                mError = NULL;
                TryChangePin();
            }
            break;
    }
    return NOERROR;
}

ECode CIccLockSettings::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (preference == IPreference::Probe(mPinToggle)) {
        // Get the new, preferred state
        ITwoStatePreference* pinToggle = ITwoStatePreference::Probe(mPinToggle);
        pinToggle->IsChecked(&mToState);
        // Flip it back and pop up pin dialog
        pinToggle->SetChecked(!mToState);
        mDialogState = ICC_LOCK_MODE;
        ShowPinDialog();
    }
    else if (preference == IPreference::Probe(mPinDialog)) {
        mDialogState = ICC_OLD_MODE;
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

void CIccLockSettings::TryChangeIccLockState()
{
    // Try to change icc lock. If it succeeds, toggle the lock state and
    // reset dialog state. Else inject error message and show dialog again.
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> callback;
    helper->Obtain(mHandler, MSG_ENABLE_ICC_PIN_COMPLETE, (IMessage**)&callback);

    AutoPtr<IIccCard> iccCard;
    mPhone->GetIccCard((IIccCard**)&iccCard);
    iccCard->SetIccLockEnabled(mToState, mPin, callback);
    // Disable the setting till the response is received.
    IPreference::Probe(mPinToggle)->SetEnabled(FALSE);
}

void CIccLockSettings::IccLockChanged(
    /* [in] */ Boolean success,
    /* [in] */ Int32 attemptsRemaining)
{
    if (success) {
        ITwoStatePreference::Probe(mPinToggle)->SetChecked(mToState);
    }
    else {
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        helper->MakeText(this, CoreUtils::Convert(GetPinPasswordErrorMessage(attemptsRemaining)),
                IToast::LENGTH_LONG, (IToast**)&toast);
        toast->Show();
    }
    IPreference::Probe(mPinToggle)->SetEnabled(TRUE);
    ResetDialogState();
}

void CIccLockSettings::IccPinChanged(
    /* [in] */ Boolean success,
    /* [in] */ Int32 attemptsRemaining)
{
    AutoPtr<IToastHelper> helper;
    CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    AutoPtr<IToast> toast;
    if (!success) {
        helper->MakeText(this, CoreUtils::Convert(GetPinPasswordErrorMessage(attemptsRemaining)),
                IToast::LENGTH_LONG, (IToast**)&toast);
        toast->Show();
    }
    else {
        String str;
        mRes->GetString(R::string::sim_change_succeeded, &str);
        helper->MakeText(this, CoreUtils::Convert(str), IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();

    }
    ResetDialogState();
}

void CIccLockSettings::TryChangePin()
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> callback;
    helper->Obtain(mHandler, MSG_CHANGE_ICC_PIN_COMPLETE, (IMessage**)&callback);
    AutoPtr<IIccCard> iccCard;
    mPhone->GetIccCard((IIccCard**)&iccCard);
    iccCard->ChangeIccLockPassword(mOldPin, mNewPin, callback);
}

String CIccLockSettings::GetPinPasswordErrorMessage(
    /* [in] */ Int32 attemptsRemaining)
{
    String displayMessage;

    if (attemptsRemaining == 0) {
        mRes->GetString(R::string::wrong_pin_code_pukked, &displayMessage);
    }
    else if (attemptsRemaining > 0) {
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(attemptsRemaining));

        mRes->GetQuantityString(R::plurals::wrong_pin_code, attemptsRemaining, args, &displayMessage);
    }
    else {
        mRes->GetString(R::string::pin_failed, &displayMessage);
    }

    if (DBG) {
        Logger::D(TAG, "getPinPasswordErrorMessage: attemptsRemaining=%d displayMessage=%s",
            attemptsRemaining, displayMessage.string());
    }
    return displayMessage;
}

Boolean CIccLockSettings::ReasonablePin(
    /* [in] */ const String& pin)
{
    if (pin.IsNull() || pin.GetLength() < MIN_PIN_LENGTH || pin.GetLength() > MAX_PIN_LENGTH) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

void CIccLockSettings::ResetDialogState()
{
    mError = String(NULL);
    mDialogState = ICC_OLD_MODE; // Default for when Change PIN is clicked
    mPin = String("");
    SetDialogValues();
    mDialogState = OFF_MODE;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos