

package com.android.settings;

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceActivity;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;

/**
 * Implements the preference screen to enable/disable ICC lock and
 * also the dialogs to change the ICC PIN. In the former case, enabling/disabling
 * the ICC lock will prompt the user for the current PIN.
 * In the Change PIN case, it prompts the user for old pin, new pin and new pin
 * again before attempting to change it. Calls the SimCard interface to execute
 * these operations.
 *
 */
public class IccLockSettings extends PreferenceActivity
        implements EditPinPreference.OnPinEnteredListener {
    private static const String TAG = "IccLockSettings";
    private static const Boolean DBG = TRUE;

    private static const Int32 OFF_MODE = 0;
    // State when enabling/disabling ICC lock
    private static const Int32 ICC_LOCK_MODE = 1;
    // State when entering the old pin
    private static const Int32 ICC_OLD_MODE = 2;
    // State when entering the new pin - first time
    private static const Int32 ICC_NEW_MODE = 3;
    // State when entering the new pin - second time
    private static const Int32 ICC_REENTER_MODE = 4;

    // Keys in xml file
    private static const String PIN_DIALOG = "sim_pin";
    private static const String PIN_TOGGLE = "sim_toggle";
    // Keys in icicle
    private static const String DIALOG_STATE = "dialogState";
    private static const String DIALOG_PIN = "dialogPin";
    private static const String DIALOG_ERROR = "dialogError";
    private static const String ENABLE_TO_STATE = "enableState";

    // Save and restore inputted PIN code when configuration changed
    // (ex. portrait<-->landscape) during change PIN code
    private static const String OLD_PINCODE = "oldPinCode";
    private static const String NEW_PINCODE = "newPinCode";

    private static const Int32 MIN_PIN_LENGTH = 4;
    private static const Int32 MAX_PIN_LENGTH = 8;
    // Which dialog to show next when popped up
    private Int32 mDialogState = OFF_MODE;

    private String mPin;
    private String mOldPin;
    private String mNewPin;
    private String mError;
    // Are we trying to enable or disable ICC lock?
    private Boolean mToState;

    private Phone mPhone;

    private EditPinPreference mPinDialog;
    private CheckBoxPreference mPinToggle;

    private Resources mRes;

    // For async handler to identify request type
    private static const Int32 MSG_ENABLE_ICC_PIN_COMPLETE = 100;
    private static const Int32 MSG_CHANGE_ICC_PIN_COMPLETE = 101;
    private static const Int32 MSG_SIM_STATE_CHANGED = 102;

    // For replies from IccCard interface
    private Handler mHandler = new Handler() {
        CARAPI HandleMessage(Message msg) {
            AsyncResult ar = (AsyncResult) msg.obj;
            switch (msg.what) {
                case MSG_ENABLE_ICC_PIN_COMPLETE:
                    IccLockChanged(ar.exception == NULL, msg.arg1);
                    break;
                case MSG_CHANGE_ICC_PIN_COMPLETE:
                    IccPinChanged(ar.exception == NULL, msg.arg1);
                    break;
                case MSG_SIM_STATE_CHANGED:
                    UpdatePreferences();
                    break;
            }

            return;
        }
    };

    private final BroadcastReceiver mSimStateReceiver = new BroadcastReceiver() {
        CARAPI OnReceive(Context context, Intent intent) {
            final String action = intent->GetAction();
            if (TelephonyIntents.ACTION_SIM_STATE_CHANGED->Equals(action)) {
                mHandler->SendMessage(mHandler->ObtainMessage(MSG_SIM_STATE_CHANGED));
            }
        }
    };

    // For top-level settings screen to query
    static Boolean IsIccLockEnabled() {
        return PhoneFactory->GetDefaultPhone()->GetIccCard().GetIccLockEnabled();
    }

    static String GetSummary(Context context) {
        Resources res = context->GetResources();
        String summary = IsIccLockEnabled()
                ? res->GetString(R::string::sim_lock_on)
                : res->GetString(R::string::sim_lock_off);
        return summary;
    }

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        if (Utils->IsMonkeyRunning()) {
            Finish();
            return;
        }

        AddPreferencesFromResource(R.xml.sim_lock_settings);

        mPinDialog = (EditPinPreference) FindPreference(PIN_DIALOG);
        mPinToggle = (CheckBoxPreference) FindPreference(PIN_TOGGLE);
        if (savedInstanceState != NULL && savedInstanceState->ContainsKey(DIALOG_STATE)) {
            mDialogState = savedInstanceState->GetInt(DIALOG_STATE);
            mPin = savedInstanceState->GetString(DIALOG_PIN);
            mError = savedInstanceState->GetString(DIALOG_ERROR);
            mToState = savedInstanceState->GetBoolean(ENABLE_TO_STATE);

            // Restore inputted PIN code
            switch (mDialogState) {
                case ICC_NEW_MODE:
                    mOldPin = savedInstanceState->GetString(OLD_PINCODE);
                    break;

                case ICC_REENTER_MODE:
                    mOldPin = savedInstanceState->GetString(OLD_PINCODE);
                    mNewPin = savedInstanceState->GetString(NEW_PINCODE);
                    break;

                case ICC_LOCK_MODE:
                case ICC_OLD_MODE:
                default:
                    break;
            }
        }

        mPinDialog->SetOnPinEnteredListener(this);

        // Don't need any changes to be remembered
        GetPreferenceScreen()->SetPersistent(FALSE);

        mPhone = PhoneFactory->GetDefaultPhone();
        mRes = GetResources();
        UpdatePreferences();
    }

    private void UpdatePreferences() {
        mPinToggle->SetChecked(mPhone->GetIccCard()->GetIccLockEnabled());
    }

    //@Override
    protected void OnResume() {
        super->OnResume();

        // ACTION_SIM_STATE_CHANGED is sticky, so we'll receive current state after this call,
        // which will call UpdatePreferences().
        final IntentFilter filter = new IntentFilter(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        RegisterReceiver(mSimStateReceiver, filter);

        if (mDialogState != OFF_MODE) {
            ShowPinDialog();
        } else {
            // Prep for standard click on "Change PIN"
            ResetDialogState();
        }
    }

    //@Override
    protected void OnPause() {
        super->OnPause();
        UnregisterReceiver(mSimStateReceiver);
    }

    //@Override
    protected void OnSaveInstanceState(Bundle out) {
        // Need to store this state for slider open/close
        // There is one case where the dialog is popped up by the preference
        // framework. In that case, let the preference framework store the
        // dialog state. In other cases, where this activity manually launches
        // the dialog, store the state of the dialog.
        if (mPinDialog->IsDialogOpen()) {
            out->PutInt(DIALOG_STATE, mDialogState);
            out->PutString(DIALOG_PIN, mPinDialog->GetEditText()->GetText().ToString());
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
        } else {
            super->OnSaveInstanceState(out);
        }
    }

    private void ShowPinDialog() {
        if (mDialogState == OFF_MODE) {
            return;
        }
        SetDialogValues();

        mPinDialog->ShowPinDialog();
    }

    private void SetDialogValues() {
        mPinDialog->SetText(mPin);
        String message = "";
        switch (mDialogState) {
            case ICC_LOCK_MODE:
                message = mRes->GetString(R::string::sim_enter_pin);
                mPinDialog->SetDialogTitle(mToState
                        ? mRes->GetString(R::string::sim_enable_sim_lock)
                        : mRes->GetString(R::string::sim_disable_sim_lock));
                break;
            case ICC_OLD_MODE:
                message = mRes->GetString(R::string::sim_enter_old);
                mPinDialog->SetDialogTitle(mRes->GetString(R::string::sim_change_pin));
                break;
            case ICC_NEW_MODE:
                message = mRes->GetString(R::string::sim_enter_new);
                mPinDialog->SetDialogTitle(mRes->GetString(R::string::sim_change_pin));
                break;
            case ICC_REENTER_MODE:
                message = mRes->GetString(R::string::sim_reenter_new);
                mPinDialog->SetDialogTitle(mRes->GetString(R::string::sim_change_pin));
                break;
        }
        if (mError != NULL) {
            message = mError + "\n" + message;
            mError = NULL;
        }
        mPinDialog->SetDialogMessage(message);
    }

    CARAPI OnPinEntered(EditPinPreference preference, Boolean positiveResult) {
        if (!positiveResult) {
            ResetDialogState();
            return;
        }

        mPin = preference->GetText();
        if (!ReasonablePin(mPin)) {
            // inject error message and display dialog again
            mError = mRes->GetString(R::string::sim_bad_pin);
            ShowPinDialog();
            return;
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
                if (!mPin->Equals(mNewPin)) {
                    mError = mRes->GetString(R::string::sim_pins_dont_match);
                    mDialogState = ICC_NEW_MODE;
                    mPin = NULL;
                    ShowPinDialog();
                } else {
                    mError = NULL;
                    TryChangePin();
                }
                break;
        }
    }

    public Boolean OnPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        if (preference == mPinToggle) {
            // Get the new, preferred state
            mToState = mPinToggle->IsChecked();
            // Flip it back and pop up pin dialog
            mPinToggle->SetChecked(!mToState);
            mDialogState = ICC_LOCK_MODE;
            ShowPinDialog();
        } else if (preference == mPinDialog) {
            mDialogState = ICC_OLD_MODE;
            return FALSE;
        }
        return TRUE;
    }

    private void TryChangeIccLockState() {
        // Try to change icc lock. If it succeeds, toggle the lock state and
        // reset dialog state. Else inject error message and show dialog again.
        Message callback = Message->Obtain(mHandler, MSG_ENABLE_ICC_PIN_COMPLETE);
        mPhone->GetIccCard()->SetIccLockEnabled(mToState, mPin, callback);
        // Disable the setting till the response is received.
        mPinToggle->SetEnabled(FALSE);
    }

    private void IccLockChanged(Boolean success, Int32 attemptsRemaining) {
        if (success) {
            mPinToggle->SetChecked(mToState);
        } else {
            Toast->MakeText(this, GetPinPasswordErrorMessage(attemptsRemaining), Toast.LENGTH_LONG)
                    .Show();
        }
        mPinToggle->SetEnabled(TRUE);
        ResetDialogState();
    }

    private void IccPinChanged(Boolean success, Int32 attemptsRemaining) {
        if (!success) {
            Toast->MakeText(this, GetPinPasswordErrorMessage(attemptsRemaining),
                    Toast.LENGTH_LONG)
                    .Show();
        } else {
            Toast->MakeText(this, mRes->GetString(R::string::sim_change_succeeded),
                    Toast.LENGTH_SHORT)
                    .Show();

        }
        ResetDialogState();
    }

    private void TryChangePin() {
        Message callback = Message->Obtain(mHandler, MSG_CHANGE_ICC_PIN_COMPLETE);
        mPhone->GetIccCard()->ChangeIccLockPassword(mOldPin,
                mNewPin, callback);
    }

    private String GetPinPasswordErrorMessage(Int32 attemptsRemaining) {
        String displayMessage;

        if (attemptsRemaining == 0) {
            displayMessage = mRes->GetString(R::string::wrong_pin_code_pukked);
        } else if (attemptsRemaining > 0) {
            displayMessage = mRes
                    .GetQuantityString(R.plurals.wrong_pin_code, attemptsRemaining,
                            attemptsRemaining);
        } else {
            displayMessage = mRes->GetString(R::string::pin_failed);
        }
        if (DBG) Logger::D(TAG, "getPinPasswordErrorMessage:"
                + " attemptsRemaining=" + attemptsRemaining + " displayMessage=" + displayMessage);
        return displayMessage;
    }

    private Boolean ReasonablePin(String pin) {
        if (pin == NULL || pin->Length() < MIN_PIN_LENGTH || pin->Length() > MAX_PIN_LENGTH) {
            return FALSE;
        } else {
            return TRUE;
        }
    }

    private void ResetDialogState() {
        mError = NULL;
        mDialogState = ICC_OLD_MODE; // Default for when Change PIN is clicked
        mPin = "";
        SetDialogValues();
        mDialogState = OFF_MODE;
    }
}
