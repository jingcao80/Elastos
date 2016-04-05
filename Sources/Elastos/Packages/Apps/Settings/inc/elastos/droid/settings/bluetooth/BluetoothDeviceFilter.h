

package com.android.settings.bluetooth;

using Elastos::Droid::Bluetooth::IBluetoothClass;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Bluetooth::IBluetoothUuid;
using Elastos::Droid::Os::IParcelUuid;
using Elastos::Droid::Utility::ILog;

/**
 * BluetoothDeviceFilter contains a static method that returns a
 * Filter object that returns whether or not the BluetoothDevice
 * passed to it matches the specified filter type constant from
 * {@link android.bluetooth.BluetoothDevicePicker}.
 */
final class BluetoothDeviceFilter {
    private static const String TAG = "BluetoothDeviceFilter";

    /** The filter interface to external classes. */
    interface Filter {
        Boolean Matches(BluetoothDevice device);
    }

    /** All filter singleton (referenced directly). */
    static const Filter ALL_FILTER = new AllFilter();

    /** Bonded devices only filter (referenced directly). */
    static const Filter BONDED_DEVICE_FILTER = new BondedDeviceFilter();

    /** Unbonded devices only filter (referenced directly). */
    static const Filter UNBONDED_DEVICE_FILTER = new UnbondedDeviceFilter();

    /** Table of singleton filter objects. */
    private static const Filter[] FILTERS = {
            ALL_FILTER,             // FILTER_TYPE_ALL
            new AudioFilter(),      // FILTER_TYPE_AUDIO
            new TransferFilter(),   // FILTER_TYPE_TRANSFER
            new PanuFilter(),       // FILTER_TYPE_PANU
            new NapFilter()         // FILTER_TYPE_NAP
    };

    /** Private constructor. */
    private BluetoothDeviceFilter() {
    }

    /**
     * Returns the singleton {@link Filter} object for the specified type,
     * or {@link #ALL_FILTER} if the type value is out of range.
     *
     * @param filterType a constant from BluetoothDevicePicker
     * @return a singleton object implementing the {@link Filter} interface.
     */
    static Filter GetFilter(Int32 filterType) {
        if (filterType >= 0 && filterType < FILTERS.length) {
            return FILTERS[filterType];
        } else {
            Logger::W(TAG, "Invalid filter type " + filterType + " for device picker");
            return ALL_FILTER;
        }
    }

    /** Filter that matches all devices. */
    private static const class AllFilter implements Filter {
        public Boolean Matches(BluetoothDevice device) {
            return TRUE;
        }
    }

    /** Filter that matches only bonded devices. */
    private static const class BondedDeviceFilter implements Filter {
        public Boolean Matches(BluetoothDevice device) {
            return device->GetBondState() == BluetoothDevice.BOND_BONDED;
        }
    }

    /** Filter that matches only unbonded devices. */
    private static const class UnbondedDeviceFilter implements Filter {
        public Boolean Matches(BluetoothDevice device) {
            return device->GetBondState() != BluetoothDevice.BOND_BONDED;
        }
    }

    /** Parent class of filters based on UUID and/or Bluetooth class. */
    private abstract static class ClassUuidFilter implements Filter {
        abstract Boolean Matches(ParcelUuid[] uuids, BluetoothClass btClass);

        public Boolean Matches(BluetoothDevice device) {
            return Matches(device->GetUuids(), device->GetBluetoothClass());
        }
    }

    /** Filter that matches devices that support AUDIO profiles. */
    private static const class AudioFilter extends ClassUuidFilter {
        //@Override
        Boolean Matches(ParcelUuid[] uuids, BluetoothClass btClass) {
            if (uuids != NULL) {
                if (BluetoothUuid->ContainsAnyUuid(uuids, A2dpProfile.SINK_UUIDS)) {
                    return TRUE;
                }
                if (BluetoothUuid->ContainsAnyUuid(uuids, HeadsetProfile.UUIDS)) {
                    return TRUE;
                }
            } else if (btClass != NULL) {
                if (btClass->DoesClassMatch(BluetoothClass.PROFILE_A2DP) ||
                        btClass->DoesClassMatch(BluetoothClass.PROFILE_HEADSET)) {
                    return TRUE;
                }
            }
            return FALSE;
        }
    }

    /** Filter that matches devices that support Object Transfer. */
    private static const class TransferFilter extends ClassUuidFilter {
        //@Override
        Boolean Matches(ParcelUuid[] uuids, BluetoothClass btClass) {
            if (uuids != NULL) {
                if (BluetoothUuid->IsUuidPresent(uuids, BluetoothUuid.ObexObjectPush)) {
                    return TRUE;
                }
            }
            return btClass != NULL
                    && btClass->DoesClassMatch(BluetoothClass.PROFILE_OPP);
        }
    }

    /** Filter that matches devices that support PAN User (PANU) profile. */
    private static const class PanuFilter extends ClassUuidFilter {
        //@Override
        Boolean Matches(ParcelUuid[] uuids, BluetoothClass btClass) {
            if (uuids != NULL) {
                if (BluetoothUuid->IsUuidPresent(uuids, BluetoothUuid.PANU)) {
                    return TRUE;
                }
            }
            return btClass != NULL
                    && btClass->DoesClassMatch(BluetoothClass.PROFILE_PANU);
        }
    }

    /** Filter that matches devices that support NAP profile. */
    private static const class NapFilter extends ClassUuidFilter {
        //@Override
        Boolean Matches(ParcelUuid[] uuids, BluetoothClass btClass) {
            if (uuids != NULL) {
                if (BluetoothUuid->IsUuidPresent(uuids, BluetoothUuid.NAP)) {
                    return TRUE;
                }
            }
            return btClass != NULL
                    && btClass->DoesClassMatch(BluetoothClass.PROFILE_NAP);
        }
    }
}
