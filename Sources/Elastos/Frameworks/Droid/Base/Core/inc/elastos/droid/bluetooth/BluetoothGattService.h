#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTSERVICE_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTSERVICE_H__

#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Utility::IList;
using Elastos::Utility::IUUID;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

/**
  * Represents a Bluetooth GATT Service
  *
  * <p> Gatt Service contains a collection of {@link BluetoothGattCharacteristic},
  * as well as referenced services.
  */
class BluetoothGattService
    : public Object
    , public IBluetoothGattService
{
public:
    CAR_INTERFACE_DECL();

    BluetoothGattService();

    /**
      * Create a new BluetoothGattService.
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param uuid The UUID for this service
      * @param serviceType The type of this service,
      *        {@link BluetoothGattService#SERVICE_TYPE_PRIMARY} or
      *        {@link BluetoothGattService#SERVICE_TYPE_SECONDARY}
      */
    CARAPI constructor(
        /* [in] */ IUUID* uuid,
        /* [in] */ Int32 serviceType);

    /**
      * Create a new BluetoothGattService
      * @hide
      */
    /*package*/
    BluetoothGattService(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ IUUID* uuid,
        /* [in] */ Int32 instanceId,
        /* [in] */ Int32 serviceType);

    /**
      * Returns the device associated with this service.
      * @hide
      */
    /*package*/
    CARAPI GetDevice(
        /* [out] */ IBluetoothDevice** result);

    /**
      * Add an included service to this service.
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param service The service to be added
      * @return true, if the included service was added to the service
      */
    CARAPI AddService(
        /* [in] */ IBluetoothGattService* service,
        /* [out] */ Boolean* result);

    /**
      * Add a characteristic to this service.
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param characteristic The characteristics to be added
      * @return true, if the characteristic was added to the service
      */
    CARAPI AddCharacteristic(
        /* [in] */ IBluetoothGattCharacteristic* characteristic,
        /* [out] */ Boolean* result);

    /**
      * Get characteristic by UUID and instanceId.
      * @hide
      */
    /*package*/
    CARAPI GetCharacteristic(
        /* [in] */ IUUID* uuid,
        /* [in] */ Int32 instanceId,
        /* [out] */ IBluetoothGattCharacteristic** result);

    /**
      * Force the instance ID.
      * This is needed for conformance testing only.
      * @hide
      */
    CARAPI SetInstanceId(
        /* [in] */ Int32 instanceId);

    /**
      * Get the handle count override (conformance testing.
      * @hide
      */
    /*package*/
    CARAPI GetHandles(
        /* [out] */ Int32* result);

    /**
      * Force the number of handles to reserve for this service.
      * This is needed for conformance testing only.
      * @hide
      */
    CARAPI SetHandles(
        /* [in] */ Int32 handles);

    /**
      * Add an included service to the internal map.
      * @hide
      */
    /*package*/
    CARAPI AddIncludedService(
        /* [in] */ IBluetoothGattService* includedService);

    /**
      * Returns the UUID of this service
      *
      * @return UUID of this service
      */
    CARAPI GetUuid(
        /* [out] */ IUUID** result);

    /**
      * Returns the instance ID for this service
      *
      * <p>If a remote device offers multiple services with the same UUID
      * (ex. multiple battery services for different batteries), the instance
      * ID is used to distuinguish services.
      *
      * @return Instance ID of this service
      */
    CARAPI GetInstanceId(
        /* [out] */ Int32* result);

    /**
      * Get the type of this service (primary/secondary)
      */
    CARAPI GetType(
        /* [out] */ Int32* result);

    /**
      * Get the list of included GATT services for this service.
      *
      * @return List of included services or empty list if no included services
      *         were discovered.
      */
    CARAPI GetIncludedServices(
        /* [out] */ IList** result);// BluetoothGattService

    /**
      * Returns a list of characteristics included in this service.
      *
      * @return Characteristics included in this service
      */
    CARAPI GetCharacteristics(
        /* [out] */ IList** result);

    /**
      * Returns a characteristic with a given UUID out of the list of
      * characteristics offered by this service.
      *
      * <p>This is a convenience function to allow access to a given characteristic
      * without enumerating over the list returned by {@link #getCharacteristics}
      * manually.
      *
      * <p>If a remote service offers multiple characteristics with the same
      * UUID, the first instance of a characteristic with the given UUID
      * is returned.
      *
      * @return GATT characteristic object or null if no characteristic with the
      *         given UUID was found.
      */
    CARAPI GetCharacteristic(
        /* [in] */ IUUID* uuid,
        /* [out] */ IBluetoothGattCharacteristic** result);

    /**
      * Returns whether the uuid of the service should be advertised.
      * @hide
      */
    CARAPI IsAdvertisePreferred(
        /* [out] */ Boolean* result);

    /**
      * Set whether the service uuid should be advertised.
      * @hide
      */
    CARAPI SetAdvertisePreferred(
        /* [in] */ Boolean advertisePreferred);

protected:
    /**
      * The remote device his service is associated with.
      * This applies to client applications only.
      * @hide
      */
    AutoPtr<IBluetoothDevice> mDevice;
    /**
      * The UUID of this service.
      * @hide
      */
    AutoPtr<IUUID> mUuid;
    /**
      * Instance ID for this service.
      * @hide
      */
    Int32 mInstanceId;
    /**
      * Handle counter override (for conformance testing).
      * @hide
      */
    Int32 mHandles;
    /**
      * Service type (Primary/Secondary).
      * @hide
      */
    Int32 mServiceType;
    /**
      * List of characteristics included in this service.
      */
    AutoPtr<IList> mCharacteristics;// BluetoothGattCharacteristic
    /**
      * List of included services for this service.
      */
    AutoPtr<IList> mIncludedServices;// BluetoothGattService

private:
    /**
      * Whether the service uuid should be advertised.
      */
    Boolean mAdvertisePreferred;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTSERVICE_H__

