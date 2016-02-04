#ifndef __ELASTOS_DROID_BLUETOOTH_LE_ADVERTISEDATA_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_ADVERTISEDATA_H__

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
  * Advertise data packet container for Bluetooth LE advertising. This represents the data to be
  * advertised as well as the scan response data for active scans.
  * <p>
  * Use {@link AdvertiseData.Builder} to create an instance of {@link AdvertiseData} to be
  * advertised.
  *
  * @see BluetoothLeAdvertiser
  * @see ScanRecord
  */
class AdvertiseData
    : public Object
    , public IAdvertiseData
    , public IParcelable
{
public:
    /**
      * Builder for {@link AdvertiseData}.
      */
    class Builder
        : public Object
        , public IAdvertiseDataBuilder
    {
    public:

        Builder();

        CAR_INTERFACE_DECL();

        /**
          * Add a service UUID to advertise data.
          *
          * @param serviceUuid A service UUID to be advertised.
          * @throws IllegalArgumentException If the {@code serviceUuids} are null.
          */
        CARAPI AddServiceUuid(
            /* [in] */ IParcelUuid* serviceUuid);

        /**
          * Add service data to advertise data.
          *
          * @param serviceDataUuid 16-bit UUID of the service the data is associated with
          * @param serviceData Service data
          * @throws IllegalArgumentException If the {@code serviceDataUuid} or {@code serviceData} is
          *             empty.
          */
        CARAPI AddServiceData(
            /* [in] */ IParcelUuid* serviceDataUuid,
            /* [in] */ ArrayOf<Byte>* serviceData);

        /**
          * Add manufacturer specific data.
          * <p>
          * Please refer to the Bluetooth Assigned Numbers document provided by the <a
          * href="https://www.bluetooth.org">Bluetooth SIG</a> for a list of existing company
          * identifiers.
          *
          * @param manufacturerId Manufacturer ID assigned by Bluetooth SIG.
          * @param manufacturerSpecificData Manufacturer specific data
          * @throws IllegalArgumentException If the {@code manufacturerId} is negative or
          *             {@code manufacturerSpecificData} is null.
          */
        CARAPI AddManufacturerData(
            /* [in] */ Int32 manufacturerId,
            /* [in] */ ArrayOf<Byte>* manufacturerSpecificData);

        /**
          * Whether the transmission power level should be included in the advertise packet. Tx power
          * level field takes 3 bytes in advertise packet.
          */
        CARAPI SetIncludeTxPowerLevel(
            /* [in] */ Boolean includeTxPowerLevel);

        /**
          * Set whether the device name should be included in advertise packet.
          */
        CARAPI SetIncludeDeviceName(
            /* [in] */ Boolean includeDeviceName);

        /**
          * Build the {@link AdvertiseData}.
          */
        CARAPI Build(
            /* [out] */ IAdvertiseData** result);

    //private:
    //    CARAPI_(AutoPtr<ISparseArray>) MiddleInitMmanufacturerspecificdata();

    private:
        // @Nullable
        AutoPtr<IList> mServiceUuids;// IParcelUuid
        AutoPtr<ISparseArray> mManufacturerSpecificData;// AutoPtr< ArrayOf<Byte> >
        AutoPtr<IMap> mServiceData;// IParcelUuid, AutoPtr< ArrayOf<Byte> >
        Boolean mIncludeTxPowerLevel;
        Boolean mIncludeDeviceName;

        friend AdvertiseData;
    };

public:
    AdvertiseData();

    CAR_INTERFACE_DECL();

    /**
      * Returns a list of service UUIDs within the advertisement that are used to identify the
      * Bluetooth GATT services.
      */
    CARAPI GetServiceUuids(
        /* [out] */ IList** result);//IParcelUuid

    /**
      * Returns an array of manufacturer Id and the corresponding manufacturer specific data. The
      * manufacturer id is a non-negative number assigned by Bluetooth SIG.
      */
    CARAPI GetManufacturerSpecificData(
        /* [out] */ ISparseArray** result);//byte[]

    /**
      * Returns a map of 16-bit UUID and its corresponding service data.
      */
    CARAPI GetServiceData(
        /* [out] */ IMap** result);//IParcelUuid, Byte[]

    /**
      * Whether the transmission power level will be included in the advertisement packet.
      */
    CARAPI GetIncludeTxPowerLevel(
        /* [out] */ Boolean* result);

    /**
      * Whether the device name will be included in the advertisement packet.
      */
    CARAPI GetIncludeDeviceName(
        /* [out] */ Boolean* result);

    /**
      * @hide
      */
    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    /**
      * @hide
      */
    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* info);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    // @Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI constructor(
        /* [in] */ IList* serviceUuids,// IParcelUuid
        /* [in] */ ISparseArray* manufacturerData,// ArrayOf<Byte>
        /* [in] */ IMap* serviceData,// IParcelUuid, ArrayOf<Byte>
        /* [in] */ Boolean includeTxPowerLevel,
        /* [in] */ Boolean includeDeviceName);

private:
    // @Nullable
    AutoPtr<IList> mServiceUuids;// IParcelUuid
    AutoPtr<ISparseArray> mManufacturerSpecificData;// ArrayOf<Byte>
    AutoPtr<IMap> mServiceData;
    Boolean mIncludeTxPowerLevel;
    Boolean mIncludeDeviceName;
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_ADVERTISEDATA_H__

