#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTDESCRIPTOR_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTDESCRIPTOR_H__

#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Utility::IUUID;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

/**
  * Represents a Bluetooth GATT Descriptor
  *
  * <p> GATT Descriptors contain additional information and attributes of a GATT
  * characteristic, {@link BluetoothGattCharacteristic}. They can be used to describe
  * the characteristic's features or to control certain behaviours of the characteristic.
  */
class BluetoothGattDescriptor
    : public Object
    , public IBluetoothGattDescriptor
{
public:
    CAR_INTERFACE_DECL()

    BluetoothGattDescriptor();

    /**
      * Create a new BluetoothGattDescriptor.
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param uuid The UUID for this descriptor
      * @param permissions Permissions for this descriptor
      */
    CARAPI constructor(
        /* [in] */ IUUID* uuid,
        /* [in] */ Int32 permissions);

    /**
      * Create a new BluetoothGattDescriptor.
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param characteristic The characteristic this descriptor belongs to
      * @param uuid The UUID for this descriptor
      * @param permissions Permissions for this descriptor
      */
    /*package*/
    BluetoothGattDescriptor(
        /* [in] */ IBluetoothGattCharacteristic* characteristic,
        /* [in] */ IUUID* uuid,
        /* [in] */ Int32 instance,
        /* [in] */ Int32 permissions);

    /**
      * Returns the characteristic this descriptor belongs to.
      * @return The characteristic.
      */
    CARAPI GetCharacteristic(
        /* [out] */ IBluetoothGattCharacteristic** result);

    /**
      * Set the back-reference to the associated characteristic
      * @hide
      */
    /*package*/
    CARAPI SetCharacteristic(
        /* [in] */ IBluetoothGattCharacteristic* characteristic);

    /**
      * Returns the UUID of this descriptor.
      *
      * @return UUID of this descriptor
      */
    CARAPI GetUuid(
        /* [out] */ IUUID** result);

    /**
      * Returns the instance ID for this descriptor.
      *
      * <p>If a remote device offers multiple descriptors with the same UUID,
      * the instance ID is used to distuinguish between descriptors.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @return Instance ID of this descriptor
      * @hide
      */
    CARAPI GetInstanceId(
        /* [out] */ Int32* result);

    /**
      * Returns the permissions for this descriptor.
      *
      * @return Permissions of this descriptor
      */
    CARAPI GetPermissions(
        /* [out] */ Int32* result);

    /**
      * Returns the stored value for this descriptor
      *
      * <p>This function returns the stored value for this descriptor as
      * retrieved by calling {@link BluetoothGatt#readDescriptor}. The cached
      * value of the descriptor is updated as a result of a descriptor read
      * operation.
      *
      * @return Cached value of the descriptor
      */
    CARAPI GetValue(
        /* [out] */ ArrayOf<Byte>** result);

    /**
      * Updates the locally stored value of this descriptor.
      *
      * <p>This function modifies the locally stored cached value of this
      * descriptor. To send the value to the remote device, call
      * {@link BluetoothGatt#writeDescriptor} to send the value to the
      * remote device.
      *
      * @param value New value for this descriptor
      * @return true if the locally stored value has been set, false if the
      *              requested value could not be stored locally.
      */
    CARAPI SetValue(
        /* [in] */ ArrayOf<Byte>* value,
        /* [out] */ Boolean* result);

private:
    static CARAPI_(AutoPtr< ArrayOf<Byte> >) MiddleInitEnableNotificationValue();

    static CARAPI_(AutoPtr< ArrayOf<Byte> >) MiddleInitEnableIndicationValue();

    static CARAPI_(AutoPtr< ArrayOf<Byte> >) MiddleInitDisableNotificationValue();

    CARAPI_(void) InitDescriptor(
        /* [in] */ IBluetoothGattCharacteristic* characteristic,
        /* [in] */ IUUID* uuid,
        /* [in] */ Int32 instance,
        /* [in] */ Int32 permissions);

//public:
//    /**
//      * Value used to enable notification for a client configuration descriptor
//      */
//    static AutoPtr< ArrayOf<Byte> > ENABLE_NOTIFICATION_VALUE;
//    /**
//      * Value used to enable indication for a client configuration descriptor
//      */
//    static AutoPtr< ArrayOf<Byte> > ENABLE_INDICATION_VALUE;
//    /**
//      * Value used to disable notifications or indicatinos
//      */
//    static AutoPtr< ArrayOf<Byte> > DISABLE_NOTIFICATION_VALUE;
protected:
    /**
      * The UUID of this descriptor.
      * @hide
      */
    AutoPtr<IUUID> mUuid;
    /**
      * Instance ID for this descriptor.
      * @hide
      */
    Int32 mInstance;
    /**
      * Permissions for this descriptor
      * @hide
      */
    Int32 mPermissions;
    /**
      * Back-reference to the characteristic this descriptor belongs to.
      * @hide
      */
    AutoPtr<IBluetoothGattCharacteristic> mCharacteristic;
    /**
      * The value for this descriptor.
      * @hide
      */
    AutoPtr<ArrayOf<Byte> > mValue;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTDESCRIPTOR_H__
