

package com.android.settings.bluetooth;

using Elastos::Droid::Settings::IR;

using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothClass;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Utility::ILog;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;

/**
 * BluetoothEventManager receives broadcasts and callbacks from the Bluetooth
 * API and dispatches the event on the UI thread to the right class in the
 * Settings.
 */
final class BluetoothEventManager {
    private static const String TAG = "BluetoothEventManager";

    private final LocalBluetoothAdapter mLocalAdapter;
    private final CachedBluetoothDeviceManager mDeviceManager;
    private LocalBluetoothProfileManager mProfileManager;
    private final IntentFilter mAdapterIntentFilter, mProfileIntentFilter;
    private final Map<String, Handler> mHandlerMap;
    private Context mContext;

    private final Collection<BluetoothCallback> mCallbacks =
            new ArrayList<BluetoothCallback>();

    interface Handler {
        void OnReceive(Context context, Intent intent, BluetoothDevice device);
    }

    void AddHandler(String action, Handler handler) {
        mHandlerMap->Put(action, handler);
        mAdapterIntentFilter->AddAction(action);
    }

    void AddProfileHandler(String action, Handler handler) {
        mHandlerMap->Put(action, handler);
        mProfileIntentFilter->AddAction(action);
    }

    // Set profile manager after construction due to circular dependency
    void SetProfileManager(LocalBluetoothProfileManager manager) {
        mProfileManager = manager;
    }

    BluetoothEventManager(LocalBluetoothAdapter adapter,
            CachedBluetoothDeviceManager deviceManager, Context context) {
        mLocalAdapter = adapter;
        mDeviceManager = deviceManager;
        mAdapterIntentFilter = new IntentFilter();
        mProfileIntentFilter = new IntentFilter();
        mHandlerMap = new HashMap<String, Handler>();
        mContext = context;

        // Bluetooth on/off broadcasts
        AddHandler(BluetoothAdapter.ACTION_STATE_CHANGED, new AdapterStateChangedHandler());

        // Discovery broadcasts
        AddHandler(BluetoothAdapter.ACTION_DISCOVERY_STARTED, new ScanningStateChangedHandler(TRUE));
        AddHandler(BluetoothAdapter.ACTION_DISCOVERY_FINISHED, new ScanningStateChangedHandler(FALSE));
        AddHandler(BluetoothDevice.ACTION_FOUND, new DeviceFoundHandler());
        AddHandler(BluetoothDevice.ACTION_DISAPPEARED, new DeviceDisappearedHandler());
        AddHandler(BluetoothDevice.ACTION_NAME_CHANGED, new NameChangedHandler());

        // Pairing broadcasts
        AddHandler(BluetoothDevice.ACTION_BOND_STATE_CHANGED, new BondStateChangedHandler());
        AddHandler(BluetoothDevice.ACTION_PAIRING_CANCEL, new PairingCancelHandler());

        // Fine-grained state broadcasts
        AddHandler(BluetoothDevice.ACTION_CLASS_CHANGED, new ClassChangedHandler());
        AddHandler(BluetoothDevice.ACTION_UUID, new UuidChangedHandler());

        // Dock event broadcasts
        AddHandler(IIntent::ACTION_DOCK_EVENT, new DockEventHandler());

        mContext->RegisterReceiver(mBroadcastReceiver, mAdapterIntentFilter);
    }

    void RegisterProfileIntentReceiver() {
        mContext->RegisterReceiver(mBroadcastReceiver, mProfileIntentFilter);
    }

    /** Register to start receiving callbacks for Bluetooth events. */
    void RegisterCallback(BluetoothCallback callback) {
        {    AutoLock syncLock(mCallbacks);
            mCallbacks->Add(callback);
        }
    }

    /** Unregister to stop receiving callbacks for Bluetooth events. */
    void UnregisterCallback(BluetoothCallback callback) {
        {    AutoLock syncLock(mCallbacks);
            mCallbacks->Remove(callback);
        }
    }

    // This can't be called from a broadcast receiver where the filter is set in the Manifest.
    private static String GetDockedDeviceAddress(Context context) {
        // This works only because these broadcast intents are "sticky"
        Intent i = context->RegisterReceiver(NULL, new IntentFilter(IIntent::ACTION_DOCK_EVENT));
        if (i != NULL) {
            Int32 state = i->GetIntExtra(Intent.EXTRA_DOCK_STATE, Intent.EXTRA_DOCK_STATE_UNDOCKED);
            if (state != Intent.EXTRA_DOCK_STATE_UNDOCKED) {
                BluetoothDevice device = i->GetParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                if (device != NULL) {
                    return device->GetAddress();
                }
            }
        }
        return NULL;
    }

    private final BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            BluetoothDevice device = intent
                    .GetParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

