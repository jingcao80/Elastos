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

using Elastos::Droid::App::IQueuedWork;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Utility::ILog;

/**
 * LocalBluetoothPreferences provides an interface to the preferences
 * related to Bluetooth.
 */
final class LocalBluetoothPreferences {
    private static const String TAG = "LocalBluetoothPreferences";
    private static const Boolean DEBUG = Utils.D;
    private static const String SHARED_PREFERENCES_NAME = "bluetooth_settings";

    // If a device was picked from the device picker or was in discoverable mode
    // in the last 60 seconds, show the pairing dialogs in foreground instead
    // of raising notifications
    private static const Int32 GRACE_PERIOD_TO_SHOW_DIALOGS_IN_FOREGROUND = 60 * 1000;

    private static const String KEY_DISCOVERING_TIMESTAMP = "last_discovering_time";

    private static const String KEY_LAST_SELECTED_DEVICE = "last_selected_device";

    private static const String KEY_LAST_SELECTED_DEVICE_TIME = "last_selected_device_time";

    private static const String KEY_DOCK_AUTO_CONNECT = "auto_connect_to_dock";

    private static const String KEY_DISCOVERABLE_END_TIMESTAMP = "discoverable_end_timestamp";

    private LocalBluetoothPreferences() {
    }

    private static SharedPreferences GetSharedPreferences(Context context) {
        return context->GetSharedPreferences(SHARED_PREFERENCES_NAME, Context.MODE_PRIVATE);
    }

    static Int64 GetDiscoverableEndTimestamp(Context context) {
        return GetSharedPreferences(context).GetLong(
                KEY_DISCOVERABLE_END_TIMESTAMP, 0);
    }

    static Boolean ShouldShowDialogInForeground(Context context,
            String deviceAddress) {
        LocalBluetoothManager manager = LocalBluetoothManager->GetInstance(context);
        if (manager == NULL) {
            If(DEBUG) Logger::V(TAG, "manager == NULL - do not show dialog.");
            return FALSE;
        }

        // If Bluetooth Settings is visible
        if (manager->IsForegroundActivity()) {
            return TRUE;
        }

        // If in appliance mode, do not show dialog in foreground.
        if ((context->GetResources()->GetConfiguration().uiMode &
                Configuration.UI_MODE_TYPE_APPLIANCE) == Configuration.UI_MODE_TYPE_APPLIANCE) {
            if (DEBUG) Logger::V(TAG, "in appliance mode - do not show dialog.");
            return FALSE;
        }

        Int64 currentTimeMillis = System->CurrentTimeMillis();
        SharedPreferences sharedPreferences = GetSharedPreferences(context);

        // If the device was in discoverABLE mode recently
        Int64 lastDiscoverableEndTime = sharedPreferences->GetLong(
                KEY_DISCOVERABLE_END_TIMESTAMP, 0);
        if ((lastDiscoverableEndTime + GRACE_PERIOD_TO_SHOW_DIALOGS_IN_FOREGROUND)
                > currentTimeMillis) {
            return TRUE;
        }

        // If the device was discoverING recently
        LocalBluetoothAdapter adapter = manager->GetBluetoothAdapter();
        if (adapter != NULL && adapter->IsDiscovering()) {
            return TRUE;
        } else if ((sharedPreferences->GetLong(KEY_DISCOVERING_TIMESTAMP, 0) +
                GRACE_PERIOD_TO_SHOW_DIALOGS_IN_FOREGROUND) > currentTimeMillis) {
            return TRUE;
        }

        // If the device was picked in the device picker recently
        if (deviceAddress != NULL) {
            String lastSelectedDevice = sharedPreferences->GetString(
                    KEY_LAST_SELECTED_DEVICE, NULL);

            if (deviceAddress->Equals(lastSelectedDevice)) {
                Int64 lastDeviceSelectedTime = sharedPreferences->GetLong(
                        KEY_LAST_SELECTED_DEVICE_TIME, 0);
                if ((lastDeviceSelectedTime + GRACE_PERIOD_TO_SHOW_DIALOGS_IN_FOREGROUND)
                        > currentTimeMillis) {
                    return TRUE;
                }
            }
        }
        if (DEBUG) Logger::V(TAG, "Found no reason to show the dialog - do not show dialog.");
        return FALSE;
    }

    static void PersistSelectedDeviceInPicker(Context context, String deviceAddress) {
        SharedPreferences.Editor editor = GetSharedPreferences(context).Edit();
        editor->PutString(KEY_LAST_SELECTED_DEVICE,
                deviceAddress);
        editor->PutLong(KEY_LAST_SELECTED_DEVICE_TIME,
                System->CurrentTimeMillis());
        editor->Apply();
    }

    static void PersistDiscoverableEndTimestamp(Context context, Int64 endTimestamp) {
        SharedPreferences.Editor editor = GetSharedPreferences(context).Edit();
        editor->PutLong(KEY_DISCOVERABLE_END_TIMESTAMP, endTimestamp);
        editor->Apply();
    }

    static void PersistDiscoveringTimestamp(final Context context) {
        // Load the shared preferences and edit it on a background
        // thread (but serialized!).
        QueuedWork->SingleThreadExecutor()->Submit(new Runnable() {
                CARAPI Run() {
                    SharedPreferences.Editor editor = GetSharedPreferences(context).Edit();
                    editor->PutLong(
                            KEY_DISCOVERING_TIMESTAMP,
                        System->CurrentTimeMillis());
                    editor->Apply();
                }
            });
    }

    static Boolean HasDockAutoConnectSetting(Context context, String addr) {
        return GetSharedPreferences(context).Contains(KEY_DOCK_AUTO_CONNECT + addr);
    }

    static Boolean GetDockAutoConnectSetting(Context context, String addr) {
        return GetSharedPreferences(context).GetBoolean(KEY_DOCK_AUTO_CONNECT + addr,
                FALSE);
    }

    static void SaveDockAutoConnectSetting(Context context, String addr, Boolean autoConnect) {
        SharedPreferences.Editor editor = GetSharedPreferences(context).Edit();
        editor->PutBoolean(KEY_DOCK_AUTO_CONNECT + addr, autoConnect);
        editor->Apply();
    }

    static void RemoveDockAutoConnectSetting(Context context, String addr) {
        SharedPreferences.Editor editor = GetSharedPreferences(context).Edit();
        editor->Remove(KEY_DOCK_AUTO_CONNECT + addr);
        editor->Apply();
    }
}
