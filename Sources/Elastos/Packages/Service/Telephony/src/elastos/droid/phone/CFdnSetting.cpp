
#include "elastos/droid/phone/CFdnSetting.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CFdnSetting::MyHandler::MyHandler(
    /* [in] */ CFdnSetting* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CFdnSetting::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        // when we are enabling FDN, either we are unsuccessful and display
        // a toast, or just update the UI.
        case EVENT_PIN2_ENTRY_COMPLETE:
        {
            AsyncResult ar = (AsyncResult) msg.obj;
            if (ar.exception != null && ar.exception instanceof CommandException) {
                Int32 attemptsRemaining = msg.arg1;
                // see if PUK2 is requested and alert the user accordingly.
                CommandException.Error e =
                        ((CommandException) ar.exception).getCommandError();
                switch (e) {
                    case SIM_PUK2:
                        // make sure we set the PUK2 state so that we can skip
                        // some redundant behaviour.
                        DisplayMessage(R.string.fdn_enable_puk2_requested,
                                attemptsRemaining);
                        ResetPinChangeStateForPUK2();
                        break;
                    case PASSWORD_INCORRECT:
                        DisplayMessage(R.string.pin2_invalid, attemptsRemaining);
                        break;
                    default:
                        DisplayMessage(R.string.fdn_failed, attemptsRemaining);
                        break;
                }
            }
            updateEnableFDN();
            break;
        }
        // when changing the pin we need to pay attention to whether or not
        // the error requests a PUK (usually after too many incorrect tries)
        // Set the state accordingly.
        case EVENT_PIN2_CHANGE_COMPLETE:
        {
            if (DBG) Log(String("Handle EVENT_PIN2_CHANGE_COMPLETE"));
            AsyncResult ar = (AsyncResult) msg.obj;
            if (ar.exception != null) {
                Int32 attemptsRemaining = msg.arg1;
                StringBuilder sb;
                sb += "Handle EVENT_PIN2_CHANGE_COMPLETE attemptsRemaining=";
                sb += attemptsRemaining;
                Log(sb.ToString());
                CommandException ce = (CommandException) ar.exception;
                if (ce.getCommandError() == CommandException.Error.SIM_PUK2) {
                    // throw an alert dialog on the screen, displaying the
                    // request for a PUK2.  set the cancel listener to
                    // FdnSetting.onCancel().
                    AutoPtr<IAlertDialogBuilder> builder;
                    CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
                    builder->SetMessage(R.string.puk2_requested);
                    builder->SetCancelable(TRUE);
                    builder->SetOnCancelListener(this);
                    AutoPtr<IAlertDialog> a;
                    builder->Create((IAlertDialog**)&a);
                    AutoPtr<IWindow> window;
                    a->GetWindow((IWindow**)&window);
                    window->AddFlags(IWindowManagerLayoutParams::FLAG_DIM_BEHIND);
                    a->Show();
                }
                else {
                    // set the correct error message depending upon the state.
                    // Reset the state depending upon or knowledge of the PUK state.
                    if (!mIsPuk2Locked) {
                        DisplayMessage(R.string.badPin2, attemptsRemaining);
                        ResetPinChangeState();
                    }
                    else {
                        DisplayMessage(R.string.badPuk2, attemptsRemaining);
                        ResetPinChangeStateForPUK2();
                    }
                }
            }
            else {
                if (mPinChangeState == PIN_CHANGE_PUK) {
                    DisplayMessage(R.string.pin2_unblocked);
                }
                else {
                    DisplayMessage(R.string.pin2_changed);
                }

                // reset to normal behaviour on successful change.
                ResetPinChangeState();
            }
            break;
        }
    }
    return NOERROR;
}

const String CFdnSetting::LOG_TAG("CFdnSetting");// = PhoneGlobals.LOG_TAG;
const Boolean CFdnSetting::DBG = FALSE;

const Int32 CFdnSetting::EVENT_PIN2_ENTRY_COMPLETE = 100;
const Int32 CFdnSetting::EVENT_PIN2_CHANGE_COMPLETE = 200;

const String CFdnSetting::BUTTON_FDN_ENABLE_KEY("button_fdn_enable_key");
const String CFdnSetting::BUTTON_CHANGE_PIN2_KEY("button_change_pin2_key");

