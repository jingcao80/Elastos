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

#include "Elastos.Droid.Os.h"
#include "elastos/droid/bluetooth/le/ScanRecord.h"
#include "elastos/droid/bluetooth/le/BluetoothLeUtils.h"
#include "elastos/droid/bluetooth/BluetoothUuid.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/core/Math.h"

using Elastos::Droid::Bluetooth::BluetoothUuid;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

//=====================================================================
//                              ScanRecord
//=====================================================================
CAR_INTERFACE_IMPL(ScanRecord, Object, IScanRecord);

const String ScanRecord::TAG("ScanRecord");
const Int32 ScanRecord::DATA_TYPE_FLAGS;
const Int32 ScanRecord::DATA_TYPE_SERVICE_UUIDS_16_BIT_PARTIAL;
const Int32 ScanRecord::DATA_TYPE_SERVICE_UUIDS_16_BIT_COMPLETE;
const Int32 ScanRecord::DATA_TYPE_SERVICE_UUIDS_32_BIT_PARTIAL;
const Int32 ScanRecord::DATA_TYPE_SERVICE_UUIDS_32_BIT_COMPLETE;
const Int32 ScanRecord::DATA_TYPE_SERVICE_UUIDS_128_BIT_PARTIAL;
const Int32 ScanRecord::DATA_TYPE_SERVICE_UUIDS_128_BIT_COMPLETE;
const Int32 ScanRecord::DATA_TYPE_LOCAL_NAME_SHORT;
const Int32 ScanRecord::DATA_TYPE_LOCAL_NAME_COMPLETE;
const Int32 ScanRecord::DATA_TYPE_TX_POWER_LEVEL;
const Int32 ScanRecord::DATA_TYPE_SERVICE_DATA;
const Int32 ScanRecord::DATA_TYPE_MANUFACTURER_SPECIFIC_DATA;

ScanRecord::ScanRecord()
{
}

ECode ScanRecord::GetAdvertiseFlags(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAdvertiseFlags;
    return NOERROR;
}

