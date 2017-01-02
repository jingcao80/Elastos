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

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IEditTextPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Text::IHtml;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Text::IEditable;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::Search::IIndex;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;

using Elastos::Utility::IHashMap;

/**
 * This preference fragment presents the user with all of the profiles
 * for a particular device, and allows them to be individually connected
 * (or disconnected).
 */
public class DeviceProfilesSettings extends SettingsPreferenceFragment
        implements CachedBluetoothDevice.Callback, Preference.OnPreferenceChangeListener {
    private static const String TAG = "DeviceProfilesSettings";

    private static const String KEY_PROFILE_CONTAINER = "profile_container";
    private static const String KEY_UNPAIR = "unpair";
    private static const String KEY_PBAP_SERVER = "PBAP Server";

    private CachedBluetoothDevice mCachedDevice;
    private LocalBluetoothManager mManager;
    private LocalBluetoothProfileManager mProfileManager;

    private PreferenceGroup mProfileContainer;
    private EditTextPreference mDeviceNamePref;

    private final HashMap<LocalBluetoothProfile, CheckBoxPreference> mAutoConnectPrefs
            = new HashMap<LocalBluetoothProfile, CheckBoxPreference>();

    private AlertDialog mDisconnectDialog;
    private Boolean mProfileGroupIsRemoved;

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        AddPreferencesFromResource(R.xml.bluetooth_device_advanced);
        GetPreferenceScreen()->SetOrderingAsAdded(FALSE);
        mProfileContainer = (PreferenceGroup) FindPreference(KEY_PROFILE_CONTAINER);
        mProfileContainer->SetLayoutResource(R.layout.bluetooth_preference_category);

        mManager = LocalBluetoothManager->GetInstance(GetActivity());
        CachedBluetoothDeviceManager deviceManager =
                mManager->GetCachedDeviceManager();
        mProfileManager = mManager->GetProfileManager();
    }

    //@Override
    CARAPI OnDestroy() {
        super->OnDestroy();
        if (mDisconnectDialog != NULL) {
            mDisconnectDialog->Dismiss();
            mDisconnectDialog = NULL;
        }
        if (mCachedDevice != NULL) {
            mCachedDevice->UnregisterCallback(this);
        }
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        super->OnSaveInstanceState(outState);
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        mManager->SetForegroundActivity(GetActivity());
        if (mCachedDevice != NULL) {
            mCachedDevice->RegisterCallback(this);
            if (mCachedDevice->GetBondState() == BluetoothDevice.BOND_NONE) {
                Finish();
                return;
            }
            Refresh();
        }
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();

        if (mCachedDevice != NULL) {
            mCachedDevice->UnregisterCallback(this);
        }

        mManager->SetForegroundActivity(NULL);
    }

    CARAPI SetDevice(CachedBluetoothDevice cachedDevice) {
        mCachedDevice = cachedDevice;

        if (IsResumed()) {
            mCachedDevice->RegisterCallback(this);
            AddPreferencesForProfiles();
            Refresh();
        }
    }

    private void AddPreferencesForProfiles() {
        mProfileContainer->RemoveAll();
        for (LocalBluetoothProfile profile : mCachedDevice->GetConnectableProfiles()) {
            Preference pref = CreateProfilePreference(profile);
            mProfileContainer->AddPreference(pref);
        }

        final Int32 pbapPermission = mCachedDevice->GetPhonebookPermissionChoice();
        // Only provide PBAP cabability if the client device has requested PBAP.
        if (pbapPermission != CachedBluetoothDevice.ACCESS_UNKNOWN) {
            final PbapServerProfile psp = mManager->GetProfileManager()->GetPbapProfile();
            CheckBoxPreference pbapPref = CreateProfilePreference(psp);
            mProfileContainer->AddPreference(pbapPref);
        }

        final MapProfile mapProfile = mManager->GetProfileManager()->GetMapProfile();
        final Int32 mapPermission = mCachedDevice->GetMessagePermissionChoice();
        if (mapPermission != CachedBluetoothDevice.ACCESS_UNKNOWN) {
            CheckBoxPreference mapPreference = CreateProfilePreference(mapProfile);
            mProfileContainer->AddPreference(mapPreference);
        }

        ShowOrHideProfileGroup();
    }

    private void ShowOrHideProfileGroup() {
        Int32 numProfiles = mProfileContainer->GetPreferenceCount();
        if (!mProfileGroupIsRemoved && numProfiles == 0) {
            GetPreferenceScreen()->RemovePreference(mProfileContainer);
            mProfileGroupIsRemoved = TRUE;
        } else if (mProfileGroupIsRemoved && numProfiles != 0) {
            GetPreferenceScreen()->AddPreference(mProfileContainer);
            mProfileGroupIsRemoved = FALSE;
        }
    }

    /**
     * Creates a checkbox preference for the particular profile. The key will be
     * the profile's name.
     *
     * @param profile The profile for which the preference controls.
     * @return A preference that allows the user to choose whether this profile
     *         will be connected to.
     */
    private CheckBoxPreference CreateProfilePreference(LocalBluetoothProfile profile) {
        CheckBoxPreference pref = new CheckBoxPreference(GetActivity());
        pref->SetLayoutResource(R.layout.preference_start_widget);
        pref->SetKey(profile->ToString());
        pref->SetTitle(profile->GetNameResource(mCachedDevice->GetDevice()));
        pref->SetPersistent(FALSE);
        pref->SetOrder(GetProfilePreferenceIndex(profile->GetOrdinal()));
        pref->SetOnPreferenceChangeListener(this);

        Int32 iconResource = profile->GetDrawableResource(mCachedDevice->GetBtClass());
        if (iconResource != 0) {
            pref->SetIcon(GetResources()->GetDrawable(iconResource));
        }

        RefreshProfilePreference(pref, profile);

        return pref;
    }

    public Boolean OnPreferenceChange(Preference preference, Object newValue) {
        if (preference == mDeviceNamePref) {
            mCachedDevice->SetName((String) newValue);
        } else if (preference instanceof CheckBoxPreference) {
            LocalBluetoothProfile prof = GetProfileOf(preference);
            OnProfileClicked(prof, (CheckBoxPreference) preference);
            return FALSE;   // checkbox will update from OnDeviceAttributesChanged() callback
        } else {
            return FALSE;
        }

        return TRUE;
    }

    private void OnProfileClicked(LocalBluetoothProfile profile, CheckBoxPreference profilePref) {
        BluetoothDevice device = mCachedDevice->GetDevice();

        if (profilePref->GetKey()->Equals(KEY_PBAP_SERVER)) {
            final Int32 newPermission = mCachedDevice->GetPhonebookPermissionChoice()
                == CachedBluetoothDevice.ACCESS_ALLOWED ? CachedBluetoothDevice.ACCESS_REJECTED
                : CachedBluetoothDevice.ACCESS_ALLOWED;
            mCachedDevice->SetPhonebookPermissionChoice(newPermission);
            profilePref->SetChecked(newPermission == CachedBluetoothDevice.ACCESS_ALLOWED);
            return;
        }

        Int32 status = profile->GetConnectionStatus(device);
        Boolean isConnected =
                status == BluetoothProfile.STATE_CONNECTED;

        if (profilePref->IsChecked()) {
            AskDisconnect(mManager->GetForegroundActivity(), profile);
        } else {
            if (profile instanceof MapProfile) {
                mCachedDevice->SetMessagePermissionChoice(BluetoothDevice.ACCESS_ALLOWED);
                RefreshProfilePreference(profilePref, profile);
            }
            if (profile->IsPreferred(device)) {
                // profile is preferred but not connected: disable auto-connect
                profile->SetPreferred(device, FALSE);
                RefreshProfilePreference(profilePref, profile);
            } else {
                profile->SetPreferred(device, TRUE);
                mCachedDevice->ConnectProfile(profile);
            }
        }
    }

    private void AskDisconnect(Context context,
            final LocalBluetoothProfile profile) {
        // local reference for callback
        final CachedBluetoothDevice device = mCachedDevice;
        String name = device->GetName();
        if (TextUtils->IsEmpty(name)) {
            name = context->GetString(R::string::bluetooth_device);
        }

        String profileName = context->GetString(profile->GetNameResource(device->GetDevice()));

        String title = context->GetString(R::string::bluetooth_disable_profile_title);
        String message = context->GetString(R::string::bluetooth_disable_profile_message,
                profileName, name);

        DialogInterface.OnClickListener disconnectListener =
                new DialogInterface->OnClickListener() {
            CARAPI OnClick(DialogInterface dialog, Int32 which) {
                device->Disconnect(profile);
                profile->SetPreferred(device->GetDevice(), FALSE);
                if (profile instanceof MapProfile) {
                    device->SetMessagePermissionChoice(BluetoothDevice.ACCESS_REJECTED);
                    RefreshProfilePreference(
                            (CheckBoxPreference)FindPreference(profile->ToString()), profile);
                }
            }
        };

        mDisconnectDialog = Utils->ShowDisconnectDialog(context,
                mDisconnectDialog, disconnectListener, title, Html->FromHtml(message));
    }

    //@Override
    CARAPI OnDeviceAttributesChanged() {
        Refresh();
    }

    private void Refresh() {
        final EditText deviceNameField = (EditText) GetView()->FindViewById(R.id.name);
        if (deviceNameField != NULL) {
            deviceNameField->SetText(mCachedDevice->GetName());
        }

        RefreshProfiles();
    }

    private void RefreshProfiles() {
        for (LocalBluetoothProfile profile : mCachedDevice->GetConnectableProfiles()) {
            CheckBoxPreference profilePref = (CheckBoxPreference)FindPreference(profile->ToString());
            if (profilePref == NULL) {
                profilePref = CreateProfilePreference(profile);
                mProfileContainer->AddPreference(profilePref);
            } else {
                RefreshProfilePreference(profilePref, profile);
            }
        }
        for (LocalBluetoothProfile profile : mCachedDevice->GetRemovedProfiles()) {
            Preference profilePref = FindPreference(profile->ToString());
            if (profilePref != NULL) {
                Logger::D(TAG, "Removing " + profile->ToString() + " from profile list");
                mProfileContainer->RemovePreference(profilePref);
            }
        }

        ShowOrHideProfileGroup();
    }

    private void RefreshProfilePreference(CheckBoxPreference profilePref,
            LocalBluetoothProfile profile) {
        BluetoothDevice device = mCachedDevice->GetDevice();

        // Gray out checkbox while connecting and disconnecting.
        profilePref->SetEnabled(!mCachedDevice->IsBusy());

        if (profile instanceof MapProfile) {
            profilePref->SetChecked(mCachedDevice->GetMessagePermissionChoice()
                    == CachedBluetoothDevice.ACCESS_ALLOWED);
        } else if (profile instanceof PbapServerProfile) {
            // Handle PBAP specially.
            profilePref->SetChecked(mCachedDevice->GetPhonebookPermissionChoice()
                    == CachedBluetoothDevice.ACCESS_ALLOWED);
        } else {
            profilePref->SetChecked(profile->IsPreferred(device));
        }
    }

    private LocalBluetoothProfile GetProfileOf(Preference pref) {
        if (!(pref instanceof CheckBoxPreference)) {
            return NULL;
        }
        String key = pref->GetKey();
        if (TextUtils->IsEmpty(key)) return NULL;

        try {
            return mProfileManager->GetProfileByName(pref->GetKey());
        } catch (IllegalArgumentException ignored) {
            return NULL;
        }
    }

    private Int32 GetProfilePreferenceIndex(Int32 profIndex) {
        return mProfileContainer->GetOrder() + profIndex * 10;
    }
}
