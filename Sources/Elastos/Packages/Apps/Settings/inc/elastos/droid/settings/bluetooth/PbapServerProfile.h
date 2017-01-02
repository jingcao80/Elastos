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
using Elastos::Droid::Bluetooth::IBluetoothClass;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Bluetooth::IBluetoothPbap;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Settings::IR;

using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;

/**
 * PBAPServer Profile
 */
final class PbapServerProfile implements LocalBluetoothProfile {
    private static const String TAG = "PbapServerProfile";
    private static Boolean V = TRUE;

    private BluetoothPbap mService;
    private Boolean mIsProfileReady;

    static const String NAME = "PBAP Server";

    // Order of this profile in device profiles list
    private static const Int32 ORDINAL = 6;

    // These callbacks run on the main thread.
    private final class PbapServiceListener
            implements BluetoothPbap.ServiceListener {

        CARAPI OnServiceConnected(BluetoothPbap proxy) {
            if (V) Logger::D(TAG,"Bluetooth service connected");
            mService = (BluetoothPbap) proxy;
            mIsProfileReady=TRUE;
        }

        CARAPI OnServiceDisconnected() {
            if (V) Logger::D(TAG,"Bluetooth service disconnected");
            mIsProfileReady=FALSE;
        }
    }

    public Boolean IsProfileReady() {
        return mIsProfileReady;
    }

    PbapServerProfile(Context context) {
        BluetoothPbap pbap = new BluetoothPbap(context, new PbapServiceListener());
    }

    public Boolean IsConnectable() {
        return TRUE;
    }

    public Boolean IsAutoConnectable() {
        return FALSE;
    }

    public Boolean Connect(BluetoothDevice device) {
        /*Can't connect from server */
        return FALSE;

    }

    public Boolean Disconnect(BluetoothDevice device) {
        if (mService == NULL) return FALSE;
        return mService->Disconnect();
    }

    public Int32 GetConnectionStatus(BluetoothDevice device) {
        if (mService == NULL) {
            return BluetoothProfile.STATE_DISCONNECTED;
        }
        if (mService->IsConnected(device))
            return BluetoothProfile.STATE_CONNECTED;
        else
            return BluetoothProfile.STATE_DISCONNECTED;
    }

    public Boolean IsPreferred(BluetoothDevice device) {
        return FALSE;
    }

    public Int32 GetPreferred(BluetoothDevice device) {
        return -1;
    }

    CARAPI SetPreferred(BluetoothDevice device, Boolean preferred) {
        // ignore: isPreferred is always TRUE for PBAP
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
        return R::string::bluetooth_profile_pbap;
    }

    public Int32 GetSummaryResourceForDevice(BluetoothDevice device) {
        return R::string::bluetooth_profile_pbap_summary;
    }

    public Int32 GetDrawableResource(BluetoothClass btClass) {
        return R.drawable.ic_bt_cellphone;
    }

    protected void Finalize() {
        if (V) Logger::D(TAG, "Finalize()");
        if (mService != NULL) {
            try {
                mService->Close();
                mService = NULL;
            }catch (Throwable t) {
                Logger::W(TAG, "Error cleaning up PBAP proxy", t);
            }
        }
    }
}
