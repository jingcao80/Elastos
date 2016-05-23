

package com.android.settings.bluetooth;

using Elastos::Droid::Bluetooth::IBluetoothClass;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Os::IParcelUuid;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;

/**
 * CachedBluetoothDevice represents a remote Bluetooth device. It contains
 * attributes of the device (such as the address, name, RSSI, etc.) and
 * functionality that can be performed on the device (connect, pair, disconnect,
 * etc.).
 */
final class CachedBluetoothDevice implements Comparable<CachedBluetoothDevice> {
    private static const String TAG = "CachedBluetoothDevice";
    private static const Boolean DEBUG = Utils.V;

    private final Context mContext;
    private final LocalBluetoothAdapter mLocalAdapter;
    private final LocalBluetoothProfileManager mProfileManager;
    private final BluetoothDevice mDevice;
    private String mName;
    private short mRssi;
    private BluetoothClass mBtClass;
    private HashMap<LocalBluetoothProfile, Integer> mProfileConnectionState;

    private final List<LocalBluetoothProfile> mProfiles =
            new ArrayList<LocalBluetoothProfile>();

    // List of profiles that were previously in mProfiles, but have been removed
    private final List<LocalBluetoothProfile> mRemovedProfiles =
            new ArrayList<LocalBluetoothProfile>();

    // Device supports PANU but not NAP: remove PanProfile after device disconnects from NAP
    private Boolean mLocalNapRoleConnected;

    private Boolean mVisible;

    private Int32 mPhonebookPermissionChoice;

    private Int32 mMessagePermissionChoice;

    private Int32 mMessageRejectionCount;

    private final Collection<Callback> mCallbacks = new ArrayList<Callback>();

    // Following constants indicate the user's choices of Phone book/message access settings
    // User hasn't made any choice or settings app has wiped out the memory
    public static const Int32 ACCESS_UNKNOWN = 0;
    // User has accepted the connection and let Settings app remember the decision
    public static const Int32 ACCESS_ALLOWED = 1;
    // User has rejected the connection and let Settings app remember the decision
    public static const Int32 ACCESS_REJECTED = 2;

    // How many times user should reject the connection to make the choice persist.
    private static const Int32 MESSAGE_REJECTION_COUNT_LIMIT_TO_PERSIST = 2;

    private static const String MESSAGE_REJECTION_COUNT_PREFS_NAME = "bluetooth_message_reject";

    /**
     * When we connect to multiple profiles, we only want to display a single
     * error even if they all fail. This tracks that state.
     */
    private Boolean mIsConnectingErrorPossible;

    /**
     * Last time a bt profile auto-connect was attempted.
     * If an ACTION_UUID intent comes in within
     * MAX_UUID_DELAY_FOR_AUTO_CONNECT milliseconds, we will try auto-connect
     * again with the new UUIDs
     */
    private Int64 mConnectAttempted;

    // See mConnectAttempted
    private static const Int64 MAX_UUID_DELAY_FOR_AUTO_CONNECT = 5000;

    /** Auto-connect after pairing only if locally initiated. */
    private Boolean mConnectAfterPairing;

    /**
     * Describes the current device and profile for logging.
     *
     * @param profile Profile to describe
     * @return Description of the device and profile
     */
    private String Describe(LocalBluetoothProfile profile) {
        StringBuilder sb = new StringBuilder();
        sb->Append("Address:").Append(mDevice);
        if (profile != NULL) {
            sb->Append(" Profile:").Append(profile);
        }

        return sb->ToString();
    }

