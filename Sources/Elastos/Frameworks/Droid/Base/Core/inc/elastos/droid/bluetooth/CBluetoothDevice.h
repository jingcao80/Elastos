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

#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHDEVICE_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHDEVICE_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothDevice.h"
#include "Elastos.Droid.Bluetooth.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/bluetooth/CBluetoothDeviceManagerCallback.h"

using Elastos::Droid::Content::IContext;
using Elastos::Utility::IUUID;
using Elastos::Droid::Os::IParcelUuid;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothDevice)
    , public Object
    , public IBluetoothDevice
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBluetoothDevice();

    static CARAPI_(AutoPtr<IIBluetooth>) GetService();

    CARAPI Equals(
        /* [in] */ IInterface *obj,
        /* [out] */ Boolean *equal);

    CARAPI ToString(
        /* [out] */ String* s);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetAddress(
        /* [out] */ String* address);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetType(
        /* [out] */ Int32* type);

    CARAPI GetAlias(
        /* [out] */ String* alias);

    CARAPI SetAlias(
        /* [in] */ const String& alias,
        /* [out] */ Boolean* result);

    CARAPI GetAliasName(
        /* [out] */ String* name);

    CARAPI CreateBond(
        /* [out] */ Boolean* result);

    CARAPI CreateBond(
        /* [in] */ Int32 transport,
        /* [out] */ Boolean* result);

    CARAPI CreateBondOutOfBand(
        /* [in] */ ArrayOf<Byte>* hash,
        /* [in] */ ArrayOf<Byte>* randomizer,
        /* [out] */ Boolean* result);

    CARAPI SetDeviceOutOfBandData(
        /* [in] */ ArrayOf<Byte>* hash,
        /* [in] */ ArrayOf<Byte>* randomizer,
        /* [out] */ Boolean* result);

    CARAPI CancelBondProcess(
        /* [out] */ Boolean* result);

    CARAPI RemoveBond(
        /* [out] */ Boolean* result);

    CARAPI GetBondState(
        /* [in] */ Int32* state);

    CARAPI IsConnected(
        /* [out] */ Boolean* connected);

    CARAPI GetBluetoothClass(
        /* [out] */ IBluetoothClass** bluetoothClass);

    CARAPI GetTrustState(
       /* [out] */ Boolean* state);

    CARAPI SetTrust(
       /* [in] */ Boolean value,
       /* [out] */ Boolean* result);

    CARAPI GetUuids(
        /* [out, callee] */ ArrayOf<IParcelUuid*>** uuids);

    CARAPI FetchUuidsWithSdp(
        /* [out] */ Boolean* result);

    CARAPI FetchMasInstances(
        /* [out] */ Boolean* result);

    CARAPI GetServiceChannel(
        /* [in] */ IParcelUuid* uuid,
        /* [out] */ Int32* channel);

    CARAPI SetPin(
        /* [in] */ ArrayOf<Byte>* pin,
        /* [out] */ Boolean* result);

    CARAPI SetPasskey(
        /* [in] */ Int32 passkey,
        /* [out] */ Boolean* result);

    CARAPI SetPairingConfirmation(
        /* [in] */ Boolean confirm,
        /* [out] */ Boolean* result);

    CARAPI SetRemoteOutOfBandData(
        /* [out] */ Boolean* result);

    CARAPI CancelPairingUserInput(
        /* [out] */ Boolean* result);

    CARAPI IsBluetoothDock(
        /* [out] */ Boolean* isBluetoothDock);

    CARAPI GetPhonebookAccessPermission(
        /* [out] */ Int32* result);

    CARAPI SetPhonebookAccessPermission(
        /* [in] */ Int32 value,
        /* [out] */ Boolean* result);

    CARAPI GetMessageAccessPermission(
        /* [out] */ Int32* result);

    CARAPI SetMessageAccessPermission(
        /* [in] */ Int32 value,
        /* [out] */ Boolean* result);

    CARAPI CreateRfcommSocket(
        /* [in] */ Int32 channel,
        /* [out] */ IBluetoothSocket** socket);

    CARAPI CreateRfcommSocketToServiceRecord(
        /* [in] */ IUUID* uuid,
        /* [out] */ IBluetoothSocket** socket);

    CARAPI CreateL2capSocketToServiceRecord(
        /* [in] */ IUUID* uuid,
        /* [out] */ IBluetoothSocket** socket);

    CARAPI CreateInsecureL2capSocketToServiceRecord(
        /* [in] */ IUUID* uuid,
        /* [out] */ IBluetoothSocket** socket);

    CARAPI CreateInsecureRfcommSocketToServiceRecord(
        /* [in] */ IUUID* uuid,
        /* [out] */ IBluetoothSocket** socket);

    CARAPI CreateInsecureRfcommSocket(
        /* [in] */ Int32 port,
        /* [out] */ IBluetoothSocket** socket);

    CARAPI CreateScoSocket(
        /* [out] */ IBluetoothSocket** socket);

    CARAPI ConnectGatt(
        /* [in] */ IContext* context,
        /* [in] */ Boolean autoConnect,
        /* [in] */ IBluetoothGattCallback* cb,
        /* [out] */ IBluetoothGatt** btGatt);

    CARAPI ConnectGatt(
        /* [in] */ IContext* context,
        /* [in] */ Boolean autoConnect,
        /* [in] */ IBluetoothGattCallback* cb,
        /* [in] */ Int32 transport,
        /* [out] */ IBluetoothGatt** btGatt);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetRemoteDiRecord(
        /* [out] */ IBluetoothRemoteDiRecord** record);

    static CARAPI_(AutoPtr< ArrayOf<Byte> >) ConvertPinToBytes(
        /* [in] */ const String& pin);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& address);

private:
    const static String TAG;
    const static Boolean DBG;

    static AutoPtr<IIBluetooth> sService;
    String mAddress;
    static AutoPtr<IIBluetoothManagerCallback> mStateChangeCallback;
    static Object sLock;

    friend class CBluetoothDeviceManagerCallback;
};

} // Bluetooth
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHDEVICE_H__
