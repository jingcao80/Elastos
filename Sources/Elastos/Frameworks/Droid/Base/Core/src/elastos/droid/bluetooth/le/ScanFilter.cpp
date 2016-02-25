#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/bluetooth/le/ScanFilter.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/utility/Objects.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Bluetooth::CBluetoothAdapter;
using Elastos::Utility::Objects;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

//=====================================================================
//                         ScanFilter::Builder
//=====================================================================
CAR_INTERFACE_IMPL(ScanFilter::Builder, Object, IScanFilterBuilder);

ScanFilter::Builder::Builder()
{
}

ECode ScanFilter::Builder::SetDeviceName(
    /* [in] */ const String& deviceName)
{
    mDeviceName = deviceName;
    // return this;
    return NOERROR;
}

ECode ScanFilter::Builder::SetDeviceAddress(
    /* [in] */ const String& deviceAddress)
{
    if (deviceAddress.IsNullOrEmpty() && !CBluetoothAdapter::CheckBluetoothAddress(deviceAddress)) {
        //throw new IllegalArgumentException("invalid device address " + deviceAddress);
        Logger::E("ScanFilter::Builder::SetDeviceAddress", "invalid device address %s", deviceAddress.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mDeviceAddress = deviceAddress;
    //return this;
    return NOERROR;
}

ECode ScanFilter::Builder::SetServiceUuid(
    /* [in] */ IParcelUuid* serviceUuid)
{
    mServiceUuid = serviceUuid;
    mUuidMask = NULL; // clear uuid mask
    //return this;
    return NOERROR;
}

ECode ScanFilter::Builder::SetServiceUuid(
    /* [in] */ IParcelUuid* serviceUuid,
    /* [in] */ IParcelUuid* uuidMask)
{
    if (mUuidMask != NULL && mServiceUuid == NULL) {
        //throw new IllegalArgumentException("uuid is null while uuidMask is not null!");
        Logger::E("ScanFilter::Builder::SetServiceUuid", "uuid is null while uuidMask is not null!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mServiceUuid = serviceUuid;
    mUuidMask = uuidMask;
    // return this;
    return NOERROR;
}

ECode ScanFilter::Builder::SetServiceData(
    /* [in] */ IParcelUuid* serviceDataUuid,
    /* [in] */ ArrayOf<Byte>* serviceData)
{
    if (serviceDataUuid == NULL) {
        //throw new IllegalArgumentException("serviceDataUuid is null");
        Logger::E("ScanFilter::Builder::SetServiceData", "serviceDataUuid is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mServiceDataUuid = serviceDataUuid;
    mServiceData = serviceData;
    mServiceDataMask = NULL; // clear service data mask
    // return this;
    return NOERROR;
}

ECode ScanFilter::Builder::SetServiceData(
    /* [in] */ IParcelUuid* serviceDataUuid,
    /* [in] */ ArrayOf<Byte>* serviceData,
    /* [in] */ ArrayOf<Byte>* serviceDataMask)
{
    if (serviceDataUuid == NULL) {
        //throw new IllegalArgumentException("serviceDataUuid is null");
        Logger::E("ScanFilter::Builder::SetServiceData", "serviceDataUuid is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mServiceDataMask != NULL) {
        if (mServiceData == NULL) {
            //throw new IllegalArgumentException(
            //        "serviceData is null while serviceDataMask is not null");
            Logger::E("ScanFilter::Builder::SetServiceData", "serviceData is null while serviceDataMask is not null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        // Since the mServiceDataMask is a bit mask for mServiceData, the lengths of the two
        // byte array need to be the same.
        if (mServiceData->GetLength() != mServiceDataMask->GetLength()) {
            //throw new IllegalArgumentException(
            //        "size mismatch for service data and service data mask");
            Logger::E("ScanFilter::Builder::SetServiceData",
                    "size mismatch for service data and service data mask");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    mServiceDataUuid = serviceDataUuid;
    mServiceData = serviceData;
    mServiceDataMask = serviceDataMask;
    // return this;
    return NOERROR;
}

ECode ScanFilter::Builder::SetManufacturerData(
    /* [in] */ Int32 manufacturerId,
    /* [in] */ ArrayOf<Byte>* manufacturerData)
{
    if (manufacturerData != NULL && manufacturerId < 0) {
        //throw new IllegalArgumentException("invalid manufacture id");
        Logger::E("ScanFilter::Builder::SetManufacturerData", "invalid manufacture id");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mManufacturerId = manufacturerId;
    mManufacturerData = manufacturerData;
    mManufacturerDataMask = NULL; // clear manufacturer data mask
    // return this;
    return NOERROR;
}

ECode ScanFilter::Builder::SetManufacturerData(
    /* [in] */ Int32 manufacturerId,
    /* [in] */ ArrayOf<Byte>* manufacturerData,
    /* [in] */ ArrayOf<Byte>* manufacturerDataMask)
{
    if (manufacturerData != NULL && manufacturerId < 0) {
        //throw new IllegalArgumentException("invalid manufacture id");
        Logger::E("ScanFilter::Builder::SetManufacturerData", "invalid manufacture id");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mManufacturerDataMask != NULL) {
        if (mManufacturerData == NULL) {
            //throw new IllegalArgumentException(
            //        "manufacturerData is null while manufacturerDataMask is not null");
            Logger::E("ScanFilter::Builder::SetManufacturerData",
                    "manufacturerData is null while manufacturerDataMask is not null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        // Since the mManufacturerDataMask is a bit mask for mManufacturerData, the lengths
        // of the two byte array need to be the same.
        if (mManufacturerData->GetLength() != mManufacturerDataMask->GetLength()) {
            //throw new IllegalArgumentException(
            //        "size mismatch for manufacturerData and manufacturerDataMask");
            Logger::E("ScanFilter::Builder::SetManufacturerData",
                    "size mismatch for manufacturerData and manufacturerDataMask");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    mManufacturerId = manufacturerId;
    mManufacturerData = manufacturerData;
    mManufacturerDataMask = manufacturerDataMask;
    // return this;
    return NOERROR;
}

ECode ScanFilter::Builder::Build(
    /* [out] */ IScanFilter** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IScanFilter> scanFilter = new ScanFilter(mDeviceName, mDeviceAddress,
            mServiceUuid, mUuidMask,
            mServiceDataUuid, mServiceData, mServiceDataMask,
            mManufacturerId, mManufacturerData, mManufacturerDataMask);

    *result = scanFilter;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(ScanFilter, Object, IScanFilter, IParcelable);

ScanFilter::ScanFilter()
{
}

ECode ScanFilter::constructor(
    /* [in] */ IParcel* source)
{
    ReadFromParcel(source);
    return NOERROR;
}

ECode ScanFilter::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mDeviceName.IsNullOrEmpty()? 0 : 1);
    if (!mDeviceName.IsNullOrEmpty()) {
        dest->WriteString(mDeviceName);
    }
    dest->WriteInt32(mDeviceAddress.IsNullOrEmpty() ? 0 : 1);
    if (!mDeviceAddress.IsNullOrEmpty()) {
        dest->WriteString(mDeviceAddress);
    }
    dest->WriteInt32(mServiceUuid == NULL? 0 : 1);
    if (mServiceUuid != NULL) {
        //dest->WriteParcelable(mServiceUuid, flags);
        dest->WriteInterfacePtr(TO_IINTERFACE(mServiceUuid));
        dest->WriteInt32(mServiceUuidMask == NULL? 0 : 1);
        if (mServiceUuidMask != NULL) {
            //dest.writeParcelable(mServiceUuidMask, flags);
            dest->WriteInterfacePtr(TO_IINTERFACE(mServiceUuidMask));
        }
    }
    dest->WriteInt32(mServiceDataUuid == NULL? 0 : 1);
    if (mServiceDataUuid != NULL) {
        //dest->WriteParcelable(mServiceDataUuid, flags);
        dest->WriteInterfacePtr(TO_IINTERFACE(mServiceDataUuid));
        dest->WriteInt32(mServiceData == NULL? 0 : 1);
        if (mServiceData != NULL) {
            dest->WriteInt32(mServiceData->GetLength());
            //dest.writeByteArray(mServiceData);
            dest->WriteArrayOf((Handle32)(mServiceData.Get()));

            dest->WriteInt32(mServiceDataMask == NULL? 0 : 1);
            if (mServiceDataMask != NULL) {
                dest->WriteInt32(mServiceDataMask->GetLength());
                //dest->WriteByteArray(mServiceDataMask);
                dest->WriteArrayOf((Handle32)(mServiceDataMask.Get()));
            }
        }
    }
    dest->WriteInt32(mManufacturerId);
    dest->WriteInt32(mManufacturerData == NULL? 0 : 1);
    if (mManufacturerData != NULL) {
        dest->WriteInt32(mManufacturerData->GetLength());
        //dest.writeByteArray(mManufacturerData);
        dest->WriteArrayOf((Handle32)(mManufacturerData.Get()));

        dest->WriteInt32(mManufacturerDataMask == NULL? 0 : 1);
        if (mManufacturerDataMask != NULL) {
            dest->WriteInt32(mManufacturerDataMask->GetLength());
            //dest->writeByteArray(mManufacturerDataMask);
            dest->WriteArrayOf((Handle32)(mManufacturerDataMask.Get()));
        }
    }
    return NOERROR;
}

ECode ScanFilter::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    //Builder builder = new Builder();
    Int32 flag = 0;
    if ((in->ReadInt32(&flag), flag) == 1) {
        //builder.setDeviceName(in.readString());
        in->ReadString(&mDeviceName);
    }
    flag = 0;
    if ((in->ReadInt32(&flag), flag) == 1) {
        //builder.setDeviceAddress(in.readString());
        in->ReadString(&mDeviceAddress);
    }
    if ((in->ReadInt32(&flag), flag) == 1) {
        //ParcelUuid uuid = in.readParcelable(ParcelUuid.class.getClassLoader());
        AutoPtr<IInterface> obj;
        in->ReadInterfacePtr((Handle32*)((IInterface**)&obj));
        mServiceUuid = IParcelUuid::Probe(obj);
        //builder.setServiceUuid(uuid);
        flag = 0;
        if ((in->ReadInt32(&flag), flag) == 1) {
            //ParcelUuid uuidMask = in.readParcelable(ParcelUuid.class.getClassLoader());
            //builder.setServiceUuid(uuid, uuidMask);
            AutoPtr<IInterface> obj;
            in->ReadInterfacePtr((Handle32*)((IInterface**)&obj));
            mServiceUuidMask = IParcelUuid::Probe(obj);
        }
    }
    flag = 0;
    if ((in->ReadInt32(&flag), flag) == 1) {
        //ParcelUuid servcieDataUuid = in.readParcelable(ParcelUuid.class.getClassLoader());
        AutoPtr<IInterface> obj;
        in->ReadInterfacePtr((Handle32*)((IInterface**)&obj));
        mServiceDataUuid = IParcelUuid::Probe(obj);
        flag = 0;
        if ((in->ReadInt32(&flag), flag) == 1) {
            Int32 serviceDataLength;
            in->ReadInt32(&serviceDataLength);
            //byte[] serviceData = new byte[serviceDataLength];
            //in.readByteArray(serviceData);
            mServiceData = NULL;
            in->ReadArrayOf((Handle32*)&mServiceData);
            flag = 0;
            if ((in->ReadInt32(&flag), flag) == 0) {
                //builder.setServiceData(servcieDataUuid, serviceData);
                mServiceDataMask = NULL;
            } else {
                Int32 serviceDataMaskLength;
                in->ReadInt32(&serviceDataMaskLength);
                //byte[] serviceDataMask = new byte[serviceDataMaskLength];
                //in.readByteArray(serviceDataMask);
                //builder.setServiceData(
                //        servcieDataUuid, serviceData, serviceDataMask);
                mServiceDataMask = NULL;
                in->ReadArrayOf((Handle32*)&mServiceDataMask);
            }
        }
    }

    //int manufacturerId = in.readInt();
    in->ReadInt32(&mManufacturerId);
    flag = 0;
    if ((in->ReadInt32(&flag), flag) == 1) {
        Int32 manufacturerDataLength;
        in->ReadInt32(&manufacturerDataLength);
        //byte[] manufacturerData = new byte[manufacturerDataLength];
        //in.readByteArray(manufacturerData);
        mManufacturerData = NULL;
        in->ReadArrayOf((Handle32*)&mManufacturerData);
        flag = 0;
        if ((in->ReadInt32(&flag), flag) == 0) {
            //builder.setManufacturerData(manufacturerId, manufacturerData);
            mManufacturerDataMask = NULL;
        } else {
            Int32 manufacturerDataMaskLength;
            in->ReadInt32(&manufacturerDataMaskLength);
            //byte[] manufacturerDataMask = new byte[manufacturerDataMaskLength];
            //in.readByteArray(manufacturerDataMask);
            //builder.setManufacturerData(manufacturerId, manufacturerData,
            //        manufacturerDataMask);
            in->ReadArrayOf((Handle32*)&mManufacturerDataMask);
        }
    }

    //return builder.build();
    return NOERROR;
}

ECode ScanFilter::GetDeviceName(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = mDeviceName;
    return NOERROR;
}

ECode ScanFilter::GetServiceUuid(
    /* [out] */ IParcelUuid** uuid)
{
    VALIDATE_NOT_NULL(uuid);
    *uuid = mServiceUuid;
    REFCOUNT_ADD(*uuid);
    return NOERROR;
}

ECode ScanFilter::GetServiceUuidMask(
    /* [out] */ IParcelUuid** uuid)
{
    VALIDATE_NOT_NULL(uuid);
    *uuid = mServiceUuidMask;
    REFCOUNT_ADD(*uuid);
    return NOERROR;
}

ECode ScanFilter::GetDeviceAddress(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = mDeviceAddress;
    return NOERROR;
}

ECode ScanFilter::GetServiceData(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mServiceData;
    REFCOUNT_ADD(*result);
    return NOERROR;;
}

ECode ScanFilter::GetServiceDataMask(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mServiceDataMask;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ScanFilter::GetServiceDataUuid(
    /* [out] */ IParcelUuid** uuid)
{
    VALIDATE_NOT_NULL(uuid);
    *uuid = mServiceDataUuid;
    REFCOUNT_ADD(*uuid);
    return NOERROR;
}

ECode ScanFilter::GetManufacturerId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mManufacturerId;
    return NOERROR;
}

ECode ScanFilter::GetManufacturerData(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mManufacturerData;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ScanFilter::GetManufacturerDataMask(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mManufacturerDataMask;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ScanFilter::Matches(
    /* [in] */ IScanResult* scanResult,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (scanResult == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IBluetoothDevice> device;
    scanResult->GetDevice((IBluetoothDevice**)&device);
    // Device match.
    String address;
    if (mDeviceAddress != NULL
            && (device == NULL || !mDeviceAddress.Equals((device->GetAddress(&address), address)))) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IScanRecord> scanRecord;
    scanResult->GetScanRecord((IScanRecord**)&scanResult);

    // Scan record is null but there exist filters on it.
    if (scanRecord == NULL
            && (!mDeviceName.IsNullOrEmpty() || mServiceUuid != NULL|| mManufacturerData != NULL
                    || mServiceData != NULL)) {
        *result = FALSE;
        return NOERROR;
    }

    // Local name match.
    String deviceName;
    if (!mDeviceName.IsNullOrEmpty() && !mDeviceName.Equals((scanRecord->GetDeviceName(&deviceName), deviceName))) {
        *result = FALSE;
        return NOERROR;
    }

    // UUID match.
    AutoPtr<IList> uuids;
    scanRecord->GetServiceUuids((IList**)&uuids);
    if (mServiceUuid != NULL && !MatchesServiceUuids(mServiceUuid, mServiceUuidMask, uuids)) {
        *result = FALSE;
        return NOERROR;
    }

    // Service data match
    AutoPtr<ArrayOf<Byte> > serviceData;
    scanRecord->GetServiceData(mServiceDataUuid, (ArrayOf<Byte>**)&serviceData);
    if (mServiceDataUuid != NULL) {
        if (!MatchesPartialData(mServiceData, mServiceDataMask, serviceData)) {
            *result = FALSE;
            return NOERROR;
        }
    }

    // Manufacturer data match.
    AutoPtr<ArrayOf<Byte> > msData;
    scanRecord->GetManufacturerSpecificData(mManufacturerId, (ArrayOf<Byte>**)&msData);
    if (mManufacturerId >= 0) {
        if (!MatchesPartialData(mManufacturerData, mManufacturerDataMask, msData)) {
            *result = FALSE;
            return NOERROR;
        }
    }
    // All filters match.
    *result = TRUE;
    return NOERROR;
}

ECode ScanFilter::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = String("BluetoothLeScanFilter [mDeviceName=") + mDeviceName + String(", mDeviceAddress=")
            + mDeviceAddress;
    //TODO
            //+ ", mUuid=" + mServiceUuid + ", mUuidMask=" + mServiceUuidMask
            //+ ", mServiceDataUuid=" + Objects.toString(mServiceDataUuid) + ", mServiceData="
            //+ Arrays.toString(mServiceData) + ", mServiceDataMask="
            //+ Arrays.toString(mServiceDataMask) + ", mManufacturerId=" + mManufacturerId
            //+ ", mManufacturerData=" + Arrays.toString(mManufacturerData)
            //+ ", mManufacturerDataMask=" + Arrays.toString(mManufacturerDataMask) + "]";
    return NOERROR;
}

ECode ScanFilter::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    //TODO
    //*hashCode = Objects.hash(mDeviceName, mDeviceAddress, mManufacturerId, mManufacturerData,
    //        mManufacturerDataMask, mServiceDataUuid, mServiceData, mServiceDataMask,
    //        mServiceUuid, mServiceUuidMask);
    assert(0);
    return NOERROR;
}

ECode ScanFilter::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    if (TO_IINTERFACE(this) == obj) {
        return TRUE;
    }
    //if (obj == NULL|| getClass() != obj.getClass())
    if (obj == NULL|| IScanFilter::Probe(obj) == NULL) {
        return FALSE;
    }
    AutoPtr<ScanFilter> other = (ScanFilter*)(IScanFilter::Probe(obj));
    //return Objects.equals(mDeviceName, other.mDeviceName) &&
    //        Objects.equals(mDeviceAddress, other.mDeviceAddress) &&
    //                mManufacturerId == other.mManufacturerId &&
    //        Objects.deepEquals(mManufacturerData, other.mManufacturerData) &&
    //        Objects.deepEquals(mManufacturerDataMask, other.mManufacturerDataMask) &&
    //        Objects.deepEquals(mServiceDataUuid, other.mServiceDataUuid) &&
    //        Objects.deepEquals(mServiceData, other.mServiceData) &&
    //        Objects.deepEquals(mServiceDataMask, other.mServiceDataMask) &&
    //        Objects.equals(mServiceUuid, other.mServiceUuid) &&
    //        Objects.equals(mServiceUuidMask, other.mServiceUuidMask);
    AutoPtr<IArrayOf> md1 = CoreUtils::ConvertByteArray(mManufacturerData);
    AutoPtr<IArrayOf> md2 = CoreUtils::ConvertByteArray(other->mManufacturerData);
    AutoPtr<IArrayOf> mdm1 = CoreUtils::ConvertByteArray(mManufacturerDataMask);
    AutoPtr<IArrayOf> mdm2 = CoreUtils::ConvertByteArray(other->mManufacturerDataMask);
    AutoPtr<IArrayOf> sd1 = CoreUtils::ConvertByteArray(mServiceData);
    AutoPtr<IArrayOf> sd2 = CoreUtils::ConvertByteArray(other->mServiceData);
    AutoPtr<IArrayOf> sdm1 = CoreUtils::ConvertByteArray(mServiceDataMask);
    AutoPtr<IArrayOf> sdm2 = CoreUtils::ConvertByteArray(other->mServiceDataMask);
    *result =
            mDeviceName.Equals(other->mDeviceName) &&
            mDeviceAddress.Equals(other->mDeviceAddress) &&
            mManufacturerId == other->mManufacturerId &&
            Objects::Equals(mServiceDataUuid, other->mServiceDataUuid) &&//need check, no DeepEquals for IParcelUuid

            Objects::DeepEquals(md1, md2) &&
            Objects::DeepEquals(mdm1, mdm2) &&
            Objects::DeepEquals(sd1, sd2) &&
            Objects::DeepEquals(sdm1, sdm2) &&

            Objects::Equals(mServiceUuid, other->mServiceUuid) &&
            Objects::Equals(mServiceUuidMask, other->mServiceUuidMask);

    return NOERROR;
}

ScanFilter::ScanFilter(
    /* [in] */ const String& name,
    /* [in] */ const String& deviceAddress,
    /* [in] */ IParcelUuid* uuid,
    /* [in] */ IParcelUuid* uuidMask,
    /* [in] */ IParcelUuid* serviceDataUuid,
    /* [in] */ ArrayOf<Byte>* serviceData,
    /* [in] */ ArrayOf<Byte>* serviceDataMask,
    /* [in] */ Int32 manufacturerId,
    /* [in] */ ArrayOf<Byte>* manufacturerData,
    /* [in] */ ArrayOf<Byte>* manufacturerDataMask)
{
    mDeviceName = name;
    mServiceUuid = uuid;
    mServiceUuidMask = uuidMask;
    mDeviceAddress = deviceAddress;
    mServiceDataUuid = serviceDataUuid;
    mServiceData = serviceData;
    mServiceDataMask = serviceDataMask;
    mManufacturerId = manufacturerId;
    mManufacturerData = manufacturerData;
    mManufacturerDataMask = manufacturerDataMask;
}

Boolean ScanFilter::MatchesServiceUuids(
    /* [in] */ IParcelUuid* uuid,
    /* [in] */ IParcelUuid* parcelUuidMask,
    /* [in] */ IList* uuids)
{
    if (uuid == NULL) {
        return TRUE;
    }
    if (uuids == NULL) {
        return FALSE;
    }

    //for (ParcelUuid parcelUuid : uuids)
    Int32 size;
    uuids->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        uuids->Get(i, (IInterface**)&obj);
        IParcelUuid* parcelUuid = IParcelUuid::Probe(obj);
        AutoPtr<IUUID> uuidMask;
        if (parcelUuidMask != NULL) {
            parcelUuidMask->GetUuid((IUUID**)&uuidMask);
        }
        AutoPtr<IUUID> iuuid, data;
        uuid->GetUuid((IUUID**)&iuuid);
        parcelUuid->GetUuid((IUUID**)&data);
        if (MatchesServiceUuid(iuuid, uuidMask, data)) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean ScanFilter::MatchesServiceUuid(
    /* [in] */ IUUID* uuid,
    /* [in] */ IUUID* mask,
    /* [in] */ IUUID* data)
{
    if (mask == NULL) {
        Boolean result;
        uuid->Equals(TO_IINTERFACE(data), &result);
        return result;
    }
    Int64 uLeastSignificantBits, mLeastSignificantBits, dLeastSignificantBits;
    uuid->GetLeastSignificantBits(&uLeastSignificantBits);
    mask->GetLeastSignificantBits(&mLeastSignificantBits);
    data->GetLeastSignificantBits(&dLeastSignificantBits);
    if ((uLeastSignificantBits & mLeastSignificantBits) !=
            (dLeastSignificantBits & mLeastSignificantBits)) {
        return FALSE;
    }
    Int64 uMostSignificantBits, mMostSignificantBits, dMostSignificantBits;
    uuid->GetMostSignificantBits(&uMostSignificantBits);
    mask->GetMostSignificantBits(&mMostSignificantBits);
    data->GetMostSignificantBits(&dMostSignificantBits);
    return ((uMostSignificantBits & mMostSignificantBits) ==
            (dMostSignificantBits & mMostSignificantBits));
}

Boolean ScanFilter::MatchesPartialData(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ ArrayOf<Byte>* dataMask,
    /* [in] */ ArrayOf<Byte>* parsedData)
{
    if (parsedData == NULL || parsedData->GetLength() < data->GetLength()) {
        return FALSE;
    }
    if (dataMask == NULL) {
        for (Int32 i = 0; i < data->GetLength(); ++i) {
            if ((*parsedData)[i] != (*data)[i]) {
                return FALSE;
            }
        }
        return TRUE;
    }
    for (Int32 i = 0; i < data->GetLength(); ++i) {
        if (((*dataMask)[i] & (*parsedData)[i]) != ((*dataMask)[i] & (*data)[i])) {
            return FALSE;
        }
    }
    return TRUE;
}

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
