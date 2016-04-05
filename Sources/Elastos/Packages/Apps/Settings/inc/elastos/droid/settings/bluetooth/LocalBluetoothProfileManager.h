

package com.android.settings.bluetooth;

using Elastos::Droid::Bluetooth::IBluetoothA2dp;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Bluetooth::IBluetoothHeadset;
using Elastos::Droid::Bluetooth::IBluetoothMap;
using Elastos::Droid::Bluetooth::IBluetoothInputDevice;
using Elastos::Droid::Bluetooth::IBluetoothPan;
using Elastos::Droid::Bluetooth::IBluetoothPbap;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Bluetooth::IBluetoothUuid;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IParcelUuid;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;
using Elastos::Utility::IList;

/**
 * LocalBluetoothProfileManager provides access to the LocalBluetoothProfile
 * objects for the available Bluetooth profiles.
 */
final class LocalBluetoothProfileManager {
    private static const String TAG = "LocalBluetoothProfileManager";
    private static const Boolean DEBUG = Utils.D;
    /** Singleton instance. */
    private static LocalBluetoothProfileManager sInstance;

    /**
     * An interface for notifying BluetoothHeadset IPC clients when they have
     * been connected to the BluetoothHeadset service.
     * Only used by {@link DockService}.
     */
    public interface ServiceListener {
        /**
         * Called to notify the client when this proxy object has been
         * connected to the BluetoothHeadset service. Clients must wait for
         * this callback before making IPC calls on the BluetoothHeadset
         * service.
         */
        void OnServiceConnected();

        /**
         * Called to notify the client that this proxy object has been
         * disconnected from the BluetoothHeadset service. Clients must not
         * make IPC calls on the BluetoothHeadset service after this callback.
         * This callback will currently only occur if the application hosting
         * the BluetoothHeadset service, but may be called more often in future.
         */
        void OnServiceDisconnected();
    }

    private final Context mContext;
    private final LocalBluetoothAdapter mLocalAdapter;
    private final CachedBluetoothDeviceManager mDeviceManager;
    private final BluetoothEventManager mEventManager;

    private A2dpProfile mA2dpProfile;
    private HeadsetProfile mHeadsetProfile;
    private MapProfile mMapProfile;
    private final HidProfile mHidProfile;
    private OppProfile mOppProfile;
    private final PanProfile mPanProfile;
    private final PbapServerProfile mPbapProfile;

    /**
     * Mapping from profile name, e.g. "HEADSET" to profile object.
     */
    private final Map<String, LocalBluetoothProfile>
            mProfileNameMap = new HashMap<String, LocalBluetoothProfile>();

    LocalBluetoothProfileManager(Context context,
            LocalBluetoothAdapter adapter,
            CachedBluetoothDeviceManager deviceManager,
            BluetoothEventManager eventManager) {
        mContext = context;

        mLocalAdapter = adapter;
        mDeviceManager = deviceManager;
        mEventManager = eventManager;
        // pass this reference to adapter and event manager (circular dependency)
        mLocalAdapter->SetProfileManager(this);
        mEventManager->SetProfileManager(this);

        ParcelUuid[] uuids = adapter->GetUuids();

        // uuids may be NULL if Bluetooth is turned off
        if (uuids != NULL) {
            UpdateLocalProfiles(uuids);
        }

        // Always add HID and PAN profiles
        mHidProfile = new HidProfile(context, mLocalAdapter, mDeviceManager, this);
        AddProfile(mHidProfile, HidProfile.NAME,
                BluetoothInputDevice.ACTION_CONNECTION_STATE_CHANGED);

        mPanProfile = new PanProfile(context);
        AddPanProfile(mPanProfile, PanProfile.NAME,
                BluetoothPan.ACTION_CONNECTION_STATE_CHANGED);

        If(DEBUG) Logger::D(TAG, "Adding local MAP profile");
        mMapProfile = new MapProfile(mContext, mLocalAdapter,
                mDeviceManager, this);
        AddProfile(mMapProfile, MapProfile.NAME,
                BluetoothMap.ACTION_CONNECTION_STATE_CHANGED);

       //Create PBAP server profile, but do not add it to list of profiles
       // as we do not need to monitor the profile as part of profile list
        mPbapProfile = new PbapServerProfile(context);

        if (DEBUG) Logger::D(TAG, "LocalBluetoothProfileManager construction complete");
    }