const Int32 CFdnSetting::PIN_CHANGE_OLD = 0;
const Int32 CFdnSetting::PIN_CHANGE_NEW = 1;
const Int32 CFdnSetting::PIN_CHANGE_REENTER = 2;
const Int32 CFdnSetting::PIN_CHANGE_PUK = 3;
const Int32 CFdnSetting::PIN_CHANGE_NEW_PIN_FOR_PUK = 4;
const Int32 CFdnSetting::PIN_CHANGE_REENTER_PIN_FOR_PUK = 5;

const String CFdnSetting::SKIP_OLD_PIN_KEY("skip_old_pin_key");
const String CFdnSetting::PIN_CHANGE_STATE_KEY("pin_change_state_key");
const String CFdnSetting::OLD_PIN_KEY("old_pin_key");
const String CFdnSetting::NEW_PIN_KEY("new_pin_key");
const String CFdnSetting::DIALOG_MESSAGE_KEY("dialog_message_key");
const String CFdnSetting::DIALOG_PIN_ENTRY_KEY("dialog_pin_entry_key");

const Int32 CFdnSetting::MIN_PIN_LENGTH = 4;
const Int32 CFdnSetting::MAX_PIN_LENGTH = 8;

CAR_INTERFACE_IMPL_3(CFdnSetting, PreferenceActivity, IFdnSetting, IEditPinPreferenceOnPinEnteredListener,
        IDialogInterfaceOnCancelListener)

CAR_OBJECT_IMPL(CFdnSetting)

CFdnSetting::CFdnSetting()
    : mOldPin(NULL)
    , mNewPin(NULL)
    , mPuk2(NULL)
    , mPinChangeState(0)
    , mIsPuk2Locked(FALSE)
{
    mFDNHandler = new MyHandler(this);
}

ECode CFdnSetting::constructor()
{
    return PreferenceActivity::constructor();
}

ECode CFdnSetting::OnPinEntered(
    /* [in] */ IEditPinPreference* preference,
    /* [in] */ Boolean positiveResult)
{
    if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonEnableFDN)) {
        ToggleFDNEnable(positiveResult);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonChangePin2)){
        UpdatePINChangeState(positiveResult);
    }
    return NOERROR;
}

void CFdnSetting::ToggleFDNEnable(
    /* [in] */ Boolean positiveResult)
{
    if (!positiveResult) {
        return;
    }

    // validate the pin first, before submitting it to the RIL for FDN enable.
    String password;
    mButtonEnableFDN->GetText(&password);
    if (ValidatePin(password, FALSE)) {
        // get the relevant data for the icc call
        AutoPtr<IIccCard> card;
        mPhone->GetIccCard((IIccCard**)&card);
        Boolean isEnabled;
        card->GetIccFdnEnabled(&isEnabled);

        AutoPtr<IMessage> onComplete;
        mFDNHandler->ObtainMessage(EVENT_PIN2_ENTRY_COMPLETE, (IMessage**)&onComplete);

        // make fdn request
        card->SetIccFdnEnabled(!isEnabled, password, onComplete);
    }
    else {
        // throw up error if the pin is invalid.
        DisplayMessage(R.string.invalidPin2);
    }

    mButtonEnableFDN->SetText(String(""));
}

