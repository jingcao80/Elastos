#ifndef __ELASTOS_DROID_BLUETOOTH_LE_SCANRECORD_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_SCANRECORD_H__

#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IParcelUuid;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;


namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

/**
  * Represents a scan record from Bluetooth LE scan.
  */
class ScanRecord
    : public Object
    , public IScanRecord
{
public:
    CAR_INTERFACE_DECL();

    ScanRecord();

    /**
      * Returns the advertising flags indicating the discoverable mode and capability of the device.
      * Returns -1 if the flag field is not set.
      */
    CARAPI GetAdvertiseFlags(
        /* [out] */ Int32* result);

    /**
      * Returns a list of service UUIDs within the advertisement that are used to identify the
      * bluetooth GATT services.
      */
    CARAPI GetServiceUuids(
        /* [out] */ IList** result);

    /**
      * Returns a sparse array of manufacturer identifier and its corresponding manufacturer specific
      * data.
      */
    CARAPI GetManufacturerSpecificData(
        /* [out] */ ISparseArray** result);

    /**
      * Returns the manufacturer specific data associated with the manufacturer id. Returns
      * {@code null} if the {@code manufacturerId} is not found.
      */
    // @Nullable
    CARAPI GetManufacturerSpecificData(
        /* [in] */ Int32 manufacturerId,
        /* [out, callee] */ ArrayOf<Byte>** data);

    /**
      * Returns a map of service UUID and its corresponding service data.
      */
    CARAPI GetServiceData(
        /* [out] */ IMap** result);// ParcelUuid, byte[]

    /**
      * Returns the service data byte array associated with the {@code serviceUuid}. Returns
      * {@code null} if the {@code serviceDataUuid} is not found.
      */
    // @Nullable
    CARAPI GetServiceData(
        /* [in] */ IParcelUuid* serviceDataUuid,
        /* [out, callee] */ ArrayOf<Byte>** data);

    /**
      * Returns the transmission power level of the packet in dBm. Returns {@link Integer#MIN_VALUE}
      * if the field is not set. This value can be used to calculate the path loss of a received
      * packet using the following equation:
      * <p>
      * <code>pathloss = txPowerLevel - rssi</code>
      */
    CARAPI GetTxPowerLevel(
        /* [out] */ Int32* result);

    /**
      * Returns the local name of the BLE device. The is a UTF-8 encoded string.
      */
    // @Nullable
    CARAPI GetDeviceName(
        /* [out] */ String* result);

    /**
      * Returns raw bytes of scan record.
      */
    CARAPI GetBytes(
        /* [out] */ ArrayOf<Byte>** result);

    /**
      * Parse scan record bytes to {@link ScanRecord}.
      * <p>
      * The format is defined in Bluetooth 4.1 specification, Volume 3, Part C, Section 11 and 18.
      * <p>
      * All numerical multi-byte entities and values shall use little-endian <strong>byte</strong>
      * order.
      *
      * @param scanRecord The scan record of Bluetooth LE advertisement and/or scan response.
      * @hide
      */
    static CARAPI_(AutoPtr<IScanRecord>) ParseFromBytes(
        /* [in] */ ArrayOf<Byte>* scanRecord);

    // @Override
    CARAPI ToString(
        /* [out] */ String* info);

private:
    ScanRecord(
        /* [in] */ IList* serviceUuids,// IParcelUuid
        /* [in] */ ISparseArray* manufacturerData,// ArrayOf<Byte>
        /* [in] */ IMap* serviceData,// IParcelUuid, ArrayOf<Byte>
        /* [in] */ Int32 advertiseFlags,
        /* [in] */ Int32 txPowerLevel,
        /* [in] */ const String& localName,
        /* [in] */ ArrayOf<Byte>* bytes);

    // Parse service UUIDs.
    static CARAPI_(Int32) ParseServiceUuid(
        /* [in] */ ArrayOf<Byte>* scanRecord,
        /* [in] */ Int32 currentPos,
        /* [in] */ Int32 dataLength,
        /* [in] */ Int32 uuidLength,
        /* [in] */ IList* serviceUuids);// IParcelUuid

    // Helper method to extract bytes from byte array.
    static CARAPI_(AutoPtr<ArrayOf<Byte> >) ExtractBytes(
        /* [in] */ ArrayOf<Byte>* scanRecord,
        /* [in] */ Int32 start,
        /* [in] */ Int32 length);

private:
    static const String TAG;
    // The following data type values are assigned by Bluetooth SIG.
    // For more details refer to Bluetooth 4.1 specification, Volume 3, Part C, Section 18.
    static const Int32 DATA_TYPE_FLAGS = 0x01;
    static const Int32 DATA_TYPE_SERVICE_UUIDS_16_BIT_PARTIAL = 0x02;
    static const Int32 DATA_TYPE_SERVICE_UUIDS_16_BIT_COMPLETE = 0x03;
    static const Int32 DATA_TYPE_SERVICE_UUIDS_32_BIT_PARTIAL = 0x04;
    static const Int32 DATA_TYPE_SERVICE_UUIDS_32_BIT_COMPLETE = 0x05;
    static const Int32 DATA_TYPE_SERVICE_UUIDS_128_BIT_PARTIAL = 0x06;
    static const Int32 DATA_TYPE_SERVICE_UUIDS_128_BIT_COMPLETE = 0x07;
    static const Int32 DATA_TYPE_LOCAL_NAME_SHORT = 0x08;
    static const Int32 DATA_TYPE_LOCAL_NAME_COMPLETE = 0x09;
    static const Int32 DATA_TYPE_TX_POWER_LEVEL = 0x0A;
    static const Int32 DATA_TYPE_SERVICE_DATA = 0x16;
    static const Int32 DATA_TYPE_MANUFACTURER_SPECIFIC_DATA = 0xFF;
    // Flags of the advertising data.
    Int32 mAdvertiseFlags;
    // @Nullable
    AutoPtr<IList> mServiceUuids;// IParcelUuid
    AutoPtr<ISparseArray> mManufacturerSpecificData;// ArrayOf<Byte>
    AutoPtr<IMap> mServiceData; // ParcelUuid, byte[]
    // Transmission power level(in dB).
    Int32 mTxPowerLevel;
    // Local name of the Bluetooth LE device.
    String mDeviceName;
    // Raw bytes of scan record.
    AutoPtr<ArrayOf<Byte> > mBytes;
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_SCANRECORD_H__