    /**
     * Initialize or update the local profile objects. If a UUID was previously
     * present but has been removed, we print a warning but don't remove the
     * profile object as it might be referenced elsewhere, or the UUID might
     * come back and we don't want multiple copies of the profile objects.
     * @param uuids
     */
    void UpdateLocalProfiles(ParcelUuid[] uuids) {
        // A2DP
        if (BluetoothUuid->IsUuidPresent(uuids, BluetoothUuid.AudioSource)) {
            if (mA2dpProfile == NULL) {
                If(DEBUG) Logger::D(TAG, "Adding local A2DP profile");
                mA2dpProfile = new A2dpProfile(mContext, mLocalAdapter, mDeviceManager, this);
                AddProfile(mA2dpProfile, A2dpProfile.NAME,
                        BluetoothA2dp.ACTION_CONNECTION_STATE_CHANGED);
            }
        } else if (mA2dpProfile != NULL) {
            Logger::W(TAG, "Warning: A2DP profile was previously added but the UUID is now missing.");
        }

        // Headset / Handsfree
        if (BluetoothUuid->IsUuidPresent(uuids, BluetoothUuid.Handsfree_AG) ||
            BluetoothUuid->IsUuidPresent(uuids, BluetoothUuid.HSP_AG)) {
            if (mHeadsetProfile == NULL) {
                if (DEBUG) Logger::D(TAG, "Adding local HEADSET profile");
                mHeadsetProfile = new HeadsetProfile(mContext, mLocalAdapter,
                        mDeviceManager, this);
                AddProfile(mHeadsetProfile, HeadsetProfile.NAME,
                        BluetoothHeadset.ACTION_CONNECTION_STATE_CHANGED);
            }
        } else if (mHeadsetProfile != NULL) {
            Logger::W(TAG, "Warning: HEADSET profile was previously added but the UUID is now missing.");
        }

        // OPP
        if (BluetoothUuid->IsUuidPresent(uuids, BluetoothUuid.ObexObjectPush)) {
            if (mOppProfile == NULL) {
                If(DEBUG) Logger::D(TAG, "Adding local OPP profile");
                mOppProfile = new OppProfile();
                // Note: no event handler for OPP, only name map.
                mProfileNameMap->Put(OppProfile.NAME, mOppProfile);
            }
        } else if (mOppProfile != NULL) {
            Logger::W(TAG, "Warning: OPP profile was previously added but the UUID is now missing.");
        }
        mEventManager->RegisterProfileIntentReceiver();

        // There is no local SDP record for HID and Settings app doesn't control PBAP
    }

    private final Collection<ServiceListener> mServiceListeners =
            new ArrayList<ServiceListener>();

    private void AddProfile(LocalBluetoothProfile profile,
            String profileName, String stateChangedAction) {
        mEventManager->AddProfileHandler(stateChangedAction, new StateChangedHandler(profile));
        mProfileNameMap->Put(profileName, profile);
    }

    private void AddPanProfile(LocalBluetoothProfile profile,
            String profileName, String stateChangedAction) {
        mEventManager->AddProfileHandler(stateChangedAction,
                new PanStateChangedHandler(profile));
        mProfileNameMap->Put(profileName, profile);
    }

    LocalBluetoothProfile GetProfileByName(String name) {
        return mProfileNameMap->Get(name);
    }

    // Called from LocalBluetoothAdapter when state changes to ON
    void SetBluetoothStateOn() {
        ParcelUuid[] uuids = mLocalAdapter->GetUuids();
        if (uuids != NULL) {
            UpdateLocalProfiles(uuids);
        }
        mEventManager->ReadPairedDevices();
    }

    /**
     * Generic handler for connection state change events for the specified profile.
     */
    private class StateChangedHandler implements BluetoothEventManager.Handler {
        final LocalBluetoothProfile mProfile;

        StateChangedHandler(LocalBluetoothProfile profile) {
            mProfile = profile;
        }

        CARAPI OnReceive(Context context, Intent intent, BluetoothDevice device) {
            CachedBluetoothDevice cachedDevice = mDeviceManager->FindDevice(device);
            if (cachedDevice == NULL) {
                Logger::W(TAG, "StateChangedHandler found new device: " + device);
                cachedDevice = mDeviceManager->AddDevice(mLocalAdapter,
                        LocalBluetoothProfileManager.this, device);
            }
            Int32 newState = intent->GetIntExtra(BluetoothProfile.EXTRA_STATE, 0);
            Int32 oldState = intent->GetIntExtra(BluetoothProfile.EXTRA_PREVIOUS_STATE, 0);
            if (newState == BluetoothProfile.STATE_DISCONNECTED &&
                    oldState == BluetoothProfile.STATE_CONNECTING) {
                Logger::I(TAG, "Failed to connect " + mProfile + " device");
            }

            cachedDevice->OnProfileStateChanged(mProfile, newState);
            cachedDevice->Refresh();
        }
    }

    /** State change handler for NAP and PANU profiles. */
    private class PanStateChangedHandler extends StateChangedHandler {

        PanStateChangedHandler(LocalBluetoothProfile profile) {
            Super(profile);
        }