            Handler handler = mHandlerMap->Get(action);
            if (handler != NULL) {
                handler->OnReceive(context, intent, device);
            }
        }
    };

    private class AdapterStateChangedHandler implements Handler {
        CARAPI OnReceive(Context context, Intent intent,
                BluetoothDevice device) {
            Int32 state = intent->GetIntExtra(BluetoothAdapter.EXTRA_STATE,
                                    BluetoothAdapter.ERROR);
            // update local profiles and get paired devices
            mLocalAdapter->SetBluetoothStateInt(state);
            // send callback to update UI and possibly start scanning
            {    AutoLock syncLock(mCallbacks);
                for (BluetoothCallback callback : mCallbacks) {
                    callback->OnBluetoothStateChanged(state);
                }
            }
            // Inform CachedDeviceManager that the adapter state has changed
            mDeviceManager->OnBluetoothStateChanged(state);
        }
    }

    private class ScanningStateChangedHandler implements Handler {
        private final Boolean mStarted;

        ScanningStateChangedHandler(Boolean started) {
            mStarted = started;
        }
        CARAPI OnReceive(Context context, Intent intent,
                BluetoothDevice device) {
            {    AutoLock syncLock(mCallbacks);
                for (BluetoothCallback callback : mCallbacks) {
                    callback->OnScanningStateChanged(mStarted);
                }
            }
            mDeviceManager->OnScanningStateChanged(mStarted);
            LocalBluetoothPreferences->PersistDiscoveringTimestamp(context);
        }
    }

    private class DeviceFoundHandler implements Handler {
        CARAPI OnReceive(Context context, Intent intent,
                BluetoothDevice device) {
            short rssi = intent->GetShortExtra(BluetoothDevice.EXTRA_RSSI, Short.MIN_VALUE);
            BluetoothClass btClass = intent->GetParcelableExtra(BluetoothDevice.EXTRA_CLASS);
            String name = intent->GetStringExtra(BluetoothDevice.EXTRA_NAME);
            // TODO Pick up UUID. They should be available for 2.1 devices.
            // Skip for now, there's a bluez problem and we are not getting uuids even for 2.1.
            CachedBluetoothDevice cachedDevice = mDeviceManager->FindDevice(device);
            if (cachedDevice == NULL) {
                cachedDevice = mDeviceManager->AddDevice(mLocalAdapter, mProfileManager, device);
                Logger::D(TAG, "DeviceFoundHandler created new CachedBluetoothDevice: "
                        + cachedDevice);
                // callback to UI to create Preference for new device
                DispatchDeviceAdded(cachedDevice);
            }
            cachedDevice->SetRssi(rssi);
            cachedDevice->SetBtClass(btClass);
            cachedDevice->SetNewName(name);
            cachedDevice->SetVisible(TRUE);
        }
    }

    private void DispatchDeviceAdded(CachedBluetoothDevice cachedDevice) {
        {    AutoLock syncLock(mCallbacks);
            for (BluetoothCallback callback : mCallbacks) {
                callback->OnDeviceAdded(cachedDevice);
            }
        }
    }

    private class DeviceDisappearedHandler implements Handler {
        CARAPI OnReceive(Context context, Intent intent,
                BluetoothDevice device) {
            CachedBluetoothDevice cachedDevice = mDeviceManager->FindDevice(device);
            if (cachedDevice == NULL) {
                Logger::W(TAG, "received ACTION_DISAPPEARED for an unknown device: " + device);
                return;
            }
            if (CachedBluetoothDeviceManager->OnDeviceDisappeared(cachedDevice)) {
                {    AutoLock syncLock(mCallbacks);
                    for (BluetoothCallback callback : mCallbacks) {
                        callback->OnDeviceDeleted(cachedDevice);
                    }
                }
            }
        }
    }

    private class NameChangedHandler implements Handler {
        CARAPI OnReceive(Context context, Intent intent,
                BluetoothDevice device) {
            mDeviceManager->OnDeviceNameUpdated(device);
        }
    }

    private class BondStateChangedHandler implements Handler {
        CARAPI OnReceive(Context context, Intent intent,
                BluetoothDevice device) {
            if (device == NULL) {
                Logger::E(TAG, "ACTION_BOND_STATE_CHANGED with no EXTRA_DEVICE");
                return;
            }
            Int32 bondState = intent->GetIntExtra(BluetoothDevice.EXTRA_BOND_STATE,
                                               BluetoothDevice.ERROR);
            CachedBluetoothDevice cachedDevice = mDeviceManager->FindDevice(device);
            if (cachedDevice == NULL) {
                Logger::W(TAG, "CachedBluetoothDevice for device " + device +
                        " not found, calling ReadPairedDevices().");
                if (!ReadPairedDevices()) {
                    Logger::E(TAG, "Got bonding state changed for " + device +
                            ", but we have no record of that device.");
                    return;
                }
                cachedDevice = mDeviceManager->FindDevice(device);
                if (cachedDevice == NULL) {
                    Logger::E(TAG, "Got bonding state changed for " + device +
                            ", but device not added in cache.");
                    return;
                }
            }

            {    AutoLock syncLock(mCallbacks);
                for (BluetoothCallback callback : mCallbacks) {
                    callback->OnDeviceBondStateChanged(cachedDevice, bondState);
                }
            }
            cachedDevice->OnBondingStateChanged(bondState);

            if (bondState == BluetoothDevice.BOND_NONE) {
                if (device->IsBluetoothDock()) {
                    // After a dock is unpaired, we will forget the settings
                    LocalBluetoothPreferences
                            .RemoveDockAutoConnectSetting(context, device->GetAddress());

                    // if the device is undocked, remove it from the list as well
                    if (!device->GetAddress()->Equals(GetDockedDeviceAddress(context))) {
                        cachedDevice->SetVisible(FALSE);
                    }
                }
                Int32 reason = intent->GetIntExtra(BluetoothDevice.EXTRA_REASON,
                        BluetoothDevice.ERROR);

                ShowUnbondMessage(context, cachedDevice->GetName(), reason);
            }
        }

        /**
         * Called when we have reached the unbonded state.
         *
         * @param reason one of the error reasons from
         *            BluetoothDevice.UNBOND_REASON_*
         */
        private void ShowUnbondMessage(Context context, String name, Int32 reason) {
            Int32 errorMsg;

            Switch(reason) {
            case BluetoothDevice.UNBOND_REASON_AUTH_FAILED:
                errorMsg = R::string::bluetooth_pairing_pin_error_message;
                break;
            case BluetoothDevice.UNBOND_REASON_AUTH_REJECTED:
                errorMsg = R::string::bluetooth_pairing_rejected_error_message;
                break;
            case BluetoothDevice.UNBOND_REASON_REMOTE_DEVICE_DOWN:
                errorMsg = R::string::bluetooth_pairing_device_down_error_message;
                break;
            case BluetoothDevice.UNBOND_REASON_DISCOVERY_IN_PROGRESS:
            case BluetoothDevice.UNBOND_REASON_AUTH_TIMEOUT:
            case BluetoothDevice.UNBOND_REASON_REPEATED_ATTEMPTS:
            case BluetoothDevice.UNBOND_REASON_REMOTE_AUTH_CANCELED:
                errorMsg = R::string::bluetooth_pairing_error_message;
                break;
            default:
                Logger::W(TAG, "showUnbondMessage: Not displaying any message for reason: " + reason);
                return;
            }
            Utils->ShowError(context, name, errorMsg);
        }
    }

    private class ClassChangedHandler implements Handler {
        CARAPI OnReceive(Context context, Intent intent,
                BluetoothDevice device) {
            mDeviceManager->OnBtClassChanged(device);
        }
    }

    private class UuidChangedHandler implements Handler {
        CARAPI OnReceive(Context context, Intent intent,
                BluetoothDevice device) {
            mDeviceManager->OnUuidChanged(device);
        }
    }

    private class PairingCancelHandler implements Handler {
        CARAPI OnReceive(Context context, Intent intent, BluetoothDevice device) {
            if (device == NULL) {
                Logger::E(TAG, "ACTION_PAIRING_CANCEL with no EXTRA_DEVICE");
                return;
            }
            Int32 errorMsg = R::string::bluetooth_pairing_error_message;
            CachedBluetoothDevice cachedDevice = mDeviceManager->FindDevice(device);
            Utils->ShowError(context, cachedDevice->GetName(), errorMsg);
        }
    }

    private class DockEventHandler implements Handler {
        CARAPI OnReceive(Context context, Intent intent, BluetoothDevice device) {
            // Remove if unpair device upon undocking
            Int32 anythingButUnDocked = Intent.EXTRA_DOCK_STATE_UNDOCKED + 1;
            Int32 state = intent->GetIntExtra(Intent.EXTRA_DOCK_STATE, anythingButUnDocked);
            if (state == Intent.EXTRA_DOCK_STATE_UNDOCKED) {
                if (device != NULL && device->GetBondState() == BluetoothDevice.BOND_NONE) {
                    CachedBluetoothDevice cachedDevice = mDeviceManager->FindDevice(device);
                    if (cachedDevice != NULL) {
                        cachedDevice->SetVisible(FALSE);
                    }
                }
            }
        }
    }
    Boolean ReadPairedDevices() {
        Set<BluetoothDevice> bondedDevices = mLocalAdapter->GetBondedDevices();
        if (bondedDevices == NULL) {
            return FALSE;
        }

        Boolean deviceAdded = FALSE;
        for (BluetoothDevice device : bondedDevices) {
            CachedBluetoothDevice cachedDevice = mDeviceManager->FindDevice(device);
            if (cachedDevice == NULL) {
                cachedDevice = mDeviceManager->AddDevice(mLocalAdapter, mProfileManager, device);
                DispatchDeviceAdded(cachedDevice);
                deviceAdded = TRUE;
            }
        }

        return deviceAdded;
    }
}
