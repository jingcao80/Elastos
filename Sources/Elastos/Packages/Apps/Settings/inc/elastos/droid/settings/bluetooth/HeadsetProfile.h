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
using Elastos::Droid::Bluetooth::IBluetoothHeadset;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Bluetooth::IBluetoothUuid;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IParcelUuid;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Settings::IR;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

/**
 * HeadsetProfile handles Bluetooth HFP and Headset profiles.
 */
final class HeadsetProfile implements LocalBluetoothProfile {
    private static const String TAG = "HeadsetProfile";
    private static Boolean V = TRUE;

    private BluetoothHeadset mService;
    private Boolean mIsProfileReady;

    private final LocalBluetoothAdapter mLocalAdapter;
    private final CachedBluetoothDeviceManager mDeviceManager;
    private final LocalBluetoothProfileManager mProfileManager;

    static const ParcelUuid[] UUIDS = {
        BluetoothUuid.HSP,
        BluetoothUuid.Handsfree,
    };

    static const String NAME = "HEADSET";

    // Order of this profile in device profiles list
    private static const Int32 ORDINAL = 0;

    // These callbacks run on the main thread.
    private final class HeadsetServiceListener
            implements BluetoothProfile.ServiceListener {

        CARAPI OnServiceConnected(Int32 profile, BluetoothProfile proxy) {
            if (V) Logger::D(TAG,"Bluetooth service connected");
            mService = (BluetoothHeadset) proxy;
            // We just bound to the service, so refresh the UI for any connected HFP devices.
            List<BluetoothDevice> deviceList = mService->GetConnectedDevices();
            while (!deviceList->IsEmpty()) {
                BluetoothDevice nextDevice = deviceList->Remove(0);
                CachedBluetoothDevice device = mDeviceManager->FindDevice(nextDevice);
                // we may add a new device here, but generally this should not happen
                if (device == NULL) {
                    Logger::W(TAG, "HeadsetProfile found new device: " + nextDevice);
                    device = mDeviceManager->AddDevice(mLocalAdapter, mProfileManager, nextDevice);
                }
                device->OnProfileStateChanged(HeadsetProfile.this,
                        BluetoothProfile.STATE_CONNECTED);
                device->Refresh();
            }

            mProfileManager->CallServiceConnectedListeners();
            mIsProfileReady=TRUE;
        }

        CARAPI OnServiceDisconnected(Int32 profile) {
            if (V) Logger::D(TAG,"Bluetooth service disconnected");
            mProfileManager->CallServiceDisconnectedListeners();
            mIsProfileReady=FALSE;
        }
    }

    public Boolean IsProfileReady() {
        return mIsProfileReady;
    }

    HeadsetProfile(Context context, LocalBluetoothAdapter adapter,
            CachedBluetoothDeviceManager deviceManager,
            LocalBluetoothProfileManager profileManager) {
        mLocalAdapter = adapter;
        mDeviceManager = deviceManager;
        mProfileManager = profileManager;
        mLocalAdapter->GetProfileProxy(context, new HeadsetServiceListener(),
                BluetoothProfile.HEADSET);
    }

    public Boolean IsConnectable() {
        return TRUE;
    }

    public Boolean IsAutoConnectable() {
        return TRUE;
    }

    public Boolean Connect(BluetoothDevice device) {
        if (mService == NULL) return FALSE;
        List<BluetoothDevice> sinks = mService->GetConnectedDevices();
        if (sinks != NULL) {
            for (BluetoothDevice sink : sinks) {
                Logger::D(TAG,"Not disconnecting device = " + sink);
            }
        }
        return mService->Connect(device);
    }

    public Boolean Disconnect(BluetoothDevice device) {
        if (mService == NULL) return FALSE;
        List<BluetoothDevice> deviceList = mService->GetConnectedDevices();
        if (!deviceList->IsEmpty()) {
            for (BluetoothDevice dev : deviceList) {
                if (dev->Equals(device)) {
                    if (V) Logger::D(TAG,"Downgrade priority as user" +
                                        "is disconnecting the headset");
                    // Downgrade priority as user is disconnecting the headset.
                    if (mService->GetPriority(device) > BluetoothProfile.PRIORITY_ON) {
                        mService->SetPriority(device, BluetoothProfile.PRIORITY_ON);
                    }
                    return mService->Disconnect(device);
                }
            }
        }
        return FALSE;
    }

    public Int32 GetConnectionStatus(BluetoothDevice device) {
        if (mService == NULL) return BluetoothProfile.STATE_DISCONNECTED;
        List<BluetoothDevice> deviceList = mService->GetConnectedDevices();
        if (!deviceList->IsEmpty()){
            for (BluetoothDevice dev : deviceList) {
                if (dev->Equals(device)) {
                    return mService->GetConnectionState(device);
                }
            }
        }
        return BluetoothProfile.STATE_DISCONNECTED;
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

    public List<BluetoothDevice> GetConnectedDevices() {
        if (mService == NULL) return new ArrayList<BluetoothDevice>(0);
        return mService->GetDevicesMatchingConnectionStates(
              new Int32[] {BluetoothProfile.STATE_CONNECTED,
                         BluetoothProfile.STATE_CONNECTING,
                         BluetoothProfile.STATE_DISCONNECTING});
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
        return R::string::bluetooth_profile_headset;
    }

    public Int32 GetSummaryResourceForDevice(BluetoothDevice device) {
        Int32 state = GetConnectionStatus(device);
        switch (state) {
            case BluetoothProfile.STATE_DISCONNECTED:
                return R::string::bluetooth_headset_profile_summary_use_for;

            case BluetoothProfile.STATE_CONNECTED:
                return R::string::bluetooth_headset_profile_summary_connected;

            default:
                return Utils->GetConnectionStateSummary(state);
        }
    }

    public Int32 GetDrawableResource(BluetoothClass btClass) {
        return R.drawable.ic_bt_headset_hfp;
    }

    protected void Finalize() {
        if (V) Logger::D(TAG, "Finalize()");
        if (mService != NULL) {
            try {
                BluetoothAdapter->GetDefaultAdapter()->CloseProfileProxy(BluetoothProfile.HEADSET,
                                                                       mService);
                mService = NULL;
            }catch (Throwable t) {
                Logger::W(TAG, "Error cleaning up HID proxy", t);
            }
        }
    }
}