        //@Override
        CARAPI OnReceive(Context context, Intent intent, BluetoothDevice device) {
            PanProfile panProfile = (PanProfile) mProfile;
            Int32 role = intent->GetIntExtra(BluetoothPan.EXTRA_LOCAL_ROLE, 0);
            panProfile->SetLocalRole(device, role);
            super->OnReceive(context, intent, device);
        }
    }

    // called from DockService
    void AddServiceListener(ServiceListener l) {
        mServiceListeners->Add(l);
    }

    // called from DockService
    void RemoveServiceListener(ServiceListener l) {
        mServiceListeners->Remove(l);
    }

    // not synchronized: use only from UI thread! (TODO: verify)
    void CallServiceConnectedListeners() {
        for (ServiceListener l : mServiceListeners) {
            l->OnServiceConnected();
        }
    }

    // not synchronized: use only from UI thread! (TODO: verify)
    void CallServiceDisconnectedListeners() {
        for (ServiceListener listener : mServiceListeners) {
            listener->OnServiceDisconnected();
        }
    }

    // This is called by DockService, so check Headset and A2DP.
    public synchronized Boolean IsManagerReady() {
        // Getting just the headset profile is fine for now. Will need to deal with A2DP
        // and others if they aren't always in a ready state.
        LocalBluetoothProfile profile = mHeadsetProfile;
        if (profile != NULL) {
            return profile->IsProfileReady();
        }
        profile = mA2dpProfile;
        if (profile != NULL) {
            return profile->IsProfileReady();
        }
        return FALSE;
    }

    A2dpProfile GetA2dpProfile() {
        return mA2dpProfile;
    }

    HeadsetProfile GetHeadsetProfile() {
        return mHeadsetProfile;
    }

    PbapServerProfile GetPbapProfile(){
        return mPbapProfile;
    }

    MapProfile GetMapProfile(){
        return mMapProfile;
    }

    /**
     * Fill in a list of LocalBluetoothProfile objects that are supported by
     * the local device and the remote device.
     *
     * @param uuids of the remote device
     * @param localUuids UUIDs of the local device
     * @param profiles The list of profiles to fill
     * @param removedProfiles list of profiles that were removed
     */
    synchronized void UpdateProfiles(ParcelUuid[] uuids, ParcelUuid[] localUuids,
            Collection<LocalBluetoothProfile> profiles,
            Collection<LocalBluetoothProfile> removedProfiles,
            Boolean isPanNapConnected, BluetoothDevice device) {
        // Copy previous profile list into removedProfiles
        removedProfiles->Clear();
        removedProfiles->AddAll(profiles);
        profiles->Clear();

        if (uuids == NULL) {
            return;
        }

        if (mHeadsetProfile != NULL) {
            if ((BluetoothUuid->IsUuidPresent(localUuids, BluetoothUuid.HSP_AG) &&
                    BluetoothUuid->IsUuidPresent(uuids, BluetoothUuid.HSP)) ||
                    (BluetoothUuid->IsUuidPresent(localUuids, BluetoothUuid.Handsfree_AG) &&
                            BluetoothUuid->IsUuidPresent(uuids, BluetoothUuid.Handsfree))) {
                profiles->Add(mHeadsetProfile);
                removedProfiles->Remove(mHeadsetProfile);
            }
        }

        if (BluetoothUuid->ContainsAnyUuid(uuids, A2dpProfile.SINK_UUIDS) &&
            mA2dpProfile != NULL) {
            profiles->Add(mA2dpProfile);
            removedProfiles->Remove(mA2dpProfile);
        }

        if (BluetoothUuid->IsUuidPresent(uuids, BluetoothUuid.ObexObjectPush) &&
            mOppProfile != NULL) {
            profiles->Add(mOppProfile);
            removedProfiles->Remove(mOppProfile);
        }

        if ((BluetoothUuid->IsUuidPresent(uuids, BluetoothUuid.Hid) ||
             BluetoothUuid->IsUuidPresent(uuids, BluetoothUuid.Hogp)) &&
            mHidProfile != NULL) {
            profiles->Add(mHidProfile);
            removedProfiles->Remove(mHidProfile);
        }

        If(isPanNapConnected)
            If(DEBUG) Logger::D(TAG, "Valid PAN-NAP connection exists.");
        if ((BluetoothUuid->IsUuidPresent(uuids, BluetoothUuid.NAP) &&
            mPanProfile != NULL) || isPanNapConnected) {
            profiles->Add(mPanProfile);
            removedProfiles->Remove(mPanProfile);
        }

        if ((mMapProfile != NULL) &&
            (mMapProfile->GetConnectionStatus(device) == BluetoothProfile.STATE_CONNECTED)) {
            profiles->Add(mMapProfile);
            removedProfiles->Remove(mMapProfile);
            mMapProfile->SetPreferred(device, TRUE);
        }
    }

}
