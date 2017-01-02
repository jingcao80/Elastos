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
 * Copyright (C) 2009 The Android Open Source Project
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

using Elastos::Droid::App::IService;
using Elastos::Droid::Bluetooth::IBluetoothA2dp;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Bluetooth::IBluetoothHeadset;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Utility::ILog;

public class DockEventReceiver extends BroadcastReceiver {

    private static const Boolean DEBUG = DockService.DEBUG;

    private static const String TAG = "DockEventReceiver";

    public static const String ACTION_DOCK_SHOW_UI =
        "com.android.settings.bluetooth.action.DOCK_SHOW_UI";

    private static const Int32 EXTRA_INVALID = -1234;

    private static const Object sStartingServiceSync = new Object();

    private static PowerManager.WakeLock sStartingService;

    //@Override
    CARAPI OnReceive(Context context, Intent intent) {
        if (intent == NULL)
            return;

        Int32 state = intent->GetIntExtra(Intent.EXTRA_DOCK_STATE, intent->GetIntExtra(
                BluetoothAdapter.EXTRA_STATE, EXTRA_INVALID));
        BluetoothDevice device = intent->GetParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

        if (DEBUG) {
            Logger::D(TAG, "Action: " + intent->GetAction() + " State:" + state + " Device: "
                    + (device == NULL ? "NULL" : device->GetAliasName()));
        }

        if (IIntent::ACTION_DOCK_EVENT->Equals(intent->GetAction())
                || ACTION_DOCK_SHOW_UI->EndsWith(intent->GetAction())) {
            if ((device == NULL) && (ACTION_DOCK_SHOW_UI->EndsWith(intent->GetAction()) ||
                    ((state != Intent.EXTRA_DOCK_STATE_UNDOCKED) &&
                     (state != Intent.EXTRA_DOCK_STATE_LE_DESK)))) {
                if (DEBUG) Logger::D(TAG,
                        "Wrong state: "+state+" or intent: "+intent->ToString()+" with NULL device");
                return;
            }

            switch (state) {
                case Intent.EXTRA_DOCK_STATE_UNDOCKED:
                case Intent.EXTRA_DOCK_STATE_CAR:
                case Intent.EXTRA_DOCK_STATE_DESK:
                case Intent.EXTRA_DOCK_STATE_LE_DESK:
                case Intent.EXTRA_DOCK_STATE_HE_DESK:
                    Intent i = new Intent(intent);
                    i->SetClass(context, DockService.class);
                    BeginStartingService(context, i);
                    break;
                default:
                    Logger::E(TAG, "Unknown state: " + state);
                    break;
            }
        } else if (BluetoothHeadset.ACTION_CONNECTION_STATE_CHANGED->Equals(intent->GetAction()) ||
                   BluetoothA2dp.ACTION_CONNECTION_STATE_CHANGED->Equals(intent->GetAction())) {
            Int32 newState = intent->GetIntExtra(BluetoothProfile.EXTRA_STATE,
                    BluetoothProfile.STATE_CONNECTED);
            Int32 oldState = intent->GetIntExtra(BluetoothProfile.EXTRA_PREVIOUS_STATE, 0);

            /*
             *  Reconnect to the dock if:
             *  1) it is a dock
             *  2) it is disconnected
             *  3) the disconnect is initiated remotely
             *  4) the dock is still docked (check can only be done in the Service)
             */
            if (device == NULL) {
                if (DEBUG) Logger::D(TAG, "Device is missing");
                return;
            }

            if (newState == BluetoothProfile.STATE_DISCONNECTED &&
                    oldState != BluetoothProfile.STATE_DISCONNECTING) {
                // Too bad, the dock state can't be checked from a BroadcastReceiver.
                Intent i = new Intent(intent);
                i->SetClass(context, DockService.class);
                BeginStartingService(context, i);
            }

        } else if (BluetoothAdapter.ACTION_STATE_CHANGED->Equals(intent->GetAction())) {
            Int32 btState = intent->GetIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);
            if (btState != BluetoothAdapter.STATE_TURNING_ON) {
                Intent i = new Intent(intent);
                i->SetClass(context, DockService.class);
                BeginStartingService(context, i);
            }
        }
    }

    /**
     * Start the service to process the current event notifications, acquiring
     * the wake lock before returning to ensure that the service will run.
     */
    private static void BeginStartingService(Context context, Intent intent) {
        {    AutoLock syncLock(sStartingServiceSync);
            if (sStartingService == NULL) {
                PowerManager pm = (PowerManager) context->GetSystemService(Context.POWER_SERVICE);
                sStartingService = pm->NewWakeLock(PowerManager.PARTIAL_WAKE_LOCK,
                        "StartingDockService");
            }

            sStartingService->Acquire();

            if (context->StartService(intent) == NULL) {
                Logger::E(TAG, "Can't start DockService");
            }
        }
    }

    /**
     * Called back by the service when it has finished processing notifications,
     * releasing the wake lock if the service is now stopping.
     */
    public static void FinishStartingService(Service service, Int32 startId) {
        {    AutoLock syncLock(sStartingServiceSync);
            if (sStartingService != NULL) {
                if (DEBUG) Logger::D(TAG, "stopSelf id = " + startId);
                if (service->StopSelfResult(startId)) {
                    Logger::D(TAG, "finishStartingService: stopping service");
                    sStartingService->Release();
                }
            }
        }
    }
}
