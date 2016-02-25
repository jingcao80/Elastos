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

using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothClass;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Bluetooth::IBluetoothInputDevice;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Settings::IR;

using Elastos::Utility::IList;

/**
 * HidProfile handles Bluetooth HID profile.
 */
final class HidProfile implements LocalBluetoothProfile {
    private static const String TAG = "HidProfile";
    private static Boolean V = TRUE;

    private BluetoothInputDevice mService;
    private Boolean mIsProfileReady;

    private final LocalBluetoothAdapter mLocalAdapter;
    private final CachedBluetoothDeviceManager mDeviceManager;
    private final LocalBluetoothProfileManager mProfileManager;

    static const String NAME = "HID";

    // Order of this profile in device profiles list
    private static const Int32 ORDINAL = 3;

    // These callbacks run on the main thread.
    private final class InputDeviceServiceListener
            implements BluetoothProfile.ServiceListener {

        CARAPI OnServiceConnected(Int32 profile, BluetoothProfile proxy) {
            if (V) Logger::D(TAG,"Bluetooth service connected");
            mService = (BluetoothInputDevice) proxy;
            // We just bound to the service, so refresh the UI for any connected HID devices.
            List<BluetoothDevice> deviceList = mService->GetConnectedDevices();
            while (!deviceList->IsEmpty()) {
                BluetoothDevice nextDevice = deviceList->Remove(0);
                CachedBluetoothDevice device = mDeviceManager->FindDevice(nextDevice);
                // we may add a new device here, but generally this should not happen
                if (device == NULL) {
                    Logger::W(TAG, "HidProfile found new device: " + nextDevice);
                    device = mDeviceManager->AddDevice(mLocalAdapter, mProfileManager, nextDevice);
                }
                device->OnProfileStateChanged(HidProfile.this, BluetoothProfile.STATE_CONNECTED);
                device->Refresh();
            }
            mIsProfileReady=TRUE;
        }

        CARAPI OnServiceDisconnected(Int32 profile) {
            if (V) Logger::D(TAG,"Bluetooth service disconnected");
            mIsProfileReady=FALSE;
        }
    }

    public Boolean IsProfileReady() {
        return mIsProfileReady;
    }

    HidProfile(Context context, LocalBluetoothAdapter adapter,
        CachedBluetoothDeviceManager deviceManager,
        LocalBluetoothProfileManager profileManager) {
        mLocalAdapter = adapter;
        mDeviceManager = deviceManager;
        mProfileManager = profileManager;
        adapter->GetProfileProxy(context, new InputDeviceServiceListener(),
                BluetoothProfile.INPUT_DEVICE);
    }

    public Boolean IsConnectable() {
        return TRUE;
    }

    public Boolean IsAutoConnectable() {
        return TRUE;
    }

    public Boolean Connect(BluetoothDevice device) {
        if (mService == NULL) return FALSE;
        return mService->Connect(device);
    }

    public Boolean Disconnect(BluetoothDevice device) {
        if (mService == NULL) return FALSE;
        return mService->Disconnect(device);
    }

    public Int32 GetConnectionStatus(BluetoothDevice device) {
        if (mService == NULL) {
            return BluetoothProfile.STATE_DISCONNECTED;
        }
        List<BluetoothDevice> deviceList = mService->GetConnectedDevices();

        return !deviceList->IsEmpty() && deviceList->Get(0).Equals(device)
                ? mService->GetConnectionState(device)
                : BluetoothProfile.STATE_DISCONNECTED;
    }

    public Boolean IsPreferred(BluetoothDevice device) {
        if (mService == NULL) return FALSE;
        return mService->GetPriority(device) > BluetoothProfile.PRIORITY_OFF;
    }

    public Int32 GetPreferred(BluetoothDevice device) {
        if (mService == NULL) return BluetoothProfile.PRIORITY_OFF;
        return mService->GetPriority(device);
    }

    CARAPI SetPreferred(BluetoothDevice device, Boolean preferred) {
        if (mService == NULL) return;
        if (preferred) {
            if (mService->GetPriority(device) < BluetoothProfile.PRIORITY_ON) {
                mService->SetPriority(device, BluetoothProfile.PRIORITY_ON);
            }
        } else {
            mService->SetPriority(device, BluetoothProfile.PRIORITY_OFF);
        }
    }

    CARAPI ToString(
        /* [out] */ String* str)
    {
        return NAME;
    }

    public Int32 GetOrdinal() {
        return ORDINAL;
    }

    public Int32 GetNameResource(BluetoothDevice device) {
        // TODO: distinguish between keyboard and mouse?
        return R::string::bluetooth_profile_hid;
    }

    public Int32 GetSummaryResourceForDevice(BluetoothDevice device) {
        Int32 state = GetConnectionStatus(device);
        switch (state) {
            case BluetoothProfile.STATE_DISCONNECTED:
                return R::string::bluetooth_hid_profile_summary_use_for;

            case BluetoothProfile.STATE_CONNECTED:
                return R::string::bluetooth_hid_profile_summary_connected;

            default:
                return Utils->GetConnectionStateSummary(state);
        }
    }

    public Int32 GetDrawableResource(BluetoothClass btClass) {
        if (btClass == NULL) {
            return R.drawable.ic_lockscreen_ime;
        }
        return GetHidClassDrawable(btClass);
    }

    static Int32 GetHidClassDrawable(BluetoothClass btClass) {
        switch (btClass->GetDeviceClass()) {
            case BluetoothClass.Device.PERIPHERAL_KEYBOARD:
            case BluetoothClass.Device.PERIPHERAL_KEYBOARD_POINTING:
                return R.drawable.ic_lockscreen_ime;
            case BluetoothClass.Device.PERIPHERAL_POINTING:
                return R.drawable.ic_bt_pointing_hid;
            default:
                return R.drawable.ic_bt_misc_hid;
        }
    }

    protected void Finalize() {
        if (V) Logger::D(TAG, "Finalize()");
        if (mService != NULL) {
            try {
                BluetoothAdapter->GetDefaultAdapter()->CloseProfileProxy(BluetoothProfile.INPUT_DEVICE,
                                                                       mService);
                mService = NULL;
            }catch (Throwable t) {
                Logger::W(TAG, "Error cleaning up HID proxy", t);
            }
        }
    }
}
