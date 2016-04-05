

package com.android.settings.bluetooth;

using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::ILog;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IList;

/**
 * CachedBluetoothDeviceManager manages the set of remote Bluetooth devices.
 */
final class CachedBluetoothDeviceManager {
    private static const String TAG = "CachedBluetoothDeviceManager";
    private static const Boolean DEBUG = Utils.D;

    private Context mContext;
    private final List<CachedBluetoothDevice> mCachedDevices =
            new ArrayList<CachedBluetoothDevice>();

    CachedBluetoothDeviceManager(Context context) {
        mContext = context;
    }

    public synchronized Collection<CachedBluetoothDevice> GetCachedDevicesCopy() {
        return new ArrayList<CachedBluetoothDevice>(mCachedDevices);
    }

    public static Boolean OnDeviceDisappeared(CachedBluetoothDevice cachedDevice) {
        cachedDevice->SetVisible(FALSE);
        return cachedDevice->GetBondState() == BluetoothDevice.BOND_NONE;
    }

    CARAPI OnDeviceNameUpdated(BluetoothDevice device) {
        CachedBluetoothDevice cachedDevice = FindDevice(device);
        if (cachedDevice != NULL) {
            cachedDevice->RefreshName();
        }
    }

    /**
     * Search for existing {@link CachedBluetoothDevice} or return NULL
     * if this device isn't in the cache. Use {@link #addDevice}
     * to create and return a new {@link CachedBluetoothDevice} for
     * a newly discovered {@link BluetoothDevice}.
     *
     * @param device the address of the Bluetooth device
     * @return the cached device object for this device, or NULL if it has
     *   not been previously seen
     */
    CachedBluetoothDevice FindDevice(BluetoothDevice device) {
        for (CachedBluetoothDevice cachedDevice : mCachedDevices) {
            if (cachedDevice->GetDevice()->Equals(device)) {
                return cachedDevice;
            }
        }
        return NULL;
    }

    /**
     * Create and return a new {@link CachedBluetoothDevice}. This assumes
     * that {@link #findDevice} has already been called and returned NULL.
     * @param device the address of the new Bluetooth device
     * @return the newly created CachedBluetoothDevice object
     */
    CachedBluetoothDevice AddDevice(LocalBluetoothAdapter adapter,
            LocalBluetoothProfileManager profileManager,
            BluetoothDevice device) {
        CachedBluetoothDevice newDevice = new CachedBluetoothDevice(mContext, adapter,
            profileManager, device);
        synchronized(mCachedDevices) {
            mCachedDevices->Add(newDevice);
        }
        return newDevice;
    }

    /**
     * Attempts to get the name of a remote device, otherwise returns the address.
     *
     * @param device The remote device.
     * @return The name, or if unavailable, the address.
     */
    public String GetName(BluetoothDevice device) {
        CachedBluetoothDevice cachedDevice = FindDevice(device);
        if (cachedDevice != NULL) {
            return cachedDevice->GetName();
        }

        String name = device->GetAliasName();
        if (name != NULL) {
            return name;
        }

        return device->GetAddress();
    }

    public synchronized void ClearNonBondedDevices() {
        for (Int32 i = mCachedDevices->Size() - 1; i >= 0; i--) {
            CachedBluetoothDevice cachedDevice = mCachedDevices->Get(i);
            if (cachedDevice->GetBondState() != BluetoothDevice.BOND_BONDED) {
                mCachedDevices->Remove(i);
            }
        }
    }

    public synchronized void OnScanningStateChanged(Boolean started) {
        if (!started) return;

        // If starting a new scan, clear old visibility
        // Iterate in reverse order since devices may be removed.
        for (Int32 i = mCachedDevices->Size() - 1; i >= 0; i--) {
            CachedBluetoothDevice cachedDevice = mCachedDevices->Get(i);
            cachedDevice->SetVisible(FALSE);
        }
    }

    public synchronized void OnBtClassChanged(BluetoothDevice device) {
        CachedBluetoothDevice cachedDevice = FindDevice(device);
        if (cachedDevice != NULL) {
            cachedDevice->RefreshBtClass();
        }
    }

    public synchronized void OnUuidChanged(BluetoothDevice device) {
        CachedBluetoothDevice cachedDevice = FindDevice(device);
        if (cachedDevice != NULL) {
            cachedDevice->OnUuidChanged();
        }
    }

    public synchronized void OnBluetoothStateChanged(Int32 bluetoothState) {
        // When Bluetooth is turning off, we need to clear the non-bonded devices
        // Otherwise, they end up showing up on the next BT enable
        if (bluetoothState == BluetoothAdapter.STATE_TURNING_OFF) {
            for (Int32 i = mCachedDevices->Size() - 1; i >= 0; i--) {
                CachedBluetoothDevice cachedDevice = mCachedDevices->Get(i);
                if (cachedDevice->GetBondState() != BluetoothDevice.BOND_BONDED) {
                    cachedDevice->SetVisible(FALSE);
                    mCachedDevices->Remove(i);
                } else {
                    // For bonded devices, we need to clear the connection status so that
                    // when BT is enabled next time, device connection status shall be retrieved
                    // by making a binder call.
                    cachedDevice->ClearProfileConnectionState();
                }
            }
        }
    }
    private void Log(String msg) {
        if (DEBUG) {
            Logger::D(TAG, msg);
        }
    }
}