ECode ScanRecord::GetServiceUuids(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mServiceUuids;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ScanRecord::GetManufacturerSpecificData(
    /* [out] */ ISparseArray** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mManufacturerSpecificData;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ScanRecord::GetManufacturerSpecificData(
    /* [in] */ Int32 manufacturerId,
    /* [out, callee] */ ArrayOf<Byte>** data)
{
    VALIDATE_NOT_NULL(data);
    AutoPtr<IInterface> obj;
    mManufacturerSpecificData->Get(manufacturerId, (IInterface**)&obj);
    IArrayOf* iArray = IArrayOf::Probe(obj);
    Int32 size;
    iArray->GetLength(&size);
    AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(size);
    for (Int32 j = 0; j < size; ++j) {
        AutoPtr<IInterface> obj;
        iArray->Get(j, (IInterface**)&obj);
        IByte* iByte = IByte::Probe(obj);
        Byte bValue;
        iByte->GetValue(&bValue);
        array->Set(j, bValue);
    }
    *data = array;
    REFCOUNT_ADD(*data);

    return NOERROR;
}

ECode ScanRecord::GetServiceData(
    /* [out] */ IMap** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mServiceData;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ScanRecord::GetServiceData(
    /* [in] */ IParcelUuid* serviceDataUuid,
    /* [out, callee] */ ArrayOf<Byte>** data)
{
    VALIDATE_NOT_NULL(data);
    if (serviceDataUuid == NULL) {
        *data = NULL;
        return NOERROR;
    }
    AutoPtr<IInterface> obj;
    mServiceData->Get(TO_IINTERFACE(serviceDataUuid), (IInterface**)&obj);
    IArrayOf* iArray = IArrayOf::Probe(obj);
    Int32 size;
    iArray->GetLength(&size);
    AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(size);
    for (Int32 j = 0; j < size; ++j) {
        AutoPtr<IInterface> obj;
        iArray->Get(j, (IInterface**)&obj);
        IByte* iByte = IByte::Probe(obj);
        Byte bValue;
        iByte->GetValue(&bValue);
        array->Set(j, bValue);
    }
    *data = array;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

ECode ScanRecord::GetTxPowerLevel(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTxPowerLevel;
    return NOERROR;
}

ECode ScanRecord::GetDeviceName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDeviceName;
    return NOERROR;
}

ECode ScanRecord::GetBytes(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBytes;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

AutoPtr<IScanRecord> ScanRecord::ParseFromBytes(
    /* [in] */ ArrayOf<Byte>* scanRecord)
{
    if (scanRecord == NULL) {
        return NULL;
    }

    Int32 currentPos = 0;
    Int32 advertiseFlag = -1;
    //List<ParcelUuid> serviceUuids = new ArrayList<ParcelUuid>();
    AutoPtr<IList> serviceUuids;
    CArrayList::New((IList**)&serviceUuids);
    String localName;;
    Int32 txPowerLevel = Elastos::Core::Math::INT32_MIN_VALUE;

    //SparseArray<byte[]> manufacturerData = new SparseArray<byte[]>();
    AutoPtr<ISparseArray> manufacturerData;
    CSparseArray::New((ISparseArray**)&manufacturerData);
    //Map<ParcelUuid, byte[]> serviceData = new ArrayMap<ParcelUuid, byte[]>();
    AutoPtr<IMap> serviceData;
    CArrayMap::New((IMap**)&serviceData);

    //try {
    Int32 length = scanRecord->GetLength();
    while (currentPos < length) {
        // length is unsigned int.
        Int32 length = (*scanRecord)[currentPos++] & 0xFF;
        if (length == 0) {
            break;
        }
        // Note the length includes the length of the field type itself.
        Int32 dataLength = length - 1;
        // fieldType is unsigned int.
        Int32 fieldType = (*scanRecord)[currentPos++] & 0xFF;
        switch (fieldType) {
            case DATA_TYPE_FLAGS:
                advertiseFlag = (*scanRecord)[currentPos] & 0xFF;
                break;
            case DATA_TYPE_SERVICE_UUIDS_16_BIT_PARTIAL:
            case DATA_TYPE_SERVICE_UUIDS_16_BIT_COMPLETE:
                ParseServiceUuid(scanRecord, currentPos,
                        dataLength, BluetoothUuid::UUID_BYTES_16_BIT, serviceUuids);
                break;
            case DATA_TYPE_SERVICE_UUIDS_32_BIT_PARTIAL:
            case DATA_TYPE_SERVICE_UUIDS_32_BIT_COMPLETE:
                ParseServiceUuid(scanRecord, currentPos, dataLength,
                        BluetoothUuid::UUID_BYTES_32_BIT, serviceUuids);
                break;
            case DATA_TYPE_SERVICE_UUIDS_128_BIT_PARTIAL:
            case DATA_TYPE_SERVICE_UUIDS_128_BIT_COMPLETE:
                ParseServiceUuid(scanRecord, currentPos, dataLength,
                        BluetoothUuid::UUID_BYTES_128_BIT, serviceUuids);
                break;
            case DATA_TYPE_LOCAL_NAME_SHORT:
            case DATA_TYPE_LOCAL_NAME_COMPLETE:
            {
                AutoPtr<ArrayOf<Byte> > bytes =
                        ExtractBytes(scanRecord, currentPos, dataLength);
                localName = String(*(bytes.Get()));
                break;
            }
            case DATA_TYPE_TX_POWER_LEVEL:
                txPowerLevel = (*scanRecord)[currentPos];
                break;
            case DATA_TYPE_SERVICE_DATA:
            {
                // The first two bytes of the service data are service data UUID in little
                // endian. The rest bytes are service data.
                Int32 serviceUuidLength = BluetoothUuid::UUID_BYTES_16_BIT;
                AutoPtr<ArrayOf<Byte> > serviceDataUuidBytes = ExtractBytes(scanRecord, currentPos,
                        serviceUuidLength);
                AutoPtr<IParcelUuid> serviceDataUuid = BluetoothUuid::ParseUuidFrom(
                        serviceDataUuidBytes);
                AutoPtr<ArrayOf<Byte> > serviceDataArray = ExtractBytes(scanRecord,
                        currentPos + serviceUuidLength, dataLength - serviceUuidLength);

                AutoPtr<IArrayOf> iArrayOf = CoreUtils::ConvertByteArray(serviceDataArray);
                serviceData->Put(TO_IINTERFACE(serviceDataUuid), TO_IINTERFACE(iArrayOf));
                break;
            }
            case DATA_TYPE_MANUFACTURER_SPECIFIC_DATA:
            {
                // The first two bytes of the manufacturer specific data are
                // manufacturer ids in little endian.
                Int32 manufacturerId = (((*scanRecord)[currentPos + 1] & 0xFF) << 8) +
                    ((*scanRecord)[currentPos] & 0xFF);
                AutoPtr<ArrayOf<Byte> > manufacturerDataBytes = ExtractBytes(scanRecord, currentPos + 2,
                        dataLength - 2);
                AutoPtr<IArrayOf> iArrayOf = CoreUtils::ConvertByteArray(manufacturerDataBytes);
                manufacturerData->Put(manufacturerId, TO_IINTERFACE(iArrayOf));
                break;
            }
            default:
                // Just ignore, we don't handle such data type.
                break;
        }
        currentPos += dataLength;
    }

    Boolean isEmpty;
    if (serviceUuids->IsEmpty(&isEmpty), isEmpty) {
        serviceUuids = NULL;
    }
    return new ScanRecord(serviceUuids, manufacturerData, serviceData,
                advertiseFlag, txPowerLevel, localName, scanRecord);
    //} catch (Exception e) {
    //TODO need check the error code
    //    Log.e(TAG, "unable to parse scan record: " + Arrays.toString(scanRecord));
    //    // As the record is invalid, ignore all the parsed results for this packet
    //    // and return an empty record with raw scanRecord bytes in results
    //    return new ScanRecord(null, null, null, -1, Integer.MIN_VALUE, null, scanRecord);
    //}
}

ECode ScanRecord::ToString(
    /* [out] */ String* info)
{
    *info = String("ScanRecord [mAdvertiseFlags=") + StringUtils::ToString(mAdvertiseFlags) + String(", mServiceUuids=list")// + mServiceUuids
             + String(", mManufacturerSpecificData=") + BluetoothLeUtils::ToString(mManufacturerSpecificData)
             + String(", mServiceData=") + BluetoothLeUtils::ToString(mServiceData)
             + String(", mTxPowerLevel=") + StringUtils::ToString(mTxPowerLevel) + String(", mDeviceName=") + mDeviceName + String("]");
    return NOERROR;
}

ScanRecord::ScanRecord(
    /* [in] */ IList* serviceUuids,
    /* [in] */ ISparseArray* manufacturerData,
    /* [in] */ IMap* serviceData,
    /* [in] */ Int32 advertiseFlags,
    /* [in] */ Int32 txPowerLevel,
    /* [in] */ const String& localName,
    /* [in] */ ArrayOf<Byte>* bytes)
{
    mServiceUuids = serviceUuids;
    mManufacturerSpecificData = manufacturerData;
    mServiceData = serviceData;
    mDeviceName = localName;
    mAdvertiseFlags = advertiseFlags;
    mTxPowerLevel = txPowerLevel;
    mBytes = bytes;
}

Int32 ScanRecord::ParseServiceUuid(
    /* [in] */ ArrayOf<Byte>* scanRecord,
    /* [in] */ Int32 currentPos,
    /* [in] */ Int32 dataLength,
    /* [in] */ Int32 uuidLength,
    /* [in] */ IList* serviceUuids)
{
    while (dataLength > 0) {
        AutoPtr<ArrayOf<Byte> > uuidBytes
            = ExtractBytes(scanRecord, currentPos, uuidLength);
        serviceUuids->Add(TO_IINTERFACE(BluetoothUuid::ParseUuidFrom(uuidBytes)));
        dataLength -= uuidLength;
        currentPos += uuidLength;
    }
    return currentPos;
}

AutoPtr<ArrayOf<Byte> > ScanRecord::ExtractBytes(
    /* [in] */ ArrayOf<Byte>* scanRecord,
    /* [in] */ Int32 start,
    /* [in] */ Int32 length)
{
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(length);
    //System.arraycopy(scanRecord, start, bytes, 0, length);
    Int32 size = scanRecord->GetLength();
    size = size - start + 1;
    if (length > size)
        length = size;
    for (Int32 i = 0; i < length; ++i) {
        bytes->Set(i, (*scanRecord)[i+start]);
    }

    return bytes;
}

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