    void OnProfileStateChanged(LocalBluetoothProfile profile, Int32 newProfileState) {
        if (Utils.D) {
            Logger::D(TAG, "onProfileStateChanged: profile " + profile +
                    " newProfileState " + newProfileState);
        }
        if (mLocalAdapter->GetBluetoothState() == BluetoothAdapter.STATE_TURNING_OFF)
        {
            if (Utils.D) Logger::D(TAG, " BT Turninig Off...Profile conn state change ignored...");
            return;
        }
        mProfileConnectionState->Put(profile, newProfileState);
        if (newProfileState == BluetoothProfile.STATE_CONNECTED) {
            if (profile instanceof MapProfile) {
                profile->SetPreferred(mDevice, TRUE);
            } else if (!mProfiles->Contains(profile)) {
                mRemovedProfiles->Remove(profile);
                mProfiles->Add(profile);
                if (profile instanceof PanProfile &&
                        ((PanProfile) profile).IsLocalRoleNap(mDevice)) {
                    // Device doesn't support NAP, so remove PanProfile on disconnect
                    mLocalNapRoleConnected = TRUE;
                }
            }
        } else if (profile instanceof MapProfile &&
                newProfileState == BluetoothProfile.STATE_DISCONNECTED) {
            profile->SetPreferred(mDevice, FALSE);
        } else if (mLocalNapRoleConnected && profile instanceof PanProfile &&
                ((PanProfile) profile).IsLocalRoleNap(mDevice) &&
                newProfileState == BluetoothProfile.STATE_DISCONNECTED) {
            Logger::D(TAG, "Removing PanProfile from device after NAP disconnect");
            mProfiles->Remove(profile);
            mRemovedProfiles->Add(profile);
            mLocalNapRoleConnected = FALSE;
        }
    }

    CachedBluetoothDevice(Context context,
                          LocalBluetoothAdapter adapter,
                          LocalBluetoothProfileManager profileManager,
                          BluetoothDevice device) {
        mContext = context;
        mLocalAdapter = adapter;
        mProfileManager = profileManager;
        mDevice = device;
        mProfileConnectionState = new HashMap<LocalBluetoothProfile, Integer>();
        FillData();
    }

    void Disconnect() {
        for (LocalBluetoothProfile profile : mProfiles) {
            Disconnect(profile);
        }
        // Disconnect  PBAP server in case its connected
        // This is to ensure all the profiles are disconnected as some CK/Hs do not
        // disconnect  PBAP connection when HF connection is brought down
        PbapServerProfile PbapProfile = mProfileManager->GetPbapProfile();
        if (PbapProfile->GetConnectionStatus(mDevice) == BluetoothProfile.STATE_CONNECTED)
        {
            PbapProfile->Disconnect(mDevice);
        }
    }

    void Disconnect(LocalBluetoothProfile profile) {
        if (profile->Disconnect(mDevice)) {
            if (Utils.D) {
                Logger::D(TAG, "Command sent successfully:DISCONNECT " + Describe(profile));
            }
        }
    }

    void Connect(Boolean connectAllProfiles) {
        if (!EnsurePaired()) {
            return;
        }

        mConnectAttempted = SystemClock->ElapsedRealtime();
        ConnectWithoutResettingTimer(connectAllProfiles);
    }

    void OnBondingDockConnect() {
        // Attempt to connect if UUIDs are available. Otherwise,
        // we will connect when the ACTION_UUID intent arrives.
        Connect(FALSE);
    }

    private void ConnectWithoutResettingTimer(Boolean connectAllProfiles) {
        // Try to initialize the profiles if they were not.
        if (mProfiles->IsEmpty()) {
            // if mProfiles is empty, then do not invoke updateProfiles. This causes a race
            // condition with carkits during pairing, wherein RemoteDevice.UUIDs have been updated
            // from bluetooth stack but ACTION.uuid is not sent yet.
            // Eventually ACTION.uuid will be received which shall trigger the connection of the
            // various profiles
            // If UUIDs are not available yet, connect will be happen
            // upon arrival of the ACTION_UUID intent.
            Logger::D(TAG, "No profiles. Maybe we will connect later");
            return;
        }

        // Reset the only-show-one-error-dialog tracking variable
        mIsConnectingErrorPossible = TRUE;

        Int32 preferredProfiles = 0;
        for (LocalBluetoothProfile profile : mProfiles) {
            if (connectAllProfiles ? profile->IsConnectable() : profile->IsAutoConnectable()) {
                if (profile->IsPreferred(mDevice)) {
                    ++preferredProfiles;
                    ConnectInt(profile);
                }
            }
        }
        if (DEBUG) Logger::D(TAG, "Preferred profiles = " + preferredProfiles);

        if (preferredProfiles == 0) {
            ConnectAutoConnectableProfiles();
        }
    }

    private void ConnectAutoConnectableProfiles() {
        if (!EnsurePaired()) {
            return;
        }
        // Reset the only-show-one-error-dialog tracking variable
        mIsConnectingErrorPossible = TRUE;

        for (LocalBluetoothProfile profile : mProfiles) {
            if (profile->IsAutoConnectable()) {
                profile->SetPreferred(mDevice, TRUE);
                ConnectInt(profile);
            }
        }
    }

