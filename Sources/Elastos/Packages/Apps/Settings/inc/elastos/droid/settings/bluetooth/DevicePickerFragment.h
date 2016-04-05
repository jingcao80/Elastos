

package com.android.settings.bluetooth;

using static android::Os::UserManager::IDISALLOW_CONFIG_BLUETOOTH;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Bluetooth::IBluetoothDevicePicker;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;

using Elastos::Droid::Settings::IR;

/**
 * BluetoothSettings is the Settings screen for Bluetooth configuration and
 * connection management.
 */
public class DevicePickerFragment extends DeviceListPreferenceFragment {
    private static const Int32 MENU_ID_REFRESH = Menu.FIRST;

    public DevicePickerFragment() {
        Super(NULL /* Not tied to any user restrictions. */);
    }

    private Boolean mNeedAuth;
    private String mLaunchPackage;
    private String mLaunchClass;
    private Boolean mStartScanOnResume;

    //@Override
    void AddPreferencesForActivity() {
        AddPreferencesFromResource(R.xml.device_picker);

        Intent intent = GetActivity()->GetIntent();
        mNeedAuth = intent->GetBooleanExtra(BluetoothDevicePicker.EXTRA_NEED_AUTH, FALSE);
        SetFilter(intent->GetIntExtra(BluetoothDevicePicker.EXTRA_FILTER_TYPE,
                BluetoothDevicePicker.FILTER_TYPE_ALL));
        mLaunchPackage = intent->GetStringExtra(BluetoothDevicePicker.EXTRA_LAUNCH_PACKAGE);
        mLaunchClass = intent->GetStringExtra(BluetoothDevicePicker.EXTRA_LAUNCH_CLASS);
    }

    //@Override
    void InitDevicePreference(BluetoothDevicePreference preference) {
        preference->SetWidgetLayoutResource(R.layout.preference_empty_list);
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        menu->Add(Menu.NONE, MENU_ID_REFRESH, 0, R::string::bluetooth_search_for_devices)
                .SetEnabled(TRUE)
                .SetShowAsAction(MenuItem.SHOW_AS_ACTION_NEVER);
        super->OnCreateOptionsMenu(menu, inflater);
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        switch (item->GetItemId()) {
            case MENU_ID_REFRESH:
                mLocalAdapter->StartScanning(TRUE);
                return TRUE;
        }
        return super->OnOptionsItemSelected(item);
    }

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        GetActivity()->SetTitle(GetString(R::string::device_picker));
        UserManager um = (UserManager) GetSystemService(Context.USER_SERVICE);
        mStartScanOnResume = !um->HasUserRestriction(DISALLOW_CONFIG_BLUETOOTH)
                && (savedInstanceState == NULL);  // don't start scan after rotation
        SetHasOptionsMenu(TRUE);
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        AddCachedDevices();
        if (mStartScanOnResume) {
            mLocalAdapter->StartScanning(TRUE);
            mStartScanOnResume = FALSE;
        }
    }

    //@Override
    void OnDevicePreferenceClick(BluetoothDevicePreference btPreference) {
        mLocalAdapter->StopScanning();
        LocalBluetoothPreferences->PersistSelectedDeviceInPicker(
                GetActivity(), mSelectedDevice->GetAddress());
        if ((btPreference->GetCachedDevice()->GetBondState() ==
                BluetoothDevice.BOND_BONDED) || !mNeedAuth) {
            SendDevicePickedIntent(mSelectedDevice);
            Finish();
        } else {
            super->OnDevicePreferenceClick(btPreference);
        }
    }

    CARAPI OnDeviceBondStateChanged(CachedBluetoothDevice cachedDevice,
            Int32 bondState) {
        if (bondState == BluetoothDevice.BOND_BONDED) {
            BluetoothDevice device = cachedDevice->GetDevice();
            if (device->Equals(mSelectedDevice)) {
                SendDevicePickedIntent(device);
                Finish();
            }
        }
    }

    //@Override
    CARAPI OnBluetoothStateChanged(Int32 bluetoothState) {
        super->OnBluetoothStateChanged(bluetoothState);

        if (bluetoothState == BluetoothAdapter.STATE_ON) {
            mLocalAdapter->StartScanning(FALSE);
        }
    }

    private void SendDevicePickedIntent(BluetoothDevice device) {
        Intent intent = new Intent(BluetoothDevicePicker.ACTION_DEVICE_SELECTED);
        intent->PutExtra(BluetoothDevice.EXTRA_DEVICE, device);
        if (mLaunchPackage != NULL && mLaunchClass != NULL) {
            intent->SetClassName(mLaunchPackage, mLaunchClass);
        }
        GetActivity()->SendBroadcast(intent);
    }
}