void CFdnSetting::UpdatePINChangeState(
    /* [in] */ Boolean positiveResult)
{
    if (DBG) {
        StringBuilder sb;
        sb += "updatePINChangeState positive=";
        sb += positiveResult;
        sb += " mPinChangeState=";
        sb += mPinChangeState;
        sb += " mSkipOldPin=";
        sb += mIsPuk2Locked;
        log(sb.ToString());
    }

    if (!positiveResult) {
        // reset the state on cancel, either to expect PUK2 or PIN2
        if (!mIsPuk2Locked) {
            ResetPinChangeState();
        }
        else {
            ResetPinChangeStateForPUK2();
        }
        return;
    }

    // Progress through the dialog states, generally in this order:
    //   1. Enter old pin
    //   2. Enter new pin
    //   3. Re-Enter new pin
    // While handling any error conditions that may show up in between.
    // Also handle the PUK2 entry, if it is requested.
    //
    // In general, if any invalid entries are made, the dialog re-
    // appears with text to indicate what the issue is.
    switch (mPinChangeState) {
        case PIN_CHANGE_OLD:
        {
            mButtonChangePin2->GetText(&mOldPin);
            mButtonChangePin2->SetText(String(""));
            // if the pin is not valid, display a message and reset the state.
            if (ValidatePin(mOldPin, FALSE)) {
                mPinChangeState = PIN_CHANGE_NEW;
                DisplayPinChangeDialog();
            }
            else {
                DisplayPinChangeDialog(R.string.invalidPin2, TRUE);
            }
            break;
        }
        case PIN_CHANGE_NEW:
        {
            mButtonChangePin2->GetText(&mNewPin);
            mButtonChangePin2->SetText(String(""));
            // if the new pin is not valid, display a message and reset the state.
            if (ValidatePin(mNewPin, FALSE)) {
                mPinChangeState = PIN_CHANGE_REENTER;
                DisplayPinChangeDialog();
            } else {
                DisplayPinChangeDialog(R.string.invalidPin2, TRUE);
            }
            break;
        }
        case PIN_CHANGE_REENTER:
        {
            String text;
            mButtonChangePin2->GetText(&text);
            // if the re-entered pin is not valid, display a message and reset the state.
            if (!mNewPin.Equals(text)) {
                mPinChangeState = PIN_CHANGE_NEW;
                mButtonChangePin2->SetText(String(""));
                DisplayPinChangeDialog(R.string.mismatchPin2, TRUE);
            }
            else {
                // If the PIN is valid, then we submit the change PIN request.
                mButtonChangePin2->SetText(String(""));
                AutoPtr<IMessage> onComplete;
                mFDNHandler->ObtainMessage(
                        EVENT_PIN2_CHANGE_COMPLETE, (IMessage**)&onComplete);
                AutoPtr<IIccCard> card;
                mPhone->GetIccCard((IIccCard**)&card);
                card->ChangeIccFdnPassword(mOldPin, mNewPin, onComplete);
            }
            break;
        }
        case PIN_CHANGE_PUK:
        {
                // Doh! too many incorrect requests, PUK requested.
                mButtonChangePin2->GetText(&mPuk2);
                mButtonChangePin2->SetText(String(""));
                // if the puk is not valid, display
                // a message and reset the state.
                if (ValidatePin(mPuk2, TRUE)) {
                    mPinChangeState = PIN_CHANGE_NEW_PIN_FOR_PUK;
                    DisplayPinChangeDialog();
                }
                else {
                    DisplayPinChangeDialog(R.string.invalidPuk2, TRUE);
                }
            break;
        }
        case PIN_CHANGE_NEW_PIN_FOR_PUK:
        {
            mButtonChangePin2->GetText(&mNewPin);
            mButtonChangePin2->SetText(String(""));
            // if the new pin is not valid, display
            // a message and reset the state.
            if (ValidatePin(mNewPin, FALSE)) {
                mPinChangeState = PIN_CHANGE_REENTER_PIN_FOR_PUK;
                DisplayPinChangeDialog();
            }
            else {
                DisplayPinChangeDialog(R.string.invalidPin2, TRUE);
            }
            break;
        }
        case PIN_CHANGE_REENTER_PIN_FOR_PUK:
        {
            String text;
            mButtonChangePin2->GetText(&text);
            // if the re-entered pin is not valid, display
            // a message and reset the state.
            if (!mNewPin.Equals(text)) {
                mPinChangeState = PIN_CHANGE_NEW_PIN_FOR_PUK;
                mButtonChangePin2->SetText(String(""));
                DisplayPinChangeDialog(R.string.mismatchPin2, TRUE);
            }
            else {
                // Both puk2 and new pin2 are ready to submit
                mButtonChangePin2->SetText(String(""));
                AutoPtr<IMessage> onComplete;
                mFDNHandler->ObtainMessage(
                        EVENT_PIN2_CHANGE_COMPLETE, (IMessage**)&onComplete);
                AutoPtr<IIccCard> card;
                mPhone->GetIccCard((IIccCard**)&card);
                card->SupplyPuk2(mPuk2, mNewPin, onComplete);
            }
            break;
        }
    }
}

ECode CFdnSetting::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    // set the state of the preference and then display the dialog.
    ResetPinChangeStateForPUK2();
    return DisplayPinChangeDialog(0, TRUE);
}

