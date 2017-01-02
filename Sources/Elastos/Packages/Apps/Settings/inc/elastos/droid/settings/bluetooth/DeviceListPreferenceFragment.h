//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

package com.android.settings.bluetooth;

using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Settings::IRestrictedSettingsFragment;

using Elastos::Utility::ICollection;
using Elastos::Utility::IWeakHashMap;

/**
 * Parent class for settings fragments that contain a list of Bluetooth
 * devices.
 *
 * @see BluetoothSettings
 * @see DevicePickerFragment
 */
public abstract class DeviceListPreferenceFragment extends
        RestrictedSettingsFragment implements BluetoothCallback {

    private static const String TAG = "DeviceListPreferenceFragment";

    private static const String KEY_BT_DEVICE_LIST = "bt_device_list";
    private static const String KEY_BT_SCAN = "bt_scan";

    private BluetoothDeviceFilter.Filter mFilter;

    BluetoothDevice mSelectedDevice;

    LocalBluetoothAdapter mLocalAdapter;
    LocalBluetoothManager mLocalManager;

    private PreferenceGroup mDeviceListGroup;

    final WeakHashMap<CachedBluetoothDevice, BluetoothDevicePreference> mDevicePreferenceMap =
            new WeakHashMap<CachedBluetoothDevice, BluetoothDevicePreference>();

    DeviceListPreferenceFragment(String restrictedKey) {
        Super(restrictedKey);
        mFilter = BluetoothDeviceFilter.ALL_FILTER;
    }

    final void SetFilter(BluetoothDeviceFilter.Filter filter) {
        mFilter = filter;
    }

    final void SetFilter(Int32 filterType) {
        mFilter = BluetoothDeviceFilter->GetFilter(filterType);
    }

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        mLocalManager = LocalBluetoothManager->GetInstance(GetActivity());
        if (mLocalManager == NULL) {
            Logger::E(TAG, "Bluetooth is not supported on this device");
            return;
        }
        mLocalAdapter = mLocalManager->GetBluetoothAdapter();

        AddPreferencesForActivity();

        mDeviceListGroup = (PreferenceCategory) FindPreference(KEY_BT_DEVICE_LIST);
    }

    void SetDeviceListGroup(PreferenceGroup preferenceGroup) {
        mDeviceListGroup = preferenceGroup;
    }

    /** Add preferences from the subclass. */
    abstract void AddPreferencesForActivity();

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        if (mLocalManager == NULL || IsUiRestricted()) return;

        mLocalManager->SetForegroundActivity(GetActivity());
        mLocalManager->GetEventManager()->RegisterCallback(this);

        UpdateProgressUi(mLocalAdapter->IsDiscovering());
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        if (mLocalManager == NULL || IsUiRestricted()) {
            return;
        }

        RemoveAllDevices();
        mLocalManager->SetForegroundActivity(NULL);
        mLocalManager->GetEventManager()->UnregisterCallback(this);
    }

    void RemoveAllDevices() {
        mLocalAdapter->StopScanning();
        mDevicePreferenceMap->Clear();
        mDeviceListGroup->RemoveAll();
    }

    void AddCachedDevices() {
        Collection<CachedBluetoothDevice> cachedDevices =
                mLocalManager->GetCachedDeviceManager()->GetCachedDevicesCopy();
        for (CachedBluetoothDevice cachedDevice : cachedDevices) {
            OnDeviceAdded(cachedDevice);
        }
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferenceScreen,
            Preference preference) {
        if (KEY_BT_SCAN->Equals(preference->GetKey())) {
            mLocalAdapter->StartScanning(TRUE);
            return TRUE;
        }

        if (preference instanceof BluetoothDevicePreference) {
            BluetoothDevicePreference btPreference = (BluetoothDevicePreference) preference;
            CachedBluetoothDevice device = btPreference->GetCachedDevice();
            mSelectedDevice = device->GetDevice();
            OnDevicePreferenceClick(btPreference);
            return TRUE;
        }

        return super->OnPreferenceTreeClick(preferenceScreen, preference);
    }

    void OnDevicePreferenceClick(BluetoothDevicePreference btPreference) {
        btPreference->OnClicked();
    }

    CARAPI OnDeviceAdded(CachedBluetoothDevice cachedDevice) {
        if (mDevicePreferenceMap->Get(cachedDevice) != NULL) {
            return;
        }

        // Prevent updates while the list shows one of the state messages
        if (mLocalAdapter->GetBluetoothState() != BluetoothAdapter.STATE_ON) return;

        if (mFilter->Matches(cachedDevice->GetDevice())) {
            CreateDevicePreference(cachedDevice);
        }
     }

    void CreateDevicePreference(CachedBluetoothDevice cachedDevice) {
        if (mDeviceListGroup == NULL) {
            Logger::W(TAG, "Trying to create a device preference before the list group/category "
                    + "exists!");
            return;
        }

        BluetoothDevicePreference preference = new BluetoothDevicePreference(
                GetActivity(), cachedDevice);

        InitDevicePreference(preference);
        mDeviceListGroup->AddPreference(preference);
        mDevicePreferenceMap->Put(cachedDevice, preference);
    }

    /**
     * Overridden in {@link BluetoothSettings} to add a listener.
     * @param preference the newly added preference
     */
    void InitDevicePreference(BluetoothDevicePreference preference) {
        // Does nothing by default
    }

    CARAPI OnDeviceDeleted(CachedBluetoothDevice cachedDevice) {
        BluetoothDevicePreference preference = mDevicePreferenceMap->Remove(cachedDevice);
        if (preference != NULL) {
            mDeviceListGroup->RemovePreference(preference);
        }
    }

    CARAPI OnScanningStateChanged(Boolean started) {
        UpdateProgressUi(started);
    }

    private void UpdateProgressUi(Boolean start) {
        if (mDeviceListGroup instanceof BluetoothProgressCategory) {
            ((BluetoothProgressCategory) mDeviceListGroup).SetProgress(start);
        }
    }

    CARAPI OnBluetoothStateChanged(Int32 bluetoothState) {
        if (bluetoothState == BluetoothAdapter.STATE_OFF) {
            UpdateProgressUi(FALSE);
        }
    }
}
