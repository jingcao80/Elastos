

package com.android.settings;

using Elastos::Droid::Internal::Widget::ILockPatternUtils;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IBatteryManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IButton;

public class CryptKeeperSettings extends Fragment {
    private static const String TAG = "CryptKeeper";

    private static const Int32 KEYGUARD_REQUEST = 55;

    // Minimum battery charge level (in percent) to launch encryption.  If the battery charge is
    // lower than this, encryption should not be activated.
    private static const Int32 MIN_BATTERY_LEVEL = 80;

    private View mContentView;
    private Button mInitiateButton;
    private View mPowerWarning;
    private View mBatteryWarning;
    private IntentFilter mIntentFilter;

    private BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            if (action->Equals(IIntent::ACTION_BATTERY_CHANGED)) {
                final Int32 level = intent->GetIntExtra(BatteryManager.EXTRA_LEVEL, 0);
                final Int32 plugged = intent->GetIntExtra(BatteryManager.EXTRA_PLUGGED, 0);
                final Int32 invalidCharger = intent->GetIntExtra(
                    BatteryManager.EXTRA_INVALID_CHARGER, 0);

                final Boolean levelOk = level >= MIN_BATTERY_LEVEL;
                final Boolean pluggedOk =
                    ((plugged & BatteryManager.BATTERY_PLUGGED_ANY) != 0) &&
                     invalidCharger == 0;

                // Update UI elements based on power/battery status
                mInitiateButton->SetEnabled(levelOk && pluggedOk);
                mPowerWarning->SetVisibility(pluggedOk ? View.GONE : View.VISIBLE );
                mBatteryWarning->SetVisibility(levelOk ? View.GONE : View.VISIBLE);
            }
        }
    };

    /**
     * If the user clicks to begin the reset sequence, we next require a
     * keyguard confirmation if the user has currently enabled one.  If there
     * is no keyguard available, we prompt the user to set a password.
     */
    private Button.OnClickListener mInitiateListener = new Button->OnClickListener() {

        CARAPI OnClick(View v) {
            if (!RunKeyguardConfirmation(KEYGUARD_REQUEST)) {
                // TODO replace (or follow) this dialog with an explicit launch into password UI
                new AlertDialog->Builder(GetActivity())
                    .SetTitle(R::string::crypt_keeper_dialog_need_password_title)
                    .SetMessage(R::string::crypt_keeper_dialog_need_password_message)
                    .SetPositiveButton(android.R::string::ok, NULL)
                    .Create()
                    .Show();
            }
        }
    };

    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedState) {
        mContentView = inflater->Inflate(R.layout.crypt_keeper_settings, NULL);

        mIntentFilter = new IntentFilter();
        mIntentFilter->AddAction(IIntent::ACTION_BATTERY_CHANGED);

        mInitiateButton = (Button) mContentView->FindViewById(R.id.initiate_encrypt);
        mInitiateButton->SetOnClickListener(mInitiateListener);
        mInitiateButton->SetEnabled(FALSE);

        mPowerWarning = mContentView->FindViewById(R.id.warning_unplugged);
        mBatteryWarning = mContentView->FindViewById(R.id.warning_low_charge);

        return mContentView;
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        GetActivity()->RegisterReceiver(mIntentReceiver, mIntentFilter);
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        GetActivity()->UnregisterReceiver(mIntentReceiver);
    }

    /**
     * If encryption is already started, and this launched via a "start encryption" intent,
     * then exit immediately - it's already up and running, so there's no point in "starting" it.
     */
    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);
        Activity activity = GetActivity();
        Intent intent = activity->GetIntent();
        if (DevicePolicyManager.ACTION_START_ENCRYPTION->Equals(intent->GetAction())) {
            DevicePolicyManager dpm = (DevicePolicyManager)
                    activity->GetSystemService(Context.DEVICE_POLICY_SERVICE);
            if (dpm != NULL) {
                Int32 status = dpm->GetStorageEncryptionStatus();
                if (status != DevicePolicyManager.ENCRYPTION_STATUS_INACTIVE) {
                    // There is nothing to do here, so simply Finish() (which returns to caller)
                    activity->Finish();
                }
            }
        }
    }

    /**
     * Keyguard validation is run using the standard {@link ConfirmLockPattern}
     * component as a subactivity
     * @param request the request code to be returned once confirmation finishes
     * @return TRUE if confirmation launched
     */
    private Boolean RunKeyguardConfirmation(Int32 request) {
        Resources res = GetActivity()->GetResources();
        ChooseLockSettingsHelper helper = new ChooseLockSettingsHelper(GetActivity(), this);

        if (helper->Utils()->GetKeyguardStoredPasswordQuality()
                == DevicePolicyManager.PASSWORD_QUALITY_UNSPECIFIED) {
            ShowFinalConfirmation(StorageManager.CRYPT_TYPE_DEFAULT, "");
            return TRUE;
        }

        return helper->LaunchConfirmationActivity(request,
                res->GetText(R::string::master_clear_gesture_prompt),
                res->GetText(R::string::crypt_keeper_confirm_encrypt),
                TRUE);
    }

    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        super->OnActivityResult(requestCode, resultCode, data);

        if (requestCode != KEYGUARD_REQUEST) {
            return;
        }

        // If the user entered a valid keyguard trace, present the final
        // confirmation prompt; otherwise, go back to the initial state.
        if (resultCode == Activity.RESULT_OK && data != NULL) {
            Int32 type = data->GetIntExtra(ChooseLockSettingsHelper.EXTRA_KEY_TYPE, -1);
            String password = data->GetStringExtra(ChooseLockSettingsHelper.EXTRA_KEY_PASSWORD);
            if (!TextUtils->IsEmpty(password)) {
                ShowFinalConfirmation(type, password);
            }
        }
    }

    private void ShowFinalConfirmation(Int32 type, String password) {
        Preference preference = new Preference(GetActivity());
        preference->SetFragment(CryptKeeperConfirm.class->GetName());
        preference->SetTitle(R::string::crypt_keeper_confirm_title);
        preference->GetExtras()->PutInt("type", type);
        preference->GetExtras()->PutString("password", password);
        ((SettingsActivity) GetActivity()).OnPreferenceStartFragment(NULL, preference);
    }
}
