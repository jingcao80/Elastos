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

#include "elastos/droid/bluetooth/BluetoothGattService.h"
#include "elastos/droid/bluetooth/BluetoothGattCharacteristic.h"

using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//                         BluetoothGattService
//=====================================================================
CAR_INTERFACE_IMPL(BluetoothGattService, Object, IBluetoothGattService);

BluetoothGattService::BluetoothGattService()
{
}

ECode BluetoothGattService::constructor(
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 serviceType)
{
    mDevice = NULL;
    mUuid = uuid;
    mInstanceId = 0;
    mServiceType = serviceType;
    //mCharacteristics = new ArrayList<BluetoothGattCharacteristic>();
    //mIncludedServices = new ArrayList<BluetoothGattService>();
    CArrayList::New((IList**)&mCharacteristics);
    CArrayList::New((IList**)&mIncludedServices);
    return NOERROR;
}

BluetoothGattService::BluetoothGattService(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 instanceId,
    /* [in] */ Int32 serviceType)
{
    mDevice = device;
    mUuid = uuid;
    mInstanceId = instanceId;
    mServiceType = serviceType;
    //mCharacteristics = new ArrayList<BluetoothGattCharacteristic>();
    //mIncludedServices = new ArrayList<BluetoothGattService>();
    CArrayList::New((IList**)&mCharacteristics);
    CArrayList::New((IList**)&mIncludedServices);
}

ECode BluetoothGattService::GetDevice(
    /* [out] */ IBluetoothDevice** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDevice;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BluetoothGattService::AddService(
    /* [in] */ IBluetoothGattService* service,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mIncludedServices->Add(TO_IINTERFACE(service));
    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGattService::AddCharacteristic(
    /* [in] */ IBluetoothGattCharacteristic* characteristic,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mCharacteristics->Add(TO_IINTERFACE(characteristic));
    ((BluetoothGattCharacteristic*)characteristic)->SetService(this);
    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGattService::GetCharacteristic(
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 instanceId,
    /* [out] */ IBluetoothGattCharacteristic** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    Int32 size;
    mCharacteristics->GetSize(&size);
    for(Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mCharacteristics->Get(i, (IInterface**)&obj);
        IBluetoothGattCharacteristic* characteristic = IBluetoothGattCharacteristic::Probe(obj);

        AutoPtr<IUUID> cuuid;
        characteristic->GetUuid((IUUID**)&cuuid);
        Int32 cInstanceId;
        characteristic->GetInstanceId(&cInstanceId);
        Boolean eq = FALSE;
        if ((uuid->Equals(TO_IINTERFACE(cuuid), &eq), eq)
         && cInstanceId == instanceId) {
            *result = characteristic;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode BluetoothGattService::SetInstanceId(
    /* [in] */ Int32 instanceId)
{
    mInstanceId = instanceId;
    return NOERROR;
}

ECode BluetoothGattService::GetHandles(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHandles;
    return NOERROR;
}

ECode BluetoothGattService::SetHandles(
    /* [in] */ Int32 handles)
{
    mHandles = handles;
    return NOERROR;
}

ECode BluetoothGattService::AddIncludedService(
    /* [in] */ IBluetoothGattService* includedService)
{
    mIncludedServices->Add(TO_IINTERFACE(includedService));
    return NOERROR;
}

ECode BluetoothGattService::GetUuid(
    /* [out] */ IUUID** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUuid;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BluetoothGattService::GetInstanceId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mInstanceId;
    return NOERROR;
}

ECode BluetoothGattService::GetType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mServiceType;
    return NOERROR;
}

ECode BluetoothGattService::GetIncludedServices(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIncludedServices;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BluetoothGattService::GetCharacteristics(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCharacteristics;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BluetoothGattService::GetCharacteristic(
    /* [in] */ IUUID* uuid,
    /* [out] */ IBluetoothGattCharacteristic** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    Int32 size;
    mCharacteristics->GetSize(&size);
    for(Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mCharacteristics->Get(i, (IInterface**)&obj);
        IBluetoothGattCharacteristic* characteristic = IBluetoothGattCharacteristic::Probe(obj);

        AutoPtr<IUUID> cuuid;
        characteristic->GetUuid((IUUID**)&cuuid);
        Boolean eq = FALSE;
        if (uuid->Equals(TO_IINTERFACE(cuuid), &eq), eq) {
            *result = characteristic;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode BluetoothGattService::IsAdvertisePreferred(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAdvertisePreferred;
    return NOERROR;
}

ECode BluetoothGattService::SetAdvertisePreferred(
    /* [in] */ Boolean advertisePreferred)
{
    mAdvertisePreferred = advertisePreferred;
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