    /**
     * Connect this device to the specified profile.
     *
     * @param profile the profile to use with the remote device
     */
    void ConnectProfile(LocalBluetoothProfile profile) {
        mConnectAttempted = SystemClock->ElapsedRealtime();
        // Reset the only-show-one-error-dialog tracking variable
        mIsConnectingErrorPossible = TRUE;
        ConnectInt(profile);
        // Refresh the UI based on profile->Connect() call
        Refresh();
    }

    synchronized void ConnectInt(LocalBluetoothProfile profile) {
        if (!EnsurePaired()) {
            return;
        }
        if (profile->Connect(mDevice)) {
            if (Utils.D) {
                Logger::D(TAG, "Command sent successfully:CONNECT " + Describe(profile));
            }
            return;
        }
        Logger::I(TAG, "Failed to connect " + profile->ToString() + " to " + mName);
    }

    private Boolean EnsurePaired() {
        if (GetBondState() == BluetoothDevice.BOND_NONE) {
            StartPairing();
            return FALSE;
        } else {
            return TRUE;
        }
    }

    Boolean StartPairing() {
        // Pairing is unreliable while scanning, so cancel discovery
        if (mLocalAdapter->IsDiscovering()) {
            mLocalAdapter->CancelDiscovery();
        }

        if (!mDevice->CreateBond()) {
            return FALSE;
        }

        mConnectAfterPairing = TRUE;  // auto-connect after pairing
        return TRUE;
    }

    /**
     * Return TRUE if user initiated pairing on this device. The message text is
     * slightly different for local vs. remote initiated pairing dialogs.
     */
    Boolean IsUserInitiatedPairing() {
        return mConnectAfterPairing;
    }

    void Unpair() {
        Int32 state = GetBondState();

        if (state == BluetoothDevice.BOND_BONDING) {
            mDevice->CancelBondProcess();
        }

        if (state != BluetoothDevice.BOND_NONE) {
            final BluetoothDevice dev = mDevice;
            if (dev != NULL) {
                final Boolean successful = dev->RemoveBond();
                if (successful) {
                    if (Utils.D) {
                        Logger::D(TAG, "Command sent successfully:REMOVE_BOND " + Describe(NULL));
                    }
                } else if (Utils.V) {
                    Logger::V(TAG, "Framework rejected command immediately:REMOVE_BOND " +
                            Describe(NULL));
                }
            }
        }
    }

    Int32 GetProfileConnectionState(LocalBluetoothProfile profile) {
        if (mProfileConnectionState == NULL ||
                mProfileConnectionState->Get(profile) == NULL) {
            // If cache is empty make the binder call to get the state
            Int32 state = profile->GetConnectionStatus(mDevice);
            mProfileConnectionState->Put(profile, state);
        }
        return mProfileConnectionState->Get(profile);
    }

    CARAPI clearProfileConnectionState ()
    {
        if (Utils.D) {
            Logger::D(TAG," Clearing all connection state for dev:" + mDevice->GetName());
        }
        for (LocalBluetoothProfile profile :GetProfiles()) {
            mProfileConnectionState->Put(profile, BluetoothProfile.STATE_DISCONNECTED);
        }
    }

    // TODO: do any of these need to run async on a background thread?
    private void FillData() {
        FetchName();
        FetchBtClass();
        UpdateProfiles();
        MigratePhonebookPermissionChoice();
        MigrateMessagePermissionChoice();
        FetchMessageRejectionCount();

        mVisible = FALSE;
        DispatchAttributesChanged();
    }

    BluetoothDevice GetDevice() {
        return mDevice;
    }

    String GetName() {
        return mName;
    }

    /**
     * Populate name from BluetoothDevice.ACTION_FOUND intent
     */
    void SetNewName(String name) {
        if (mName == NULL) {
            mName = name;
            if (mName == NULL || TextUtils->IsEmpty(mName)) {
                mName = mDevice->GetAddress();
            }
            DispatchAttributesChanged();
        }
    }

    /**
     * user changes the device name
     */
    void SetName(String name) {
        if (!mName->Equals(name)) {
            mName = name;
            mDevice->SetAlias(name);
            DispatchAttributesChanged();
        }
    }

