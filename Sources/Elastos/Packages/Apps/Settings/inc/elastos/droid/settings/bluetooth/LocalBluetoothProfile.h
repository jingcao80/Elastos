

package com.android.settings.bluetooth;

using Elastos::Droid::Bluetooth::IBluetoothClass;
using Elastos::Droid::Bluetooth::IBluetoothDevice;

/**
 * LocalBluetoothProfile is an interface defining the basic
 * functionality related to a Bluetooth profile.
 */
interface LocalBluetoothProfile {

    /**
     * Returns TRUE if the user can initiate a connection, FALSE otherwise.
     */
    Boolean IsConnectable();

    /**
     * Returns TRUE if the user can enable auto connection for this profile.
     */
    Boolean IsAutoConnectable();

    Boolean Connect(BluetoothDevice device);

    Boolean Disconnect(BluetoothDevice device);

    Int32 GetConnectionStatus(BluetoothDevice device);

    Boolean IsPreferred(BluetoothDevice device);

    Int32 GetPreferred(BluetoothDevice device);

    void SetPreferred(BluetoothDevice device, Boolean preferred);

    Boolean IsProfileReady();

    /** Display order for device profile settings. */
    Int32 GetOrdinal();

    /**
     * Returns the string resource ID for the localized name for this profile.
     * @param device the Bluetooth device (to distinguish between PAN roles)
     */
    Int32 GetNameResource(BluetoothDevice device);

    /**
     * Returns the string resource ID for the summary text for this profile
     * for the specified device, e.g. "Use for media audio" or
     * "Connected to media audio".
     * @param device the device to query for profile connection status
     * @return a string resource ID for the profile summary text
     */
    Int32 GetSummaryResourceForDevice(BluetoothDevice device);

    Int32 GetDrawableResource(BluetoothClass btClass);
}