void CFdnSetting::DisplayMessage(
    /* [in] */ Int 32strId,
    /* [in] */ Int 32attemptsRemaining)
{
    String s;
    GetString(strId, &s);
    if ((strId == R.string.badPin2) || (strId == R.string.badPuk2) ||
            (strId == R.string.pin2_invalid)) {
        if (attemptsRemaining >= 0) {
            StringBuilder sb;
            String str;
            GetString(strId, &str);
            sb += str;
            GetString(R.string.pin2_attempts, attemptsRemaining, &str);
            sb += str;
            s = sb.ToString();
        }
        else {
            GetString(strId, &s);
        }
    }
    StringBuilder sb;
    sb += "displayMessage: attemptsRemaining=";
    sb += attemptsRemaining;
    sb += " s=";
    sb += s;
    Log(sb.ToString());

    AutoPtr<IToastHelper> helper;
    CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    AutoPtr<IToast> toast;
    helper->MakeText(this, s, IToast::LENGTH_SHORT, (IToast**)&toast);
    toast->Show();
}

void CFdnSetting::DisplayMessage(
    /* [in] */ Int 32strId)
{
    DisplayMessage(strId, -1);
}

void CFdnSetting::DisplayPinChangeDialog()
{
    DisplayPinChangeDialog(0, TRUE);
}

void CFdnSetting::DisplayPinChangeDialog(
    /* [in] */ Int32 strId,
    /* [in] */ Boolean shouldDisplay)
{
    Int32 msgId;
    switch (mPinChangeState) {
        case PIN_CHANGE_OLD:
            msgId = R.string.oldPin2Label;
            break;
        case PIN_CHANGE_NEW:
        case PIN_CHANGE_NEW_PIN_FOR_PUK:
            msgId = R.string.newPin2Label;
            break;
        case PIN_CHANGE_REENTER:
        case PIN_CHANGE_REENTER_PIN_FOR_PUK:
            msgId = R.string.confirmPin2Label;
            break;
        case PIN_CHANGE_PUK:
        default:
            msgId = R.string.label_puk2_code;
            break;
    }

    // append the note / additional message, if needed.
    if (strId != 0) {
        StringBuilder sb;
        String str;
        GetText(msgId, &str);
        sb += str;
        sb += "\n";
        GetText(strId, &str);
        sb += str;
        mButtonChangePin2->SetDialogMessage(sb.ToString());
    }
    else {
        mButtonChangePin2->SetDialogMessage(msgId);
    }

    // only display if requested.
    if (shouldDisplay) {
        mButtonChangePin2->ShowPinDialog();
    }
}

void CFdnSetting::ResetPinChangeState()
{
    if (DBG) Log(String("resetPinChangeState"));
    mPinChangeState = PIN_CHANGE_OLD;
    DisplayPinChangeDialog(0, FALSE);
    mOldPin = String("");
    mNewPin = String("");
    mIsPuk2Locked = FALSE;
}

void CFdnSetting::ResetPinChangeStateForPUK2()
{
    if (DBG) Log(String("resetPinChangeStateForPUK2"));
    mPinChangeState = PIN_CHANGE_PUK;
    DisplayPinChangeDialog(0, FALSE);
    mOldPin = String("");
    mNewPin = String("");
    mPuk2 = String("");
    mIsPuk2Locked = TRUE;
}

Boolean CFdnSetting::ValidatePin(
    /* [in] */ const String& pin,
    /* [in] */ Boolean isPuk)
{
    // for pin, we have 4-8 numbers, or puk, we use only 8.
    Int32 pinMinimum = isPuk ? MAX_PIN_LENGTH : MIN_PIN_LENGTH;

    // check validity
    if (pin.IsNull() || pin.GetLength() < pinMinimum || pin.GetLength() > MAX_PIN_LENGTH) {
        return FALSE;
    } else {
        return TRUE;
    }
}

void CFdnSetting::UpdateEnableFDN()
{
    AutoPtr<IIccCard> card;
    mPhone->GetIccCard((IIccCard**)&card);
    Boolean enable;
    if (card->GetIccFdnEnabled(&enable), enable) {
        mButtonEnableFDN->SetTitle(R.string.enable_fdn_ok);
        mButtonEnableFDN->SetSummary(R.string.fdn_enabled);
        mButtonEnableFDN->SetDialogTitle(R.string.disable_fdn);
    }
    else {
        mButtonEnableFDN->SetTitle(R.string.disable_fdn_ok);
        mButtonEnableFDN->SetSummary(R.string.fdn_disabled);
        mButtonEnableFDN->SetDialogTitle(R.string.enable_fdn);
    }
}