    void RefreshName() {
        FetchName();
        DispatchAttributesChanged();
    }

    private void FetchName() {
        mName = mDevice->GetAliasName();

        if (TextUtils->IsEmpty(mName)) {
            mName = mDevice->GetAddress();
            if (DEBUG) Logger::D(TAG, "Device has no name (yet), use address: " + mName);
        }
    }

    void Refresh() {
        DispatchAttributesChanged();
    }

    Boolean IsVisible() {
        return mVisible;
    }

    void SetVisible(Boolean visible) {
        if (mVisible != visible) {
            mVisible = visible;
            DispatchAttributesChanged();
        }
    }

    Int32 GetBondState() {
        return mDevice->GetBondState();
    }

    void SetRssi(short rssi) {
        if (mRssi != rssi) {
            mRssi = rssi;
            DispatchAttributesChanged();
        }
    }

    /**
     * Checks whether we are connected to this device (any profile counts).
     *
     * @return Whether it is connected.
     */
    Boolean IsConnected() {
        for (LocalBluetoothProfile profile : mProfiles) {
            Int32 status = GetProfileConnectionState(profile);
            if (status == BluetoothProfile.STATE_CONNECTED) {
                return TRUE;
            }
        }

        return FALSE;
    }

    Boolean IsConnectedProfile(LocalBluetoothProfile profile) {
        Int32 status = GetProfileConnectionState(profile);
        return status == BluetoothProfile.STATE_CONNECTED;

    }

    Boolean IsBusy() {
        for (LocalBluetoothProfile profile : mProfiles) {
            Int32 status = GetProfileConnectionState(profile);
            if (status == BluetoothProfile.STATE_CONNECTING
                    || status == BluetoothProfile.STATE_DISCONNECTING) {
                return TRUE;
            }
        }
        return GetBondState() == BluetoothDevice.BOND_BONDING;
    }

    /**
     * Fetches a new value for the cached BT class.
     */
    private void FetchBtClass() {
        mBtClass = mDevice->GetBluetoothClass();
    }

    private Boolean UpdateProfiles() {
        ParcelUuid[] uuids = mDevice->GetUuids();
        if (uuids == NULL) return FALSE;

        ParcelUuid[] localUuids = mLocalAdapter->GetUuids();
        if (localUuids == NULL) return FALSE;

        mProfileManager->UpdateProfiles(uuids, localUuids, mProfiles, mRemovedProfiles,
                                       mLocalNapRoleConnected, mDevice);

        if (DEBUG) {
            Logger::E(TAG, "updating profiles for " + mDevice->GetAliasName());
            BluetoothClass bluetoothClass = mDevice->GetBluetoothClass();

            if (bluetoothClass != NULL) Logger::V(TAG, "Class: " + bluetoothClass->ToString());
            Logger::V(TAG, "UUID:");
            for (ParcelUuid uuid : uuids) {
                Logger::V(TAG, "  " + uuid);
            }
        }
        return TRUE;
    }

    /**
     * Refreshes the UI for the BT class, including fetching the latest value
     * for the class.
     */
    void RefreshBtClass() {
        FetchBtClass();
        DispatchAttributesChanged();
    }

    /**
     * Refreshes the UI when framework alerts us of a UUID change.
     */
    void OnUuidChanged() {
        UpdateProfiles();

        if (DEBUG) {
            Logger::E(TAG, "onUuidChanged: Time since last connect"
                    + (SystemClock->ElapsedRealtime() - mConnectAttempted));
        }

        /*
         * If a connect was attempted earlier without any UUID, we will do the
         * connect now.
         */
        if (!mProfiles->IsEmpty()
                && (mConnectAttempted + MAX_UUID_DELAY_FOR_AUTO_CONNECT) > SystemClock
                        .ElapsedRealtime()) {
            ConnectWithoutResettingTimer(FALSE);
        }
        DispatchAttributesChanged();
    }

