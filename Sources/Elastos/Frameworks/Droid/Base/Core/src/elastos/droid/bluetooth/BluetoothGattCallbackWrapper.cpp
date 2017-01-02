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

#include "elastos/droid/bluetooth/BluetoothGattCallbackWrapper.h"

using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//                     BluetoothGattCallbackWrapper
//=====================================================================
CAR_INTERFACE_IMPL_2(BluetoothGattCallbackWrapper, Object, IIBluetoothGattCallback, IBinder)

BluetoothGattCallbackWrapper::BluetoothGattCallbackWrapper()
{
}

ECode BluetoothGattCallbackWrapper::OnClientRegistered(
    /* [in] */ Int32 status,
    /* [in] */ Int32 clientIf)
{
    return NOERROR;
}

ECode BluetoothGattCallbackWrapper::OnClientConnectionState(
    /* [in] */ Int32 status,
    /* [in] */ Int32 clientIf,
    /* [in] */ Boolean connected,
    /* [in] */ const String& address)
{
    return NOERROR;
}

ECode BluetoothGattCallbackWrapper::OnScanResult(
    /* [in] */ IScanResult* scanResult)
{
    VALIDATE_NOT_NULL(scanResult);
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCallbackWrapper::OnBatchScanResults(
    /* [in] */ IList* batchResults)
{
    return NOERROR;
}

ECode BluetoothGattCallbackWrapper::OnGetService(
    /* [in] */ const String& address,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcUuid)
{
    return NOERROR;
}

ECode BluetoothGattCallbackWrapper::OnGetIncludedService(
    /* [in] */ const String& address,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcUuid,
    /* [in] */ Int32 inclSrvcType,
    /* [in] */ Int32 inclSrvcInstId,
    /* [in] */ IParcelUuid* inclSrvcUuid)
{
    return NOERROR;
}

ECode BluetoothGattCallbackWrapper::OnGetCharacteristic(
    /* [in] */ const String& address,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcUuid,
    /* [in] */ Int32 charInstId,
    /* [in] */ IParcelUuid* charUuid,
    /* [in] */ Int32 charProps)
{
    return NOERROR;
}

ECode BluetoothGattCallbackWrapper::OnGetDescriptor(
    /* [in] */ const String& address,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcUuid,
    /* [in] */ Int32 charInstId,
    /* [in] */ IParcelUuid* charUuid,
    /* [in] */ Int32 descrInstId,
    /* [in] */ IParcelUuid* descrUuid)
{
    return NOERROR;
}

ECode BluetoothGattCallbackWrapper::OnSearchComplete(
    /* [in] */ const String& address,
    /* [in] */ Int32 status)
{
    return NOERROR;
}

ECode BluetoothGattCallbackWrapper::OnCharacteristicRead(
    /* [in] */ const String& address,
    /* [in] */ Int32 status,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcUuid,
    /* [in] */ Int32 charInstId,
    /* [in] */ IParcelUuid* charUuid,
    /* [in] */ ArrayOf<Byte>* value)
{
    return NOERROR;
}

ECode BluetoothGattCallbackWrapper::OnCharacteristicWrite(
    /* [in] */ const String& address,
    /* [in] */ Int32 status,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcUuid,
    /* [in] */ Int32 charInstId,
    /* [in] */ IParcelUuid* charUuid)
{
    return NOERROR;
}

ECode BluetoothGattCallbackWrapper::OnExecuteWrite(
    /* [in] */ const String& address,
    /* [in] */ Int32 status)
{
    return NOERROR;
}

ECode BluetoothGattCallbackWrapper::OnDescriptorRead(
    /* [in] */ const String& address,
    /* [in] */ Int32 status,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcUuid,
    /* [in] */ Int32 charInstId,
    /* [in] */ IParcelUuid* charUuid,
    /* [in] */ Int32 descrInstId,
    /* [in] */ IParcelUuid* descrUuid,
    /* [in] */ ArrayOf<Byte>* value)
{
    return NOERROR;
}

ECode BluetoothGattCallbackWrapper::OnDescriptorWrite(
    /* [in] */ const String& address,
    /* [in] */ Int32 status,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcUuid,
    /* [in] */ Int32 charInstId,
    /* [in] */ IParcelUuid* charUuid,
    /* [in] */ Int32 descrInstId,
    /* [in] */ IParcelUuid* descrUuid)
{
    return NOERROR;
}

ECode BluetoothGattCallbackWrapper::OnNotify(
    /* [in] */ const String& address,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcUuid,
    /* [in] */ Int32 charInstId,
    /* [in] */ IParcelUuid* charUuid,
    /* [in] */ ArrayOf<Byte>* value)
{
    return NOERROR;
}

ECode BluetoothGattCallbackWrapper::OnReadRemoteRssi(
    /* [in] */ const String& address,
    /* [in] */ Int32 rssi,
    /* [in] */ Int32 status)
{
    return NOERROR;
}

ECode BluetoothGattCallbackWrapper::OnMultiAdvertiseCallback(
    /* [in] */ Int32 status,
    /* [in] */ Boolean isStart,
    /* [in] */ IAdvertiseSettings* advertiseSettings)
{
    return NOERROR;
}

ECode BluetoothGattCallbackWrapper::OnConfigureMTU(
    /* [in] */ const String& address,
    /* [in] */ Int32 mtu,
    /* [in] */ Int32 status)
{
    return NOERROR;
}

ECode BluetoothGattCallbackWrapper::OnFoundOrLost(
    /* [in] */ Boolean onFound,
    /* [in] */ IScanResult* scanResult)
{
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
