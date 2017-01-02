//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

package com.android.settings.bluetooth;

using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Settings::IR;

/**
 * BluetoothPermissionRequest is a receiver to receive Bluetooth connection
 * access request.
 */
public class BluetoothPermissionRequest extends BroadcastReceiver {

    private static const String TAG = "BluetoothPermissionRequest";
    private static const Boolean DEBUG = Utils.V;
    private static const Int32 NOTIFICATION_ID = android.R.drawable.stat_sys_data_bluetooth;

    private static const String NOTIFICATION_TAG_PBAP = "Phonebook Access" ;
    private static const String NOTIFICATION_TAG_MAP = "Message Access";


    Context mContext;
    Int32 mRequestType;
    BluetoothDevice mDevice;
    String mReturnPackage = NULL;
    String mReturnClass = NULL;

    //@Override
    CARAPI OnReceive(Context context, Intent intent) {
        mContext = context;
        String action = intent->GetAction();

        if (DEBUG) Logger::D(TAG, "onReceive" + action);

        if (action->Equals(BluetoothDevice.ACTION_CONNECTION_ACCESS_REQUEST)) {
            // convert broadcast intent into activity intent (same action string)
            mDevice = intent->GetParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
            mRequestType = intent->GetIntExtra(BluetoothDevice.EXTRA_ACCESS_REQUEST_TYPE,
                                                 BluetoothDevice.REQUEST_TYPE_PROFILE_CONNECTION);
            mReturnPackage = intent->GetStringExtra(BluetoothDevice.EXTRA_PACKAGE_NAME);
            mReturnClass = intent->GetStringExtra(BluetoothDevice.EXTRA_CLASS_NAME);

            if (DEBUG) Logger::D(TAG, "onReceive request type: " + mRequestType + " return "
                    + mReturnPackage + "," + mReturnClass);

            // Even if the user has already made the choice, Bluetooth still may not know that if
            // the user preference data have not been migrated from Settings app's shared
            // preferences to Bluetooth app's. In that case, Bluetooth app broadcasts an
            // ACTION_CONNECTION_ACCESS_REQUEST intent to ask to Settings app.
            //
            // If that happens, 'CheckUserChoice()' here will do migration because it finds or
            // creates a 'CachedBluetoothDevice' object for the device.
            //
            // After migration is done, 'CheckUserChoice()' replies to the request by sending an
            // ACTION_CONNECTION_ACCESS_REPLY intent. And we don't need to start permission activity
            // dialog or notification.
            if (CheckUserChoice()) {
                return;
            }

            Intent connectionAccessIntent = new Intent(action);
            connectionAccessIntent->SetClass(context, BluetoothPermissionActivity.class);
            // We use the FLAG_ACTIVITY_MULTIPLE_TASK since we can have multiple concurrent access
            // requests.
            connectionAccessIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
                    | IIntent::FLAG_ACTIVITY_MULTIPLE_TASK);
            // This is needed to create two pending intents to the same activity. The value is not
            // used in the activity.
            connectionAccessIntent->SetType(Integer->ToString(mRequestType));
            connectionAccessIntent->PutExtra(BluetoothDevice.EXTRA_ACCESS_REQUEST_TYPE,
                                            mRequestType);
            connectionAccessIntent->PutExtra(BluetoothDevice.EXTRA_DEVICE, mDevice);
            connectionAccessIntent->PutExtra(BluetoothDevice.EXTRA_PACKAGE_NAME, mReturnPackage);
            connectionAccessIntent->PutExtra(BluetoothDevice.EXTRA_CLASS_NAME, mReturnClass);

            String deviceAddress = mDevice != NULL ? mDevice->GetAddress() : NULL;
            String title = NULL;
            String message = NULL;
            PowerManager powerManager =
                (PowerManager) context->GetSystemService(Context.POWER_SERVICE);

            if (powerManager->IsScreenOn()
                    && LocalBluetoothPreferences->ShouldShowDialogInForeground(
                            context, deviceAddress)) {
                context->StartActivity(connectionAccessIntent);
            } else {
                // Put up a notification that leads to the dialog

                // Create an intent triggered by clicking on the
                // "Clear All Notifications" button

                Intent deleteIntent = new Intent(BluetoothDevice.ACTION_CONNECTION_ACCESS_REPLY);
                deleteIntent->PutExtra(BluetoothDevice.EXTRA_DEVICE, mDevice);
                deleteIntent->PutExtra(BluetoothDevice.EXTRA_CONNECTION_ACCESS_RESULT,
                        BluetoothDevice.CONNECTION_ACCESS_NO);
                deleteIntent->PutExtra(BluetoothDevice.EXTRA_ACCESS_REQUEST_TYPE, mRequestType);
                String deviceName = mDevice != NULL ? mDevice->GetAliasName() : NULL;
                switch (mRequestType) {
                    case BluetoothDevice.REQUEST_TYPE_PHONEBOOK_ACCESS:
                        title = context->GetString(R::string::bluetooth_phonebook_request);
                        message = context->GetString(R::string::bluetooth_pb_acceptance_dialog_text,
                                deviceName, deviceName);
                        break;
                    case BluetoothDevice.REQUEST_TYPE_MESSAGE_ACCESS:
                        title = context->GetString(R::string::bluetooth_map_request);
                        message = context->GetString(R::string::bluetooth_map_acceptance_dialog_text,
                                deviceName, deviceName);
                        break;
                    default:
                        title = context->GetString(R::string::bluetooth_connection_permission_request);
                        message = context->GetString(R::string::bluetooth_connection_dialog_text,
                                deviceName, deviceName);
                        break;
                }
                Notification notification = new Notification->Builder(context)
                        .SetContentTitle(title)
                        .SetTicker(message)
                        .SetContentText(message)
                        .SetSmallIcon(android.R.drawable.stat_sys_data_bluetooth)
                        .SetAutoCancel(TRUE)
                        .SetPriority(Notification.PRIORITY_MAX)
                        .SetOnlyAlertOnce(FALSE)
                        .SetDefaults(Notification.DEFAULT_ALL)
                        .SetContentIntent(PendingIntent->GetActivity(context, 0,
                                connectionAccessIntent, 0))
                        .SetDeleteIntent(PendingIntent->GetBroadcast(context, 0, deleteIntent, 0))
                        .SetColor(context->GetResources()->GetColor(
                                R::Color::system_notification_accent_color))
                        .Build();

                notification.flags |= Notification.FLAG_NO_CLEAR; // Cannot be set with the builder.

                NotificationManager notificationManager =
                    (NotificationManager) context->GetSystemService(Context.NOTIFICATION_SERVICE);

                notificationManager->Notify(GetNotificationTag(mRequestType), NOTIFICATION_ID,
                        notification);
            }
        } else if (action->Equals(BluetoothDevice.ACTION_CONNECTION_ACCESS_CANCEL)) {
            // Remove the notification
            NotificationManager manager = (NotificationManager) context
                .GetSystemService(Context.NOTIFICATION_SERVICE);
            mRequestType = intent->GetIntExtra(BluetoothDevice.EXTRA_ACCESS_REQUEST_TYPE,
                                        BluetoothDevice.REQUEST_TYPE_PHONEBOOK_ACCESS);
            manager->Cancel(GetNotificationTag(mRequestType), NOTIFICATION_ID);
        }
    }

    private String GetNotificationTag(Int32 requestType) {
        If(requestType == BluetoothDevice.REQUEST_TYPE_PHONEBOOK_ACCESS) {
            return NOTIFICATION_TAG_PBAP;
        } else If(mRequestType == BluetoothDevice.REQUEST_TYPE_MESSAGE_ACCESS) {
            return NOTIFICATION_TAG_MAP;
        }
        return NULL;
    }

    /**
     * @return TRUE user had made a choice, this method replies to the request according
     *              to user's previous decision
     *         FALSE user hadnot made any choice on this device
     */
    private Boolean CheckUserChoice() {
        Boolean processed = FALSE;

        // ignore if it is something else than phonebook/message settings it wants us to remember
        if (mRequestType != BluetoothDevice.REQUEST_TYPE_PHONEBOOK_ACCESS
                && mRequestType != BluetoothDevice.REQUEST_TYPE_MESSAGE_ACCESS) {
            if (DEBUG) Logger::D(TAG, "CheckUserChoice(): Unknown RequestType " + mRequestType);
            return processed;
        }

        LocalBluetoothManager bluetoothManager = LocalBluetoothManager->GetInstance(mContext);
        CachedBluetoothDeviceManager cachedDeviceManager =
            bluetoothManager->GetCachedDeviceManager();
        CachedBluetoothDevice cachedDevice = cachedDeviceManager->FindDevice(mDevice);
        if (cachedDevice == NULL) {
            cachedDevice = cachedDeviceManager->AddDevice(bluetoothManager->GetBluetoothAdapter(),
                bluetoothManager->GetProfileManager(), mDevice);
        }

        String intentName = BluetoothDevice.ACTION_CONNECTION_ACCESS_REPLY;

        if (mRequestType == BluetoothDevice.REQUEST_TYPE_PHONEBOOK_ACCESS) {
            Int32 phonebookPermission = cachedDevice->GetPhonebookPermissionChoice();

            if (phonebookPermission == CachedBluetoothDevice.ACCESS_UNKNOWN) {
                // Leave 'processed' as FALSE.
            } else if (phonebookPermission == CachedBluetoothDevice.ACCESS_ALLOWED) {
                SendReplyIntentToReceiver(TRUE);
                processed = TRUE;
            } else if (phonebookPermission == CachedBluetoothDevice.ACCESS_REJECTED) {
                SendReplyIntentToReceiver(FALSE);
                processed = TRUE;
            } else {
                Logger::E(TAG, "Bad phonebookPermission: " + phonebookPermission);
            }
        } else if (mRequestType == BluetoothDevice.REQUEST_TYPE_MESSAGE_ACCESS) {
            Int32 messagePermission = cachedDevice->GetMessagePermissionChoice();

            if (messagePermission == CachedBluetoothDevice.ACCESS_UNKNOWN) {
                // Leave 'processed' as FALSE.
            } else if (messagePermission == CachedBluetoothDevice.ACCESS_ALLOWED) {
                SendReplyIntentToReceiver(TRUE);
                processed = TRUE;
            } else if (messagePermission == CachedBluetoothDevice.ACCESS_REJECTED) {
                SendReplyIntentToReceiver(FALSE);
                processed = TRUE;
            } else {
                Logger::E(TAG, "Bad messagePermission: " + messagePermission);
            }
        }
        if (DEBUG) Logger::D(TAG,"CheckUserChoice(): returning " + processed);
        return processed;
    }

    private void SendReplyIntentToReceiver(final Boolean allowed) {
        Intent intent = new Intent(BluetoothDevice.ACTION_CONNECTION_ACCESS_REPLY);

        if (mReturnPackage != NULL && mReturnClass != NULL) {
            intent->SetClassName(mReturnPackage, mReturnClass);
        }

        intent->PutExtra(BluetoothDevice.EXTRA_CONNECTION_ACCESS_RESULT,
                        allowed ? BluetoothDevice.CONNECTION_ACCESS_YES
                                : BluetoothDevice.CONNECTION_ACCESS_NO);
        intent->PutExtra(BluetoothDevice.EXTRA_DEVICE, mDevice);
        intent->PutExtra(BluetoothDevice.EXTRA_ACCESS_REQUEST_TYPE, mRequestType);
        mContext->SendBroadcast(intent, Manifest::permission::BLUETOOTH_ADMIN);
    }
}
