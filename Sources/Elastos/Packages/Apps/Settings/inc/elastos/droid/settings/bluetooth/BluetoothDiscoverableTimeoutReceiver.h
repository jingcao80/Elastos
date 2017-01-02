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

/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.settings.bluetooth;

/* Required to handle timeout notification when phone is suspended */
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;

using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Utility::ILog;


public class BluetoothDiscoverableTimeoutReceiver extends BroadcastReceiver {
    private static const String TAG = "BluetoothDiscoverableTimeoutReceiver";

    private static const String INTENT_DISCOVERABLE_TIMEOUT =
        "android.bluetooth.intent.DISCOVERABLE_TIMEOUT";

    static void SetDiscoverableAlarm(Context context, Int64 alarmTime) {
        Logger::D(TAG, "SetDiscoverableAlarm(): alarmTime = " + alarmTime);

        Intent intent = new Intent(INTENT_DISCOVERABLE_TIMEOUT);
        intent->SetClass(context, BluetoothDiscoverableTimeoutReceiver.class);
        PendingIntent pending = PendingIntent->GetBroadcast(
            context, 0, intent, 0);
        AlarmManager alarmManager =
              (AlarmManager) context.getSystemService (Context.ALARM_SERVICE);

        if (pending != NULL) {
            // Cancel any previous alarms that do the same thing.
            alarmManager->Cancel(pending);
            Logger::D(TAG, "SetDiscoverableAlarm(): cancel prev alarm");
        }
        pending = PendingIntent->GetBroadcast(
            context, 0, intent, 0);

        alarmManager->Set(AlarmManager.RTC_WAKEUP, alarmTime, pending);
    }

    static void CancelDiscoverableAlarm(Context context) {
        Logger::D(TAG, "CancelDiscoverableAlarm(): Enter");

        Intent intent = new Intent(INTENT_DISCOVERABLE_TIMEOUT);
        intent->SetClass(context, BluetoothDiscoverableTimeoutReceiver.class);
        PendingIntent pending = PendingIntent->GetBroadcast(
            context, 0, intent, PendingIIntent::FLAG_NO_CREATE);
        if (pending != NULL) {
            // Cancel any previous alarms that do the same thing.
            AlarmManager alarmManager =
              (AlarmManager) context.getSystemService (Context.ALARM_SERVICE);

            alarmManager->Cancel(pending);
        }
    }

    //@Override
    CARAPI OnReceive(Context context, Intent intent) {
        LocalBluetoothAdapter localBluetoothAdapter = LocalBluetoothAdapter->GetInstance();

         If(localBluetoothAdapter != NULL  &&
            localBluetoothAdapter->GetState() == BluetoothAdapter.STATE_ON) {
            Logger::D(TAG, "Disable discoverable...");

            localBluetoothAdapter->SetScanMode(BluetoothAdapter.SCAN_MODE_CONNECTABLE);
         } else {
            Logger::E(TAG, "localBluetoothAdapter is NULL!!");
        }
    }
};
