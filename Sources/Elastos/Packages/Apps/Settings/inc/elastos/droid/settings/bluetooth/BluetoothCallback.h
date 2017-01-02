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

/**
 * BluetoothCallback provides a callback interface for the settings
 * UI to receive events from {@link BluetoothEventManager}.
 */
interface BluetoothCallback {
    void OnBluetoothStateChanged(Int32 bluetoothState);
    void OnScanningStateChanged(Boolean started);
    void OnDeviceAdded(CachedBluetoothDevice cachedDevice);
    void OnDeviceDeleted(CachedBluetoothDevice cachedDevice);
    void OnDeviceBondStateChanged(CachedBluetoothDevice cachedDevice, Int32 bondState);
}
