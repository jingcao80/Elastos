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
