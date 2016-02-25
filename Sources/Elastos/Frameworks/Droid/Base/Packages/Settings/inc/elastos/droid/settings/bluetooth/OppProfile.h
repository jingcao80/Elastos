

package com.android.settings.bluetooth;

using Elastos::Droid::Settings::IR;

using Elastos::Droid::Bluetooth::IBluetoothClass;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Bluetooth::IBluetoothProfile;

/**
 * OppProfile handles Bluetooth OPP.
 */
final class OppProfile implements LocalBluetoothProfile {

    static const String NAME = "OPP";

    // Order of this profile in device profiles list
    private static const Int32 ORDINAL = 2;

    public Boolean IsConnectable() {
        return FALSE;
    }

    public Boolean IsAutoConnectable() {
        return FALSE;
    }

    public Boolean Connect(BluetoothDevice device) {
        return FALSE;
    }

    public Boolean Disconnect(BluetoothDevice device) {
        return FALSE;
    }

    public Int32 GetConnectionStatus(BluetoothDevice device) {
        return BluetoothProfile.STATE_DISCONNECTED; // Settings app doesn't handle OPP
    }

    public Boolean IsPreferred(BluetoothDevice device) {
        return FALSE;
    }

    public Int32 GetPreferred(BluetoothDevice device) {
        return BluetoothProfile.PRIORITY_OFF; // Settings app doesn't handle OPP
    }

    CARAPI SetPreferred(BluetoothDevice device, Boolean preferred) {
    }

    public Boolean IsProfileReady() {
        return TRUE;
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
        return R::string::bluetooth_profile_opp;
    }

    public Int32 GetSummaryResourceForDevice(BluetoothDevice device) {
        return 0;   // OPP profile not displayed in UI
    }

    public Int32 GetDrawableResource(BluetoothClass btClass) {
        return 0;   // no icon for OPP
    }
}