Ecode CFdnSetting::OnCreate(
    /* [in] */ IBundle* icicle)
{
    PreferenceActivity::OnCreate(icicle);

    AddPreferencesFromResource(R.xml.fdn_setting);

    PhoneGlobals::GetPhone((IPhone**)&mPhone);

    //get UI object references
    AutoPtr<IPreferenceScreen> prefSet;
    GetPreferenceScreen((IPreferenceScreen**)&prefSet);

    AutoPtr<IPreference> preference;
    prefSet->FindPreference(BUTTON_FDN_ENABLE_KEY, (IPreference**)&preference);
    mButtonEnableFDN = IEditPinPreference::Probe(preference);

    AutoPtr<IPreference> preference2;
    prefSet->FindPreference(BUTTON_CHANGE_PIN2_KEY, (IPreference**)&preference2);
    mButtonChangePin2 = IEditPinPreference::Probe(preference2);

    //assign click listener and update state
    mButtonEnableFDN->SetOnPinEnteredListener(this);
    UpdateEnableFDN();

    mButtonChangePin2->SetOnPinEnteredListener(this);

    // Only reset the pin change dialog if we're not in the middle of changing it.
    if (icicle == NULL) {
        ResetPinChangeState();
    }
    else {
        icicle->GetBoolean(SKIP_OLD_PIN_KEY, &mIsPuk2Locked);
        icicle->GetInt32(PIN_CHANGE_STATE_KEY, &mPinChangeState);
        icicle->GetString(OLD_PIN_KEY, &mOldPin);
        icicle->GetString(NEW_PIN_KEY, &mNewPin);

        String str;
        icicle->GetString(DIALOG_MESSAGE_KEY, &str);
        mButtonChangePin2->SetDialogMessage(str);

        String str2;
        icicle->GetString(DIALOG_PIN_ENTRY_KEY, &str2);
        mButtonChangePin2->SetText(str2);
    }

    AutoPtr<IActionBar> actionBar;
    GetActionBar((IActionBar**)&actionBar);
    if (actionBar != NULL) {
        // android.R.id.home will be triggered in onOptionsItemSelected()
        actionBar->SetDisplayHomeAsUpEnabled(TRUE);
    }
    return NOERROR;
}

Ecode CFdnSetting::OnResume()
{
    PreferenceActivity::OnResume();
    PhoneGlobals::GetPhone((IPhone**)&mPhone);
    UpdateEnableFDN();
    return NOERROR;
}

Ecode CFdnSetting::OnSaveInstanceState(
    /* [in] */ IBundle* _out)
{
    PreferenceActivity::OnSaveInstanceState(_out);
    _out->PutBoolean(SKIP_OLD_PIN_KEY, mIsPuk2Locked);
    _out->PutInt32(PIN_CHANGE_STATE_KEY, mPinChangeState);
    _out->PutString(OLD_PIN_KEY, mOldPin);
    _out->PutString(NEW_PIN_KEY, mNewPin);

    AutoPtr<IDialogMessage> dialogMessage;
    mButtonChangePin2->GetDialogMessage((IDialogMessage**)&dialogMessage);
    String str;
    dialogMessage->ToString(&str);
    _out->PutString(DIALOG_MESSAGE_KEY, str);

    String str2;
    mButtonChangePin2->GetText(&str2);
    return _out->PutString(DIALOG_PIN_ENTRY_KEY, str2);
}

Ecode CFdnSetting::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 itemId;
    item->GetItemId(&itemId);
    if (itemId == android.R.id.home) {  // See ActionBar#setDisplayHomeAsUpEnabled()
        CallFeaturesSetting::GoUpToTopLevelSetting(this);
        *result = TRUE;
        return NOERROR;
    }
    return PreferenceActivity::OnOptionsItemSelected(item, result);
}

void CFdnSetting::Log(
    /* [in] */ const String& msg)
{
    StringBuilder sb;
    sb += "FdnSetting: ";
    sb += msg;
    Logger::D(LOG_TAG, sb.ToString());
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos