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

using Elastos::Droid::Bluetooth::IBluetoothClass;
using Elastos::Droid::Bluetooth::IBluetoothDevice;

/**
 * LocalBluetoothProfile is an interface defining the basic
 * functionality related to a Bluetooth profile.
 */
interface LocalBluetoothProfile {

    /**
     * Returns TRUE if the user can initiate a connection, FALSE otherwise.
     */
    Boolean IsConnectable();

    /**
     * Returns TRUE if the user can enable auto connection for this profile.
     */
    Boolean IsAutoConnectable();

    Boolean Connect(BluetoothDevice device);

    Boolean Disconnect(BluetoothDevice device);

    Int32 GetConnectionStatus(BluetoothDevice device);

    Boolean IsPreferred(BluetoothDevice device);

    Int32 GetPreferred(BluetoothDevice device);

    void SetPreferred(BluetoothDevice device, Boolean preferred);

    Boolean IsProfileReady();

    /** Display order for device profile settings. */
    Int32 GetOrdinal();

    /**
     * Returns the string resource ID for the localized name for this profile.
     * @param device the Bluetooth device (to distinguish between PAN roles)
     */
    Int32 GetNameResource(BluetoothDevice device);

    /**
     * Returns the string resource ID for the summary text for this profile
     * for the specified device, e.g. "Use for media audio" or
     * "Connected to media audio".
     * @param device the device to query for profile connection status
     * @return a string resource ID for the profile summary text
     */
    Int32 GetSummaryResourceForDevice(BluetoothDevice device);

    Int32 GetDrawableResource(BluetoothClass btClass);
}