    void OnBondingStateChanged(Int32 bondState) {
        if (bondState == BluetoothDevice.BOND_NONE) {
            mProfiles->Clear();
            mConnectAfterPairing = FALSE;  // cancel auto-connect
            SetPhonebookPermissionChoice(ACCESS_UNKNOWN);
            SetMessagePermissionChoice(ACCESS_UNKNOWN);
            mMessageRejectionCount = 0;
            SaveMessageRejectionCount();
        }

        Refresh();

        if (bondState == BluetoothDevice.BOND_BONDED) {
            if (mDevice->IsBluetoothDock()) {
                OnBondingDockConnect();
            } else if (mConnectAfterPairing) {
                Connect(FALSE);
            }
            mConnectAfterPairing = FALSE;
        }
    }

    void SetBtClass(BluetoothClass btClass) {
        if (btClass != NULL && mBtClass != btClass) {
            mBtClass = btClass;
            DispatchAttributesChanged();
        }
    }

    BluetoothClass GetBtClass() {
        return mBtClass;
    }

    List<LocalBluetoothProfile> GetProfiles() {
        return Collections->UnmodifiableList(mProfiles);
    }

    List<LocalBluetoothProfile> GetConnectableProfiles() {
        List<LocalBluetoothProfile> connectableProfiles =
                new ArrayList<LocalBluetoothProfile>();
        for (LocalBluetoothProfile profile : mProfiles) {
            if (profile->IsConnectable()) {
                connectableProfiles->Add(profile);
            }
        }
        return connectableProfiles;
    }

    List<LocalBluetoothProfile> GetRemovedProfiles() {
        return mRemovedProfiles;
    }

    void RegisterCallback(Callback callback) {
        {    AutoLock syncLock(mCallbacks);
            mCallbacks->Add(callback);
        }
    }

    void UnregisterCallback(Callback callback) {
        {    AutoLock syncLock(mCallbacks);
            mCallbacks->Remove(callback);
        }
    }

    private void DispatchAttributesChanged() {
        {    AutoLock syncLock(mCallbacks);
            for (Callback callback : mCallbacks) {
                callback->OnDeviceAttributesChanged();
            }
        }
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return mDevice->ToString();
    }

    //@Override
    public Boolean Equals(Object o) {
        if ((o == NULL) || !(o instanceof CachedBluetoothDevice)) {
            return FALSE;
        }
        return mDevice->Equals(((CachedBluetoothDevice) o).mDevice);
    }

    //@Override
    public Int32 HashCode() {
        return mDevice->GetAddress()->HashCode();
    }

    // This comparison uses non-final fields so the sort order may change
    // when device attributes change (such as bonding state). Settings
    // will completely refresh the device list when this happens.
    public Int32 CompareTo(CachedBluetoothDevice another) {
        // Connected above not connected
        Int32 comparison = (another->IsConnected() ? 1 : 0) - (IsConnected() ? 1 : 0);
        if (comparison != 0) return comparison;

        // Paired above not paired
        comparison = (another->GetBondState() == BluetoothDevice.BOND_BONDED ? 1 : 0) -
            (GetBondState() == BluetoothDevice.BOND_BONDED ? 1 : 0);
        if (comparison != 0) return comparison;

        // Visible above not visible
        comparison = (another.mVisible ? 1 : 0) - (mVisible ? 1 : 0);
        if (comparison != 0) return comparison;

        // Stronger signal above weaker signal
        comparison = another.mRssi - mRssi;
        if (comparison != 0) return comparison;

        // Fallback on name
        return mName->CompareTo(another.mName);
    }

    public interface Callback {
        void OnDeviceAttributesChanged();
    }

    Int32 GetPhonebookPermissionChoice() {
        Int32 permission = mDevice->GetPhonebookAccessPermission();
        if (permission == BluetoothDevice.ACCESS_ALLOWED) {
            return ACCESS_ALLOWED;
        } else if (permission == BluetoothDevice.ACCESS_REJECTED) {
            return ACCESS_REJECTED;
        }
        return ACCESS_UNKNOWN;
    }

    void SetPhonebookPermissionChoice(Int32 permissionChoice) {
        Int32 permission = BluetoothDevice.ACCESS_UNKNOWN;
        if (permissionChoice == ACCESS_ALLOWED) {
            permission = BluetoothDevice.ACCESS_ALLOWED;
        } else if (permissionChoice == ACCESS_REJECTED) {
            permission = BluetoothDevice.ACCESS_REJECTED;
        }
        mDevice->SetPhonebookAccessPermission(permission);
    }

