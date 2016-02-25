

package com.android.settings.bluetooth;

using Elastos::Droid::Settings::IR;

using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Os::IPowerManager;

/**
 * BluetoothPairingRequest is a receiver for any Bluetooth pairing request. It
 * checks if the Bluetooth Settings is currently visible and brings up the PIN, the passkey or a
 * confirmation entry dialog. Otherwise it puts a Notification in the status bar, which can
 * be clicked to bring up the Pairing entry dialog.
 */
public class BluetoothPairingRequest extends BroadcastReceiver {

    private static const Int32 NOTIFICATION_ID = android.R.drawable.stat_sys_data_bluetooth;

    //@Override
    CARAPI OnReceive(Context context, Intent intent) {
        String action = intent->GetAction();
        if (action->Equals(BluetoothDevice.ACTION_PAIRING_REQUEST)) {
            // convert broadcast intent into activity intent (same action string)
            BluetoothDevice device =
                    intent->GetParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
            Int32 type = intent->GetIntExtra(BluetoothDevice.EXTRA_PAIRING_VARIANT,
                    BluetoothDevice.ERROR);
            Intent pairingIntent = new Intent();
            pairingIntent->SetClass(context, BluetoothPairingDialog.class);
            pairingIntent->PutExtra(BluetoothDevice.EXTRA_DEVICE, device);
            pairingIntent->PutExtra(BluetoothDevice.EXTRA_PAIRING_VARIANT, type);
            if (type == BluetoothDevice.PAIRING_VARIANT_PASSKEY_CONFIRMATION ||
                    type == BluetoothDevice.PAIRING_VARIANT_DISPLAY_PASSKEY ||
                    type == BluetoothDevice.PAIRING_VARIANT_DISPLAY_PIN) {
                Int32 pairingKey = intent->GetIntExtra(BluetoothDevice.EXTRA_PAIRING_KEY,
                        BluetoothDevice.ERROR);
                pairingIntent->PutExtra(BluetoothDevice.EXTRA_PAIRING_KEY, pairingKey);
            }
            pairingIntent->SetAction(BluetoothDevice.ACTION_PAIRING_REQUEST);
            pairingIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);

            PowerManager powerManager =
                    (PowerManager)context->GetSystemService(Context.POWER_SERVICE);
            String deviceAddress = device != NULL ? device->GetAddress() : NULL;
            if (powerManager->IsScreenOn() &&
                    LocalBluetoothPreferences->ShouldShowDialogInForeground(context, deviceAddress)) {
                // Since the screen is on and the BT-related activity is in the foreground,
                // just open the dialog
                context->StartActivity(pairingIntent);
            } else {
                // Put up a notification that leads to the dialog
                Resources res = context->GetResources();
                Notification.Builder builder = new Notification->Builder(context)
                        .SetSmallIcon(android.R.drawable.stat_sys_data_bluetooth)
                        .SetTicker(res->GetString(R::string::bluetooth_notif_ticker));

                PendingIntent pending = PendingIntent->GetActivity(context, 0,
                        pairingIntent, PendingIIntent::FLAG_ONE_SHOT);

                String name = intent->GetStringExtra(BluetoothDevice.EXTRA_NAME);
                if (TextUtils->IsEmpty(name)) {
                    name = device != NULL ? device->GetAliasName() :
                            context->GetString(android.R::string::unknownName);
                }

                builder->SetContentTitle(res->GetString(R::string::bluetooth_notif_title))
                        .SetContentText(res->GetString(R::string::bluetooth_notif_message, name))
                        .SetContentIntent(pending)
                        .SetAutoCancel(TRUE)
                        .SetDefaults(Notification.DEFAULT_SOUND)
                        .SetColor(res->GetColor(
                                R::Color::system_notification_accent_color));

                NotificationManager manager = (NotificationManager)
                        context->GetSystemService(Context.NOTIFICATION_SERVICE);
                manager->Notify(NOTIFICATION_ID, builder->GetNotification());
            }

        } else if (action->Equals(BluetoothDevice.ACTION_PAIRING_CANCEL)) {

            // Remove the notification
            NotificationManager manager = (NotificationManager) context
                    .GetSystemService(Context.NOTIFICATION_SERVICE);
            manager->Cancel(NOTIFICATION_ID);

        } else if (BluetoothDevice.ACTION_BOND_STATE_CHANGED->Equals(action)) {
            Int32 bondState = intent->GetIntExtra(BluetoothDevice.EXTRA_BOND_STATE,
                    BluetoothDevice.ERROR);
            Int32 oldState = intent->GetIntExtra(BluetoothDevice.EXTRA_PREVIOUS_BOND_STATE,
                    BluetoothDevice.ERROR);
            If((oldState == BluetoothDevice.BOND_BONDING) &&
                    (bondState == BluetoothDevice.BOND_NONE)) {
                // Remove the notification
                NotificationManager manager = (NotificationManager) context
                    .GetSystemService(Context.NOTIFICATION_SERVICE);
                manager->Cancel(NOTIFICATION_ID);
            }
        }
    }
}
