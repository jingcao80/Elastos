

package com.android.settings.bluetooth;

using Elastos::Droid::Bluetooth::IBluetoothA2dp;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothClass;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Bluetooth::IBluetoothUuid;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IParcelUuid;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Settings::IR;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

final class A2dpProfile implements LocalBluetoothProfile {
    private static const String TAG = "A2dpProfile";
    private static Boolean V = TRUE;

    private BluetoothA2dp mService;
    private Boolean mIsProfileReady;

    private final LocalBluetoothAdapter mLocalAdapter;
    private final CachedBluetoothDeviceManager mDeviceManager;

    static const ParcelUuid[] SINK_UUIDS = {
        BluetoothUuid.AudioSink,
        BluetoothUuid.AdvAudioDist,
    };

    static const String NAME = "A2DP";
    private final LocalBluetoothProfileManager mProfileManager;

    // Order of this profile in device profiles list
    private static const Int32 ORDINAL = 1;

    // These callbacks run on the main thread.
    private final class A2dpServiceListener
            implements BluetoothProfile.ServiceListener {

        CARAPI OnServiceConnected(Int32 profile, BluetoothProfile proxy) {
            if (V) Logger::D(TAG,"Bluetooth service connected");
            mService = (BluetoothA2dp) proxy;
            // We just bound to the service, so refresh the UI for any connected A2DP devices.
            List<BluetoothDevice> deviceList = mService->GetConnectedDevices();
            while (!deviceList->IsEmpty()) {
                BluetoothDevice nextDevice = deviceList->Remove(0);
                CachedBluetoothDevice device = mDeviceManager->FindDevice(nextDevice);
                // we may add a new device here, but generally this should not happen
                if (device == NULL) {
                    Logger::W(TAG, "A2dpProfile found new device: " + nextDevice);
                    device = mDeviceManager->AddDevice(mLocalAdapter, mProfileManager, nextDevice);
                }
                device->OnProfileStateChanged(A2dpProfile.this, BluetoothProfile.STATE_CONNECTED);
                device->Refresh();
            }
            mIsProfileReady=TRUE;
        }

        CARAPI OnServiceDisconnected(Int32 profile) {
            if (V) Logger::D(TAG,"Bluetooth service disconnected");
            mIsProfileReady=FALSE;
        }
    }

    public Boolean IsProfileReady() {
        return mIsProfileReady;
    }

    A2dpProfile(Context context, LocalBluetoothAdapter adapter,
            CachedBluetoothDeviceManager deviceManager,
            LocalBluetoothProfileManager profileManager) {
        mLocalAdapter = adapter;
        mDeviceManager = deviceManager;
        mProfileManager = profileManager;
        mLocalAdapter->GetProfileProxy(context, new A2dpServiceListener(),
                BluetoothProfile.A2DP);
    }

    public Boolean IsConnectable() {
        return TRUE;
    }

    public Boolean IsAutoConnectable() {
        return TRUE;
    }

    public List<BluetoothDevice> GetConnectedDevices() {
        if (mService == NULL) return new ArrayList<BluetoothDevice>(0);
        return mService->GetDevicesMatchingConnectionStates(
              new Int32[] {BluetoothProfile.STATE_CONNECTED,
                         BluetoothProfile.STATE_CONNECTING,
                         BluetoothProfile.STATE_DISCONNECTING});
    }

    public Boolean Connect(BluetoothDevice device) {
        if (mService == NULL) return FALSE;
        List<BluetoothDevice> sinks = GetConnectedDevices();
        if (sinks != NULL) {
            for (BluetoothDevice sink : sinks) {
                mService->Disconnect(sink);
            }
        }
        return mService->Connect(device);
    }

    public Boolean Disconnect(BluetoothDevice device) {
        if (mService == NULL) return FALSE;
        // Downgrade priority as user is disconnecting the headset.
        if (mService->GetPriority(device) > BluetoothProfile.PRIORITY_ON){
            mService->SetPriority(device, BluetoothProfile.PRIORITY_ON);
        }
        return mService->Disconnect(device);
    }

    public Int32 GetConnectionStatus(BluetoothDevice device) {
        if (mService == NULL) {
            return BluetoothProfile.STATE_DISCONNECTED;
        }
        return mService->GetConnectionState(device);
    }

    public Boolean IsPreferred(BluetoothDevice device) {
        if (mService == NULL) return FALSE;
        return mService->GetPriority(device) > BluetoothProfile.PRIORITY_OFF;
    }

    public Int32 GetPreferred(BluetoothDevice device) {
        if (mService == NULL) return BluetoothProfile.PRIORITY_OFF;
        return mService->GetPriority(device);
    }

    CARAPI SetPreferred(BluetoothDevice device, Boolean preferred) {
        if (mService == NULL) return;
        if (preferred) {
            if (mService->GetPriority(device) < BluetoothProfile.PRIORITY_ON) {
                mService->SetPriority(device, BluetoothProfile.PRIORITY_ON);
            }
        } else {
            mService->SetPriority(device, BluetoothProfile.PRIORITY_OFF);
        }
    }
    Boolean IsA2dpPlaying() {
        if (mService == NULL) return FALSE;
        List<BluetoothDevice> sinks = mService->GetConnectedDevices();
        if (!sinks->IsEmpty()) {
            if (mService->IsA2dpPlaying(sinks->Get(0))) {
                return TRUE;
            }
        }
        return FALSE;
    }

    CARAPI ToString(
        /* [out] */ String* str)
    {
        return NAME;
    }

    public Int32 GetOrdinal() {
        return ORDINAL;
    }

    public Int32 GetNameResource(BluetoothDevice device) {
        return R::string::bluetooth_profile_a2dp;
    }

    public Int32 GetSummaryResourceForDevice(BluetoothDevice device) {
        Int32 state = GetConnectionStatus(device);
        switch (state) {
            case BluetoothProfile.STATE_DISCONNECTED:
                return R::string::bluetooth_a2dp_profile_summary_use_for;

            case BluetoothProfile.STATE_CONNECTED:
                return R::string::bluetooth_a2dp_profile_summary_connected;

            default:
                return Utils->GetConnectionStateSummary(state);
        }
    }

    public Int32 GetDrawableResource(BluetoothClass btClass) {
        return R.drawable.ic_bt_headphones_a2dp;
    }

    protected void Finalize() {
        if (V) Logger::D(TAG, "Finalize()");
        if (mService != NULL) {
            try {
                BluetoothAdapter->GetDefaultAdapter()->CloseProfileProxy(BluetoothProfile.A2DP,
                                                                       mService);
                mService = NULL;
            }catch (Throwable t) {
                Logger::W(TAG, "Error cleaning up A2DP proxy", t);
            }
        }
    }
}
