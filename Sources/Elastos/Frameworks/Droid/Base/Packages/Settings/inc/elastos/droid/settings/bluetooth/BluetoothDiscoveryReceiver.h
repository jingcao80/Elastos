

package com.android.settings.bluetooth;

using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Utility::ILog;

/**
 * BluetoothDiscoveryReceiver updates a timestamp when the
 * Bluetooth adapter starts or finishes discovery mode. This
 * is used to decide whether to open an alert dialog or
 * create a notification when we receive a pairing request.
 *
 * <p>Note that the discovery start/finish intents are also handled
 * by {@link BluetoothEventManager} to update the UI, if visible.
 */
public class BluetoothDiscoveryReceiver extends BroadcastReceiver {
    private static const String TAG = "BluetoothDiscoveryReceiver";

    //@Override
    CARAPI OnReceive(Context context, Intent intent) {
        String action = intent->GetAction();
        Logger::V(TAG, "Received: " + action);

        if (action->Equals(BluetoothAdapter.ACTION_DISCOVERY_STARTED) ||
                action->Equals(BluetoothAdapter.ACTION_DISCOVERY_FINISHED)) {
            LocalBluetoothPreferences->PersistDiscoveringTimestamp(context);
        }
    }
}
