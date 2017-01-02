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

#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTCALLBACKWRAPPER_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTCALLBACKWRAPPER_H__

#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Bluetooth::LE::IAdvertiseSettings;
using Elastos::Droid::Bluetooth::LE::IScanResult;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelUuid;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

/**
  * Wrapper class for default implementation of IBluetoothGattCallback.
  *
  * @hide
  */
class BluetoothGattCallbackWrapper
    : public Object
    , public IIBluetoothGattCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    BluetoothGattCallbackWrapper();

    // @Override
    CARAPI OnClientRegistered(
        /* [in] */ Int32 status,
        /* [in] */ Int32 clientIf);

    // @Override
    CARAPI OnClientConnectionState(
        /* [in] */ Int32 status,
        /* [in] */ Int32 clientIf,
        /* [in] */ Boolean connected,
        /* [in] */ const String& address);

    // @Override
    CARAPI OnScanResult(
        /* [in] */ IScanResult* scanResult);

    // @Override
    CARAPI OnBatchScanResults(
        /* [in] */ IList* batchResults);// IScanResult

    // @Override
    CARAPI OnGetService(
        /* [in] */ const String& address,
        /* [in] */ Int32 srvcType,
        /* [in] */ Int32 srvcInstId,
        /* [in] */ IParcelUuid* srvcUuid);

    // @Override
    CARAPI OnGetIncludedService(
        /* [in] */ const String& address,
        /* [in] */ Int32 srvcType,
        /* [in] */ Int32 srvcInstId,
        /* [in] */ IParcelUuid* srvcUuid,
        /* [in] */ Int32 inclSrvcType,
        /* [in] */ Int32 inclSrvcInstId,
        /* [in] */ IParcelUuid* inclSrvcUuid);

    // @Override
    CARAPI OnGetCharacteristic(
        /* [in] */ const String& address,
        /* [in] */ Int32 srvcType,
        /* [in] */ Int32 srvcInstId,
        /* [in] */ IParcelUuid* srvcUuid,
        /* [in] */ Int32 charInstId,
        /* [in] */ IParcelUuid* charUuid,
        /* [in] */ Int32 charProps);

    // @Override
    CARAPI OnGetDescriptor(
        /* [in] */ const String& address,
        /* [in] */ Int32 srvcType,
        /* [in] */ Int32 srvcInstId,
        /* [in] */ IParcelUuid* srvcUuid,
        /* [in] */ Int32 charInstId,
        /* [in] */ IParcelUuid* charUuid,
        /* [in] */ Int32 descrInstId,
        /* [in] */ IParcelUuid* descrUuid);

    // @Override
    CARAPI OnSearchComplete(
        /* [in] */ const String& address,
        /* [in] */ Int32 status);

    // @Override
    CARAPI OnCharacteristicRead(
        /* [in] */ const String& address,
        /* [in] */ Int32 status,
        /* [in] */ Int32 srvcType,
        /* [in] */ Int32 srvcInstId,
        /* [in] */ IParcelUuid* srvcUuid,
        /* [in] */ Int32 charInstId,
        /* [in] */ IParcelUuid* charUuid,
        /* [in] */ ArrayOf<Byte>* value);

    // @Override
    CARAPI OnCharacteristicWrite(
        /* [in] */ const String& address,
        /* [in] */ Int32 status,
        /* [in] */ Int32 srvcType,
        /* [in] */ Int32 srvcInstId,
        /* [in] */ IParcelUuid* srvcUuid,
        /* [in] */ Int32 charInstId,
        /* [in] */ IParcelUuid* charUuid);

    // @Override
    CARAPI OnExecuteWrite(
        /* [in] */ const String& address,
        /* [in] */ Int32 status);

    // @Override
    CARAPI OnDescriptorRead(
        /* [in] */ const String& address,
        /* [in] */ Int32 status,
        /* [in] */ Int32 srvcType,
        /* [in] */ Int32 srvcInstId,
        /* [in] */ IParcelUuid* srvcUuid,
        /* [in] */ Int32 charInstId,
        /* [in] */ IParcelUuid* charUuid,
        /* [in] */ Int32 descrInstId,
        /* [in] */ IParcelUuid* descrUuid,
        /* [in] */ ArrayOf<Byte>* value);

    // @Override
    CARAPI OnDescriptorWrite(
        /* [in] */ const String& address,
        /* [in] */ Int32 status,
        /* [in] */ Int32 srvcType,
        /* [in] */ Int32 srvcInstId,
        /* [in] */ IParcelUuid* srvcUuid,
        /* [in] */ Int32 charInstId,
        /* [in] */ IParcelUuid* charUuid,
        /* [in] */ Int32 descrInstId,
        /* [in] */ IParcelUuid* descrUuid);

    // @Override
    CARAPI OnNotify(
        /* [in] */ const String& address,
        /* [in] */ Int32 srvcType,
        /* [in] */ Int32 srvcInstId,
        /* [in] */ IParcelUuid* srvcUuid,
        /* [in] */ Int32 charInstId,
        /* [in] */ IParcelUuid* charUuid,
        /* [in] */ ArrayOf<Byte>* value);

    // @Override
    CARAPI OnReadRemoteRssi(
        /* [in] */ const String& address,
        /* [in] */ Int32 rssi,
        /* [in] */ Int32 status);

    // @Override
    CARAPI OnMultiAdvertiseCallback(
        /* [in] */ Int32 status,
        /* [in] */ Boolean isStart,
        /* [in] */ IAdvertiseSettings* advertiseSettings);

    // @Override
    CARAPI OnConfigureMTU(
        /* [in] */ const String& address,
        /* [in] */ Int32 mtu,
        /* [in] */ Int32 status);

    // @Override
    CARAPI OnFoundOrLost(
        /* [in] */ Boolean onFound,
        /* [in] */ IScanResult* scanResult);

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info)
        return Object::ToString(info);
    }
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTCALLBACKWRAPPER_H__

