#ifndef __ELASTOS_DROID_BLUETOOTH_LE_SCANFILTER_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_SCANFILTER_H__

#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IParcelUuid;
using Elastos::Utility::IList;
using Elastos::Utility::IUUID;

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

/**
  * Criteria for filtering result from Bluetooth LE scans. A {@link ScanFilter} allows clients to
  * restrict scan results to only those that are of interest to them.
  * <p>
  * Current filtering on the following fields are supported:
  * <li>Service UUIDs which identify the bluetooth gatt services running on the device.
  * <li>Name of remote Bluetooth LE device.
  * <li>Mac address of the remote device.
  * <li>Service data which is the data associated with a service.
  * <li>Manufacturer specific data which is the data associated with a particular manufacturer.
  *
  * @see ScanResult
  * @see BluetoothLeScanner
  */
class ScanFilter
    : public Object
    , public IScanFilter
    , public IParcelable
{
public:
    /**
      * Builder class for {@link ScanFilter}.
      */
    class Builder
        : public Object
        , public IScanFilterBuilder
    {
    public:
        CAR_INTERFACE_DECL()

        Builder();
        /**
          * Set filter on device name.
          */
        CARAPI SetDeviceName(
            /* [in] */ const String& deviceName);

        /**
          * Set filter on device address.
          *
          * @param deviceAddress The device Bluetooth address for the filter. It needs to be in the
          *            format of "01:02:03:AB:CD:EF". The device address can be validated using
          *            {@link BluetoothAdapter#checkBluetoothAddress}.
          * @throws IllegalArgumentException If the {@code deviceAddress} is invalid.
          */
        CARAPI SetDeviceAddress(
            /* [in] */ const String& deviceAddress);

        /**
          * Set filter on service uuid.
          */
        CARAPI SetServiceUuid(
            /* [in] */ IParcelUuid* serviceUuid);

        /**
          * Set filter on partial service uuid. The {@code uuidMask} is the bit mask for the
          * {@code serviceUuid}. Set any bit in the mask to 1 to indicate a match is needed for the
          * bit in {@code serviceUuid}, and 0 to ignore that bit.
          *
          * @throws IllegalArgumentException If {@code serviceUuid} is {@code null} but
          *             {@code uuidMask} is not {@code null}.
          */
        CARAPI SetServiceUuid(
            /* [in] */ IParcelUuid* serviceUuid,
            /* [in] */ IParcelUuid* uuidMask);

        /**
          * Set filtering on service data.
          *
          * @throws IllegalArgumentException If {@code serviceDataUuid} is null.
          */
        CARAPI SetServiceData(
            /* [in] */ IParcelUuid* serviceDataUuid,
            /* [in] */ ArrayOf<Byte>* serviceData);

        /**
          * Set partial filter on service data. For any bit in the mask, set it to 1 if it needs to
          * match the one in service data, otherwise set it to 0 to ignore that bit.
          * <p>
          * The {@code serviceDataMask} must have the same length of the {@code serviceData}.
          *
          * @throws IllegalArgumentException If {@code serviceDataUuid} is null or
          *             {@code serviceDataMask} is {@code null} while {@code serviceData} is not or
          *             {@code serviceDataMask} and {@code serviceData} has different length.
          */
        CARAPI SetServiceData(
            /* [in] */ IParcelUuid* serviceDataUuid,
            /* [in] */ ArrayOf<Byte>* serviceData,
            /* [in] */ ArrayOf<Byte>* serviceDataMask);

        /**
          * Set filter on on manufacturerData. A negative manufacturerId is considered as invalid id.
          * <p>
          * Note the first two bytes of the {@code manufacturerData} is the manufacturerId.
          *
          * @throws IllegalArgumentException If the {@code manufacturerId} is invalid.
          */
        CARAPI SetManufacturerData(
            /* [in] */ Int32 manufacturerId,
            /* [in] */ ArrayOf<Byte>* manufacturerData);

        /**
          * Set filter on partial manufacture data. For any bit in the mask, set it the 1 if it needs
          * to match the one in manufacturer data, otherwise set it to 0.
          * <p>
          * The {@code manufacturerDataMask} must have the same length of {@code manufacturerData}.
          *
          * @throws IllegalArgumentException If the {@code manufacturerId} is invalid, or
          *             {@code manufacturerData} is null while {@code manufacturerDataMask} is not,
          *             or {@code manufacturerData} and {@code manufacturerDataMask} have different
          *             length.
          */
        CARAPI SetManufacturerData(
            /* [in] */ Int32 manufacturerId,
            /* [in] */ ArrayOf<Byte>* manufacturerData,
            /* [in] */ ArrayOf<Byte>* manufacturerDataMask);

        /**
          * Build {@link ScanFilter}.
          *
          * @throws IllegalArgumentException If the filter cannot be built.
          */
        CARAPI Build(
            /* [out] */ IScanFilter** result);

    private:
        String mDeviceName;
        String mDeviceAddress;
        AutoPtr<IParcelUuid> mServiceUuid;
        AutoPtr<IParcelUuid> mUuidMask;
        AutoPtr<IParcelUuid> mServiceDataUuid;
        AutoPtr<ArrayOf<Byte> > mServiceData;
        AutoPtr<ArrayOf<Byte> > mServiceDataMask;
        Int32 mManufacturerId;
        AutoPtr<ArrayOf<Byte> > mManufacturerData;
        AutoPtr<ArrayOf<Byte> > mManufacturerDataMask;
    };

public:
    CAR_INTERFACE_DECL()

    ScanFilter();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IParcel* source);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);
    //    /* [in] */ Int32 flags);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
      * Returns the filter set the device name field of Bluetooth advertisement data.
      */
    // @Nullable
    CARAPI GetDeviceName(
        /* [out] */ String* str);

    /**
      * Returns the filter set on the service uuid.
      */
    // @Nullable
    CARAPI GetServiceUuid(
        /* [out] */ IParcelUuid** uuid);

    // @Nullable
    CARAPI GetServiceUuidMask(
        /* [out] */ IParcelUuid** uuid);

    // @Nullable
    CARAPI GetDeviceAddress(
        /* [out] */ String* str);

    // @Nullable
    CARAPI GetServiceData(
        /* [out, callee] */ ArrayOf<Byte>** result);

    // @Nullable
    CARAPI GetServiceDataMask(
        /* [out, callee] */ ArrayOf<Byte>** result);

    // @Nullable
    CARAPI GetServiceDataUuid(
        /* [out] */ IParcelUuid** uuid);

    /**
      * Returns the manufacturer id. -1 if the manufacturer filter is not set.
      */
    CARAPI GetManufacturerId(
        /* [out] */ Int32* result);

    // @Nullable
    CARAPI GetManufacturerData(
        /* [out, callee] */ ArrayOf<Byte>** result);

    // @Nullable
    CARAPI GetManufacturerDataMask(
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
      * Check if the scan filter matches a {@code scanResult}. A scan result is considered as a match
      * if it matches all the field filters.
      */
    CARAPI Matches(
        /* [in] */ IScanResult* scanResult,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* info);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

private:
    ScanFilter(
        /* [in] */ const String& name,
        /* [in] */ const String& deviceAddress,
        /* [in] */ IParcelUuid* uuid,
        /* [in] */ IParcelUuid* uuidMask,
        /* [in] */ IParcelUuid* serviceDataUuid,
        /* [in] */ ArrayOf<Byte>* serviceData,
        /* [in] */ ArrayOf<Byte>* serviceDataMask,
        /* [in] */ Int32 manufacturerId,
        /* [in] */ ArrayOf<Byte>* manufacturerData,
        /* [in] */ ArrayOf<Byte>* manufacturerDataMask);

    // Check if the uuid pattern is contained in a list of parcel uuids.
    CARAPI_(Boolean) MatchesServiceUuids(
        /* [in] */ IParcelUuid* uuid,
        /* [in] */ IParcelUuid* parcelUuidMask,
        /* [in] */ IList* uuids);// IParcelUuid

    // Check if the uuid pattern matches the particular service uuid.
    CARAPI_(Boolean) MatchesServiceUuid(
        /* [in] */ IUUID* uuid,
        /* [in] */ IUUID* mask,
        /* [in] */ IUUID* data);

    // Check whether the data pattern matches the parsed data.
    CARAPI_(Boolean) MatchesPartialData(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ ArrayOf<Byte>* dataMask,
        /* [in] */ ArrayOf<Byte>* parsedData);

private:
    // @Nullable
    String mDeviceName;
    // @Nullable
    String mDeviceAddress;
    // @Nullable
    AutoPtr<IParcelUuid> mServiceUuid;
    // @Nullable
    AutoPtr<IParcelUuid> mServiceUuidMask;
    // @Nullable
    AutoPtr<IParcelUuid> mServiceDataUuid;
    // @Nullable
    AutoPtr<ArrayOf<Byte> > mServiceData;
    // @Nullable
    AutoPtr<ArrayOf<Byte> > mServiceDataMask;
    Int32 mManufacturerId;
    // @Nullable
    AutoPtr<ArrayOf<Byte> > mManufacturerData;
    // @Nullable
    AutoPtr<ArrayOf<Byte> > mManufacturerDataMask;
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_SCANFILTER_H__

