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
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IParcelUuid;
using Elastos::Droid::Utility::ILog;

using Elastos::Utility::ISet;

/**
 * LocalBluetoothAdapter provides an interface between the Settings app
 * and the functionality of the local {@link BluetoothAdapter}, specifically
 * those related to state transitions of the adapter itself.
 *
 * <p>Connection and bonding state changes affecting specific devices
 * are handled by {@link CachedBluetoothDeviceManager},
 * {@link BluetoothEventManager}, and {@link LocalBluetoothProfileManager}.
 */
public class LocalBluetoothAdapter {
    private static const String TAG = "LocalBluetoothAdapter";

    /** This class does not allow direct access to the BluetoothAdapter. */
    private final BluetoothAdapter mAdapter;

    private LocalBluetoothProfileManager mProfileManager;

    private static LocalBluetoothAdapter sInstance;

    private Int32 mState = BluetoothAdapter.ERROR;

    private static const Int32 SCAN_EXPIRATION_MS = 5 * 60 * 1000; // 5 mins

    private Int64 mLastScan;

    private LocalBluetoothAdapter(BluetoothAdapter adapter) {
        mAdapter = adapter;
    }

    void SetProfileManager(LocalBluetoothProfileManager manager) {
        mProfileManager = manager;
    }

    /**
     * Get the singleton instance of the LocalBluetoothAdapter. If this device
     * doesn't support Bluetooth, then NULL will be returned. Callers must be
     * prepared to handle a NULL return value.
     * @return the LocalBluetoothAdapter object, or NULL if not supported
     */
    static synchronized LocalBluetoothAdapter GetInstance() {
        if (sInstance == NULL) {
            BluetoothAdapter adapter = BluetoothAdapter->GetDefaultAdapter();
            if (adapter != NULL) {
                sInstance = new LocalBluetoothAdapter(adapter);
            }
        }

        return sInstance;
    }

    // Pass-through BluetoothAdapter methods that we can intercept if necessary

    void CancelDiscovery() {
        mAdapter->CancelDiscovery();
    }

    Boolean Enable() {
        return mAdapter->Enable();
    }

    Boolean Disable() {
        return mAdapter->Disable();
    }

    void GetProfileProxy(Context context,
            BluetoothProfile.ServiceListener listener, Int32 profile) {
        mAdapter->GetProfileProxy(context, listener, profile);
    }

    Set<BluetoothDevice> GetBondedDevices() {
        return mAdapter->GetBondedDevices();
    }

    String GetName() {
        return mAdapter->GetName();
    }

    Int32 GetScanMode() {
        return mAdapter->GetScanMode();
    }

    Int32 GetState() {
        return mAdapter->GetState();
    }

    ParcelUuid[] GetUuids() {
        return mAdapter->GetUuids();
    }

    Boolean IsDiscovering() {
        return mAdapter->IsDiscovering();
    }

    Boolean IsEnabled() {
        return mAdapter->IsEnabled();
    }

    void SetDiscoverableTimeout(Int32 timeout) {
        mAdapter->SetDiscoverableTimeout(timeout);
    }

    void SetName(String name) {
        mAdapter->SetName(name);
    }

    void SetScanMode(Int32 mode) {
        mAdapter->SetScanMode(mode);
    }

    Boolean SetScanMode(Int32 mode, Int32 duration) {
        return mAdapter->SetScanMode(mode, duration);
    }

    void StartScanning(Boolean force) {
        // Only start if we're not already scanning
        if (!mAdapter->IsDiscovering()) {
            if (!force) {
                // Don't scan more than frequently than SCAN_EXPIRATION_MS,
                // unless forced
                if (mLastScan + SCAN_EXPIRATION_MS > System->CurrentTimeMillis()) {
                    return;
                }

                // If we are playing music, don't scan unless forced.
                A2dpProfile a2dp = mProfileManager->GetA2dpProfile();
                if (a2dp != NULL && a2dp->IsA2dpPlaying()) {
                    return;
                }
            }

            if (mAdapter->StartDiscovery()) {
                mLastScan = System->CurrentTimeMillis();
            }
        }
    }

    void StopScanning() {
        if (mAdapter->IsDiscovering()) {
            mAdapter->CancelDiscovery();
        }
    }

    public synchronized Int32 GetBluetoothState() {
        // Always sync state, in case it changed while paused
        SyncBluetoothState();
        return mState;
    }

    synchronized void SetBluetoothStateInt(Int32 state) {
        mState = state;

        if (state == BluetoothAdapter.STATE_ON) {
            // if mProfileManager hasn't been constructed yet, it will
            // get the adapter UUIDs in its constructor when it is.
            if (mProfileManager != NULL) {
                mProfileManager->SetBluetoothStateOn();
            }
        }
    }

    // Returns TRUE if the state changed; FALSE otherwise.
    Boolean SyncBluetoothState() {
        Int32 currentState = mAdapter->GetState();
        if (currentState != mState) {
            SetBluetoothStateInt(mAdapter->GetState());
            return TRUE;
        }
        return FALSE;
    }

    CARAPI SetBluetoothEnabled(Boolean enabled) {
        Boolean success = enabled
                ? mAdapter->Enable()
                : mAdapter->Disable();

        if (success) {
            SetBluetoothStateInt(enabled
                ? BluetoothAdapter.STATE_TURNING_ON
                : BluetoothAdapter.STATE_TURNING_OFF);
        } else {
            if (Utils.V) {
                Logger::V(TAG, "setBluetoothEnabled call, manager didn't return " +
                        "success for enabled: " + enabled);
            }

            SyncBluetoothState();
        }
    }
}
