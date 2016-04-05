

package com.android.settings.bluetooth;

/**
 * BluetoothCallback provides a callback interface for the settings
 * UI to receive events from {@link BluetoothEventManager}.
 */
interface BluetoothCallback {
    void OnBluetoothStateChanged(Int32 bluetoothState);
    void OnScanningStateChanged(Boolean started);
    void OnDeviceAdded(CachedBluetoothDevice cachedDevice);
    void OnDeviceDeleted(CachedBluetoothDevice cachedDevice);
    void OnDeviceBondStateChanged(CachedBluetoothDevice cachedDevice, Int32 bondState);
}
