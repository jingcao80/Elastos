

package com.android.settings.deviceinfo;

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::IUtils;

/**
 * USB storage settings.
 */
public class UsbSettings extends SettingsPreferenceFragment {

    private static const String TAG = "UsbSettings";

    private static const String KEY_MTP = "usb_mtp";
    private static const String KEY_PTP = "usb_ptp";

    private UsbManager mUsbManager;
    private CheckBoxPreference mMtp;
    private CheckBoxPreference mPtp;
    private Boolean mUsbAccessoryMode;

    private final BroadcastReceiver mStateReceiver = new BroadcastReceiver() {
        CARAPI OnReceive(Context content, Intent intent) {
            String action = intent->GetAction();
            if (action->Equals(UsbManager.ACTION_USB_STATE)) {
               mUsbAccessoryMode = intent->GetBooleanExtra(UsbManager.USB_FUNCTION_ACCESSORY, FALSE);
               Logger::E(TAG, "UsbAccessoryMode " + mUsbAccessoryMode);
            }
            UpdateToggles(mUsbManager->GetDefaultFunction());
        }
    };

    private PreferenceScreen CreatePreferenceHierarchy() {
        PreferenceScreen root = GetPreferenceScreen();
        if (root != NULL) {
            root->RemoveAll();
        }
        AddPreferencesFromResource(R.xml.usb_settings);
        root = GetPreferenceScreen();

        mMtp = (CheckBoxPreference)root->FindPreference(KEY_MTP);
        mPtp = (CheckBoxPreference)root->FindPreference(KEY_PTP);

        UserManager um = (UserManager) GetActivity()->GetSystemService(Context.USER_SERVICE);
        if (um->HasUserRestriction(UserManager.DISALLOW_USB_FILE_TRANSFER)) {
            mMtp->SetEnabled(FALSE);
            mPtp->SetEnabled(FALSE);
        }

        return root;
    }

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);
        mUsbManager = (UsbManager)GetSystemService(Context.USB_SERVICE);
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        GetActivity()->UnregisterReceiver(mStateReceiver);
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        // Make sure we reload the preference hierarchy since some of these settings
        // depend on others...
        CreatePreferenceHierarchy();

        // ACTION_USB_STATE is sticky so this will call updateToggles
        GetActivity()->RegisterReceiver(mStateReceiver,
                new IntentFilter(UsbManager.ACTION_USB_STATE));
    }

    private void UpdateToggles(String function) {
        if (UsbManager.USB_FUNCTION_MTP->Equals(function)) {
            mMtp->SetChecked(TRUE);
            mPtp->SetChecked(FALSE);
        } else if (UsbManager.USB_FUNCTION_PTP->Equals(function)) {
            mMtp->SetChecked(FALSE);
            mPtp->SetChecked(TRUE);
        } else  {
            mMtp->SetChecked(FALSE);
            mPtp->SetChecked(FALSE);
        }
        UserManager um = (UserManager) GetActivity()->GetSystemService(Context.USER_SERVICE);
        if (um->HasUserRestriction(UserManager.DISALLOW_USB_FILE_TRANSFER)) {
            Logger::E(TAG, "USB is locked down");
            mMtp->SetEnabled(FALSE);
            mPtp->SetEnabled(FALSE);
        } else if (!mUsbAccessoryMode) {
            //Enable MTP and PTP switch while USB is not in Accessory Mode, otherwise disable it
            Logger::E(TAG, "USB Normal Mode");
            mMtp->SetEnabled(TRUE);
            mPtp->SetEnabled(TRUE);
        } else {
            Logger::E(TAG, "USB Accessory Mode");
            mMtp->SetEnabled(FALSE);
            mPtp->SetEnabled(FALSE);
        }
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {

        // Don't allow any changes to take effect as the USB host will be disconnected, killing
        // the monkeys
        if (Utils->IsMonkeyRunning()) {
            return TRUE;
        }
        // If this user is disallowed from using USB, don't handle their attempts to change the
        // setting.
        UserManager um = (UserManager) GetActivity()->GetSystemService(Context.USER_SERVICE);
        if (um->HasUserRestriction(UserManager.DISALLOW_USB_FILE_TRANSFER)) {
            return TRUE;
        }

        String function = "none";
        if (preference == mMtp && mMtp->IsChecked()) {
            function = UsbManager.USB_FUNCTION_MTP;
        } else if (preference == mPtp && mPtp->IsChecked()) {
            function = UsbManager.USB_FUNCTION_PTP;
        }

        mUsbManager->SetCurrentFunction(function, TRUE);
        UpdateToggles(function);

        return TRUE;
    }
}
