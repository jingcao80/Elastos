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

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/bluetooth/le/AdvertiseData.h"
#include "elastos/droid/bluetooth/le/CAdvertiseData.h"
#include "elastos/droid/bluetooth/le/BluetoothLeUtils.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/utility/Arrays.h"
#include "elastos/utility/Objects.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Utility::Arrays;
using Elastos::Utility::Objects;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

//=====================================================================
//                        AdvertiseData::Builder
//=====================================================================
CAR_INTERFACE_IMPL(AdvertiseData::Builder, Object, IAdvertiseDataBuilder);

AdvertiseData::Builder::Builder()
{
    CArrayList::New((IList**)&mServiceUuids);
    CSparseArray::New((ISparseArray**)&mManufacturerSpecificData);
    CArrayMap::New((IMap**)&mServiceData);
}

ECode AdvertiseData::Builder::AddServiceUuid(
    /* [in] */ IParcelUuid* serviceUuid)
{
    if (serviceUuid == NULL) {
        //throw new IllegalArgumentException("serivceUuids are null");
        Slogger::E("AdvertiseData::Builder::AddServiceUuid", "serivceUuids are null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mServiceUuids->Add(TO_IINTERFACE(serviceUuid));
    //return this;
    return NOERROR;
}

ECode AdvertiseData::Builder::AddServiceData(
    /* [in] */ IParcelUuid* serviceDataUuid,
    /* [in] */ ArrayOf<Byte>* serviceData)
{
    if (serviceDataUuid == NULL || serviceData == NULL ) {
        //throw new IllegalArgumentException("serviceDataUuid or serviceDataUuid is null");
        Slogger::E("AdvertiseData::Builder::AddServiceData", "serviceDataUuid or serviceDataUuid is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IArrayOf> sd = CoreUtils::ConvertByteArray(serviceData);
    mServiceData->Put(TO_IINTERFACE(serviceDataUuid), TO_IINTERFACE(sd));
    //return this;
    return NOERROR;
}

ECode AdvertiseData::Builder::AddManufacturerData(
    /* [in] */ Int32 manufacturerId,
    /* [in] */ ArrayOf<Byte>* manufacturerSpecificData)
{
    if (manufacturerId < 0) {
        //throw new IllegalArgumentException("invalid manufacturerId - " + manufacturerId);
        Slogger::E("AdvertiseData::Builder::AddManufacturerData", "invalid manufacturerId - %d", manufacturerId);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (manufacturerSpecificData == NULL) {
        //throw new IllegalArgumentException("manufacturerSpecificData is null");
        Slogger::E("AdvertiseData::Builder::AddManufacturerData", "manufacturerSpecificData is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IArrayOf> msd = CoreUtils::ConvertByteArray(manufacturerSpecificData);
    mManufacturerSpecificData->Put(manufacturerId, TO_IINTERFACE(msd));
    //return this;
    return NOERROR;
}

ECode AdvertiseData::Builder::SetIncludeTxPowerLevel(
    /* [in] */ Boolean includeTxPowerLevel)
{
    mIncludeTxPowerLevel = includeTxPowerLevel;
    // return this;
    return NOERROR;
}

ECode AdvertiseData::Builder::SetIncludeDeviceName(
    /* [in] */ Boolean includeDeviceName)
{
    mIncludeDeviceName = includeDeviceName;
    // return this;
    return NOERROR;
}

ECode AdvertiseData::Builder::Build(
    /* [out] */ IAdvertiseData** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    CAdvertiseData::New(mServiceUuids, mManufacturerSpecificData, mServiceData, mIncludeTxPowerLevel, mIncludeDeviceName, result);
    return NOERROR;
}

//AutoPtr<ISparseArray> AdvertiseData::Builder::MiddleInitMmanufacturerspecificdata()
//{
//    // SparseArray<byte[]> result = new SparseArray<byte[]>();
//    AutoPtr<ISparseArray> empty;
//    return empty;
//}
//=====================================================================
//                            AdvertiseData
//=====================================================================
CAR_INTERFACE_IMPL_2(AdvertiseData, Object, IAdvertiseData, IParcelable);

AdvertiseData::AdvertiseData()
{
}

ECode AdvertiseData::GetServiceUuids(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mServiceUuids;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode AdvertiseData::GetManufacturerSpecificData(
    /* [out] */ ISparseArray** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mManufacturerSpecificData;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode AdvertiseData::GetServiceData(
    /* [out] */ IMap** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mServiceData;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode AdvertiseData::GetIncludeTxPowerLevel(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIncludeTxPowerLevel;
    return NOERROR;
}

ECode AdvertiseData::GetIncludeDeviceName(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIncludeDeviceName;
    return NOERROR;
}

ECode AdvertiseData::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    //return Objects.hash(mServiceUuids, mManufacturerSpecificData, mServiceData, mIncludeDeviceName, mIncludeTxPowerLevel);
    AutoPtr<IBoolean> idnBoolean = CoreUtils::Convert(mIncludeDeviceName);
    AutoPtr<IBoolean> itplBoolean = CoreUtils::Convert(mIncludeTxPowerLevel);
    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(5);
    array->Set(0, TO_IINTERFACE(mServiceUuids));
    array->Set(1, TO_IINTERFACE(mManufacturerSpecificData));
    array->Set(2, TO_IINTERFACE(mServiceData));
    array->Set(3, TO_IINTERFACE(idnBoolean));
    array->Set(4, TO_IINTERFACE(itplBoolean));
    *hashCode = Arrays::GetHashCode(array);
    return NOERROR;
}

ECode AdvertiseData::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (TO_IINTERFACE(this) == obj) {
        return TRUE;
    }
    if (obj == NULL || IAdvertiseData::Probe(obj) == NULL/*getClass() != obj.getClass()*/) {
        return FALSE;
    }
    AdvertiseData* other = (AdvertiseData*)(IAdvertiseData::Probe(obj));
    //return Objects.equals(mServiceUuids, other.mServiceUuids) &&
    //        BluetoothLeUtils.equals(mManufacturerSpecificData, other.mManufacturerSpecificData) &&
    //        BluetoothLeUtils.equals(mServiceData, other.mServiceData) &&
    //                mIncludeDeviceName == other.mIncludeDeviceName &&
    //                mIncludeTxPowerLevel == other.mIncludeTxPowerLevel;
    *result = Objects::Equals(mServiceUuids, other->mServiceUuids) &&
        BluetoothLeUtils::Equals(mManufacturerSpecificData, other->mManufacturerSpecificData) &&
        BluetoothLeUtils::Equals(mServiceData, other->mServiceData) &&
        mIncludeDeviceName == other->mIncludeDeviceName &&
        mIncludeTxPowerLevel == other->mIncludeTxPowerLevel;
    return NOERROR;
}

ECode AdvertiseData::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = String("AdvertiseData [mServiceUuids=") //TODO mServiceUuids +
            + String(", mManufacturerSpecificData=")
            + BluetoothLeUtils::ToString(mManufacturerSpecificData)
            + String(", mServiceData=")
            + BluetoothLeUtils::ToString(mServiceData)
            + String(", mIncludeTxPowerLevel=") + (mIncludeTxPowerLevel? String("True") : String("False"))
            + String(", mIncludeDeviceName=") + (mIncludeDeviceName? String("True") : String("False"))
            + String("]");
    return NOERROR;
}

ECode AdvertiseData::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);
    //dest.writeList(mServiceUuids);
    dest->WriteInterfacePtr(TO_IINTERFACE(mServiceUuids.Get()));

    // mManufacturerSpecificData could not be null.
    Int32 msdSize;
    mManufacturerSpecificData->GetSize(&msdSize);
    dest->WriteInt32(msdSize);
    for (Int32 i = 0; i < msdSize; ++i) {
        Int32 keyValue;
        mManufacturerSpecificData->KeyAt(i, &keyValue);
        dest->WriteInt32(keyValue);
        AutoPtr<IInterface> value;
        mManufacturerSpecificData->ValueAt(i, (IInterface**)&value);
        AutoPtr<IArrayOf> data = IArrayOf::Probe(value);
        if (data == NULL) {
            dest->WriteInt32(0);
        } else {
            dest->WriteInt32(1);
            Int32 length;
            data->GetLength(&length);
            dest->WriteInt32(length);
            AutoPtr<ArrayOf<Byte> > arrayByte = ArrayOf<Byte>::Alloc(length);
            for (Int32 i = 0; i < length; ++i) {
                AutoPtr<IInterface> obj;
                data->Get(i, (IInterface**)&obj);
                IByte* iByte = IByte::Probe(obj);
                Byte bValue;
                iByte->GetValue(&bValue);
                arrayByte->Set(i, bValue);
            }
            dest->WriteArrayOf((HANDLE)(arrayByte.Get()));
        }
    }
    Int32 mapSize;
    mServiceData->GetSize(&mapSize);
    dest->WriteInt32(mapSize);
    AutoPtr<ISet> keySet;
    mServiceData->GetKeySet((ISet**)&keySet);
    AutoPtr<ArrayOf<IInterface*> > keyArray;
    keySet->ToArray((ArrayOf<IInterface*>**)&keyArray);
    for (Int32 i = 0; i < keyArray->GetLength(); ++i) {
        //AutoPtr<IParcelUuid> uuid = IParcelUuid::Probe((*keyArray)[i]);
        AutoPtr<IInterface> uuid = (*keyArray)[i];
        //dest.writeParcelable(uuid, flags);
        dest->WriteInterfacePtr(uuid);
        AutoPtr<IInterface> value;
        mServiceData->Get(uuid, (IInterface**)&value);
        AutoPtr<IArrayOf> data = IArrayOf::Probe(value);
        if (data == NULL) {
            dest->WriteInt32(0);
        } else {
            dest->WriteInt32(1);
            Int32 length;
            data->GetLength(&length);
            dest->WriteInt32(length);
            AutoPtr<ArrayOf<Byte> > arrayByte = ArrayOf<Byte>::Alloc(length);
            for (Int32 i = 0; i < length; ++i) {
                AutoPtr<IInterface> obj;
                data->Get(i, (IInterface**)&obj);
                IByte* iByte = IByte::Probe(obj);
                Byte bValue;
                iByte->GetValue(&bValue);
                arrayByte->Set(i, bValue);
            }
            dest->WriteArrayOf((HANDLE)(arrayByte.Get()));
        }
    }
    Boolean bPL = FALSE, bDN = FALSE;
    dest->WriteByte((Byte) ((GetIncludeTxPowerLevel(&bPL), bPL) ? 1 : 0));
    dest->WriteByte((Byte) ((GetIncludeDeviceName(&bDN), bDN) ? 1 : 0));
    return NOERROR;
}

ECode AdvertiseData::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    AutoPtr<Builder> builder = new Builder();
    //List<ParcelUuid> uuids = in.readArrayList(ParcelUuid.class.getClassLoader());
    AutoPtr<IInterface> listInterface;
    in->ReadInterfacePtr((HANDLE*)(IInterface**)&listInterface);
    AutoPtr<IList> uuids = IList::Probe(listInterface.Get());
    Int32 listSize;
    uuids->GetSize(&listSize);
    if (uuids != NULL) {
        for (Int32 i = 0; i < listSize; ++i) {
            AutoPtr<IInterface> obj;
            uuids->Get(i, (IInterface**)&obj);
            AutoPtr<IParcelUuid> uuid = IParcelUuid::Probe(obj);
            builder->AddServiceUuid(uuid);
        }
    }
    Int32 manufacturerSize;
    in->ReadInt32(&manufacturerSize);
    for (Int32 i = 0; i < manufacturerSize; ++i) {
        Int32 manufacturerId;
        in->ReadInt32(&manufacturerId);
        Int32 flags;
        in->ReadInt32(&flags);
        if (flags == 1) {
            Int32 manufacturerDataLength;
            in->ReadInt32(&manufacturerDataLength);
            //byte[] manufacturerData = new byte[manufacturerDataLength];
            AutoPtr<ArrayOf<Byte> > manufacturerData = ArrayOf<Byte>::Alloc(manufacturerDataLength);
            in->ReadArrayOf((HANDLE*)(&manufacturerData));
            builder->AddManufacturerData(manufacturerId, manufacturerData);
        }
    }
    Int32 serviceDataSize;
    in->ReadInt32(&serviceDataSize);
    for (Int32 i = 0; i < serviceDataSize; ++i) {
        //ParcelUuid serviceDataUuid = in.readParcelable( ParcelUuid.class.getClassLoader());
        AutoPtr<IInterface> itfp;
        in->ReadInterfacePtr((HANDLE*)(IInterface**)&itfp);
        AutoPtr<IParcelUuid> serviceDataUuid = IParcelUuid::Probe(itfp);
        Int32 flag;
        in->ReadInt32(&flag);
        if (flag == 1) {
            Int32 serviceDataLength;
            in->ReadInt32(&serviceDataLength);
            //byte[] serviceData = new byte[serviceDataLength];
            AutoPtr<ArrayOf<Byte> > serviceData = ArrayOf<Byte>::Alloc(serviceDataLength);
            //in->ReadByteArray((HANDLE*)&serviceData);
            in->ReadArrayOf((HANDLE*)&serviceData);
            builder->AddServiceData(serviceDataUuid, serviceData);
        }
    }
    Byte bTRL = 0, bDN = 0;
    in->ReadByte(&bTRL);
    in->ReadByte(&bDN);
    builder->SetIncludeTxPowerLevel(bTRL == 1);
    builder->SetIncludeDeviceName(bDN == 1);
    //AutoPtr<IAdvertiseData> ad;
    //builder->Build((IAdvertiseData**)&ad);
    this->mServiceUuids = builder->mServiceUuids;
    this->mManufacturerSpecificData = builder->mManufacturerSpecificData;
    this->mServiceData = builder->mServiceData;
    this->mIncludeTxPowerLevel = builder->mIncludeTxPowerLevel;
    this->mIncludeDeviceName = builder->mIncludeDeviceName;
    return NOERROR;
}

ECode AdvertiseData::constructor()
{
    return NOERROR;
}

ECode AdvertiseData::constructor(
    /* [in] */ IParcel* source)
{
    ReadFromParcel(source);
    return NOERROR;
}

ECode AdvertiseData::constructor(
    /* [in] */ IList* serviceUuids,
    /* [in] */ ISparseArray* manufacturerData,
    /* [in] */ IMap* serviceData,
    /* [in] */ Boolean includeTxPowerLevel,
    /* [in] */ Boolean includeDeviceName)
{
    mServiceUuids = serviceUuids;
    mManufacturerSpecificData = manufacturerData;
    mServiceData = serviceData;
    mIncludeTxPowerLevel = includeTxPowerLevel;
    mIncludeDeviceName = includeDeviceName;
    return NOERROR;
}

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