    // Migrates data from old data store (in Settings app's shared preferences) to new (in Bluetooth
    // app's shared preferences).
    private void MigratePhonebookPermissionChoice() {
        SharedPreferences preferences = mContext->GetSharedPreferences(
                "bluetooth_phonebook_permission", Context.MODE_PRIVATE);
        if (!preferences->Contains(mDevice->GetAddress())) {
            return;
        }

        if (mDevice->GetPhonebookAccessPermission() == BluetoothDevice.ACCESS_UNKNOWN) {
            Int32 oldPermission = preferences->GetInt(mDevice->GetAddress(), ACCESS_UNKNOWN);
            if (oldPermission == ACCESS_ALLOWED) {
                mDevice->SetPhonebookAccessPermission(BluetoothDevice.ACCESS_ALLOWED);
            } else if (oldPermission == ACCESS_REJECTED) {
                mDevice->SetPhonebookAccessPermission(BluetoothDevice.ACCESS_REJECTED);
            }
        }

        SharedPreferences.Editor editor = preferences->Edit();
        editor->Remove(mDevice->GetAddress());
        editor->Commit();
    }

    Int32 GetMessagePermissionChoice() {
        Int32 permission = mDevice->GetMessageAccessPermission();
        if (permission == BluetoothDevice.ACCESS_ALLOWED) {
            return ACCESS_ALLOWED;
        } else if (permission == BluetoothDevice.ACCESS_REJECTED) {
            return ACCESS_REJECTED;
        }
        return ACCESS_UNKNOWN;
    }

    void SetMessagePermissionChoice(Int32 permissionChoice) {
        Int32 permission = BluetoothDevice.ACCESS_UNKNOWN;
        if (permissionChoice == ACCESS_ALLOWED) {
            permission = BluetoothDevice.ACCESS_ALLOWED;
        } else if (permissionChoice == ACCESS_REJECTED) {
            permission = BluetoothDevice.ACCESS_REJECTED;
        }
        mDevice->SetMessageAccessPermission(permission);
    }

    // Migrates data from old data store (in Settings app's shared preferences) to new (in Bluetooth
    // app's shared preferences).
    private void MigrateMessagePermissionChoice() {
        SharedPreferences preferences = mContext->GetSharedPreferences(
                "bluetooth_message_permission", Context.MODE_PRIVATE);
        if (!preferences->Contains(mDevice->GetAddress())) {
            return;
        }

        if (mDevice->GetMessageAccessPermission() == BluetoothDevice.ACCESS_UNKNOWN) {
            Int32 oldPermission = preferences->GetInt(mDevice->GetAddress(), ACCESS_UNKNOWN);
            if (oldPermission == ACCESS_ALLOWED) {
                mDevice->SetMessageAccessPermission(BluetoothDevice.ACCESS_ALLOWED);
            } else if (oldPermission == ACCESS_REJECTED) {
                mDevice->SetMessageAccessPermission(BluetoothDevice.ACCESS_REJECTED);
            }
        }

        SharedPreferences.Editor editor = preferences->Edit();
        editor->Remove(mDevice->GetAddress());
        editor->Commit();
    }

    /**
     * @return Whether this rejection should persist.
     */
    Boolean CheckAndIncreaseMessageRejectionCount() {
        if (mMessageRejectionCount < MESSAGE_REJECTION_COUNT_LIMIT_TO_PERSIST) {
            mMessageRejectionCount++;
            SaveMessageRejectionCount();
        }
        return mMessageRejectionCount >= MESSAGE_REJECTION_COUNT_LIMIT_TO_PERSIST;
    }

    private void FetchMessageRejectionCount() {
        SharedPreferences preference = mContext->GetSharedPreferences(
                MESSAGE_REJECTION_COUNT_PREFS_NAME, Context.MODE_PRIVATE);
        mMessageRejectionCount = preference->GetInt(mDevice->GetAddress(), 0);
    }

    private void SaveMessageRejectionCount() {
        SharedPreferences.Editor editor = mContext->GetSharedPreferences(
                MESSAGE_REJECTION_COUNT_PREFS_NAME, Context.MODE_PRIVATE).Edit();
        if (mMessageRejectionCount == 0) {
            editor->Remove(mDevice->GetAddress());
        } else {
            editor->PutInt(mDevice->GetAddress(), mMessageRejectionCount);
        }
        editor->Commit();
    }
}
