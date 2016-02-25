

package com.android.settings.bluetooth;

using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothClass;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Bluetooth::IBluetoothPan;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Settings::IR;

using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;

/**
 * PanProfile handles Bluetooth PAN profile (NAP and PANU).
 */
final class PanProfile implements LocalBluetoothProfile {
    private static const String TAG = "PanProfile";
    private static Boolean V = TRUE;

    private BluetoothPan mService;
    private Boolean mIsProfileReady;

    // Tethering direction for each device
    private final HashMap<BluetoothDevice, Integer> mDeviceRoleMap =
            new HashMap<BluetoothDevice, Integer>();

    static const String NAME = "PAN";

    // Order of this profile in device profiles list
    private static const Int32 ORDINAL = 4;

    // These callbacks run on the main thread.
    private final class PanServiceListener
            implements BluetoothProfile.ServiceListener {

        CARAPI OnServiceConnected(Int32 profile, BluetoothProfile proxy) {
            if (V) Logger::D(TAG,"Bluetooth service connected");
            mService = (BluetoothPan) proxy;
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

    PanProfile(Context context) {
        BluetoothAdapter adapter = BluetoothAdapter->GetDefaultAdapter();
        adapter->GetProfileProxy(context, new PanServiceListener(),
                BluetoothProfile.PAN);
    }

    public Boolean IsConnectable() {
        return TRUE;
    }

    public Boolean IsAutoConnectable() {
        return FALSE;
    }

    public Boolean Connect(BluetoothDevice device) {
        if (mService == NULL) return FALSE;
        List<BluetoothDevice> sinks = mService->GetConnectedDevices();
        if (sinks != NULL) {
            for (BluetoothDevice sink : sinks) {
                mService->Disconnect(sink);
            }
        }
        return mService->Connect(device);
    }

    public Boolean Disconnect(BluetoothDevice device) {
        if (mService == NULL) return FALSE;
        return mService->Disconnect(device);
    }

    public Int32 GetConnectionStatus(BluetoothDevice device) {
        if (mService == NULL) {
            return BluetoothProfile.STATE_DISCONNECTED;
        }
        return mService->GetConnectionState(device);
    }

    public Boolean IsPreferred(BluetoothDevice device) {
        // return current connection status so profile checkbox is set correctly
        return GetConnectionStatus(device) == BluetoothProfile.STATE_CONNECTED;
    }

    public Int32 GetPreferred(BluetoothDevice device) {
        return -1;
    }

    CARAPI SetPreferred(BluetoothDevice device, Boolean preferred) {
        // ignore: isPreferred is always TRUE for PAN
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
        if (IsLocalRoleNap(device)) {
            return R::string::bluetooth_profile_pan_nap;
        } else {
            return R::string::bluetooth_profile_pan;
        }
    }

    public Int32 GetSummaryResourceForDevice(BluetoothDevice device) {
        Int32 state = GetConnectionStatus(device);
        switch (state) {
            case BluetoothProfile.STATE_DISCONNECTED:
                return R::string::bluetooth_pan_profile_summary_use_for;

            case BluetoothProfile.STATE_CONNECTED:
                if (IsLocalRoleNap(device)) {
                    return R::string::bluetooth_pan_nap_profile_summary_connected;
                } else {
                    return R::string::bluetooth_pan_user_profile_summary_connected;
                }

            default:
                return Utils->GetConnectionStateSummary(state);
        }
    }

    public Int32 GetDrawableResource(BluetoothClass btClass) {
        return R.drawable.ic_bt_network_pan;
    }

    // Tethering direction determines UI strings.
    void SetLocalRole(BluetoothDevice device, Int32 role) {
        mDeviceRoleMap->Put(device, role);
    }

    Boolean IsLocalRoleNap(BluetoothDevice device) {
        if (mDeviceRoleMap->ContainsKey(device)) {
            return mDeviceRoleMap->Get(device) == BluetoothPan.LOCAL_NAP_ROLE;
        } else {
            return FALSE;
        }
    }

    protected void Finalize() {
        if (V) Logger::D(TAG, "Finalize()");
        if (mService != NULL) {
            try {
                BluetoothAdapter->GetDefaultAdapter()->CloseProfileProxy(BluetoothProfile.PAN, mService);
                mService = NULL;
            }catch (Throwable t) {
                Logger::W(TAG, "Error cleaning up PAN proxy", t);
            }
        }
    }
}
