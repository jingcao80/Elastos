#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTCHARACTERISTIC_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTCHARACTERISTIC_H__

#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Core::IFloat;
using Elastos::Core::IInteger32;
using Elastos::Utility::IList;
using Elastos::Utility::IUUID;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

/**
  * Represents a Bluetooth GATT Characteristic
  *
  * <p>A GATT characteristic is a basic data element used to construct a GATT service,
  * {@link BluetoothGattService}. The characteristic contains a value as well as
  * additional information and optional GATT descriptors, {@link BluetoothGattDescriptor}.
  */
class BluetoothGattCharacteristic
    : public Object
    , public IBluetoothGattCharacteristic
{
public:
    CAR_INTERFACE_DECL()

    BluetoothGattCharacteristic();

    /**
      * Create a new BluetoothGattCharacteristic.
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param uuid The UUID for this characteristic
      * @param properties Properties of this characteristic
      * @param permissions Permissions for this characteristic
      */
    BluetoothGattCharacteristic(
        /* [in] */ IUUID* uuid,
        /* [in] */ Int32 properties,
        /* [in] */ Int32 permissions);

    /**
      * Create a new BluetoothGattCharacteristic
      * @hide
      */
    /*package*/
    BluetoothGattCharacteristic(
        /* [in] */ IBluetoothGattService* service,
        /* [in] */ IUUID* uuid,
        /* [in] */ Int32 instanceId,
        /* [in] */ Int32 properties,
        /* [in] */ Int32 permissions);

    /**
      * Returns the deisred key size.
      * @hide
      */
    /*package*/
    CARAPI GetKeySize(
        /* [out] */ Int32* result);

    /**
      * Adds a descriptor to this characteristic.
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param descriptor Descriptor to be added to this characteristic.
      * @return true, if the descriptor was added to the characteristic
      */
    CARAPI AddDescriptor(
        /* [in] */ IBluetoothGattDescriptor* descriptor,
        /* [out] */ Boolean* result);

    /**
      * Get a descriptor by UUID and isntance id.
      * @hide
      */
    /*package*/
    CARAPI GetDescriptor(
        /* [in] */ IUUID* uuid,
        /* [in] */ Int32 instanceId,
        /* [out] */ IBluetoothGattDescriptor** result);

    /**
      * Returns the service this characteristic belongs to.
      * @return The asscociated service
      */
    CARAPI GetService(
        /* [out] */ IBluetoothGattService** result);

    /**
      * Sets the service associated with this device.
      * @hide
      */
    /*package*/
    CARAPI SetService(
        /* [in] */ IBluetoothGattService* service);

    /**
      * Returns the UUID of this characteristic
      *
      * @return UUID of this characteristic
      */
    CARAPI GetUuid(
        /* [out] */ IUUID** result);

    /**
      * Returns the instance ID for this characteristic.
      *
      * <p>If a remote device offers multiple characteristics with the same UUID,
      * the instance ID is used to distuinguish between characteristics.
      *
      * @return Instance ID of this characteristic
      */
    CARAPI GetInstanceId(
        /* [out] */ Int32* result);

    /**
      * Returns the properties of this characteristic.
      *
      * <p>The properties contain a bit mask of property flags indicating
      * the features of this characteristic.
      *
      * @return Properties of this characteristic
      */
    CARAPI GetProperties(
        /* [out] */ Int32* result);

    /**
      * Returns the permissions for this characteristic.
      *
      * @return Permissions of this characteristic
      */
    CARAPI GetPermissions(
        /* [out] */ Int32* result);

    /**
      * Gets the write type for this characteristic.
      *
      * @return Write type for this characteristic
      */
    CARAPI GetWriteType(
        /* [out] */ Int32* result);

    /**
      * Set the write type for this characteristic
      *
      * <p>Setting the write type of a characteristic determines how the
      * {@link BluetoothGatt#writeCharacteristic} function write this
      * characteristic.
      *
      * @param writeType The write type to for this characteristic. Can be one
      *                  of:
      *                  {@link #WRITE_TYPE_DEFAULT},
      *                  {@link #WRITE_TYPE_NO_RESPONSE} or
      *                  {@link #WRITE_TYPE_SIGNED}.
      */
    CARAPI SetWriteType(
        /* [in] */ Int32 writeType);

    /**
      * Set the desired key size.
      * @hide
      */
    CARAPI SetKeySize(
        /* [in] */ Int32 keySize);

    /**
      * Returns a list of descriptors for this characteristic.
      *
      * @return Descriptors for this characteristic
      */
    CARAPI GetDescriptors(
        /* [out] */ IList** result);// BluetoothGattDescriptor

    /**
      * Returns a descriptor with a given UUID out of the list of
      * descriptors for this characteristic.
      *
      * @return GATT descriptor object or null if no descriptor with the
      *         given UUID was found.
      */
    CARAPI GetDescriptor(
        /* [in] */ IUUID* uuid,
        /* [out] */ IBluetoothGattDescriptor** result);

    /**
      * Get the stored value for this characteristic.
      *
      * <p>This function returns the stored value for this characteristic as
      * retrieved by calling {@link BluetoothGatt#readCharacteristic}. The cached
      * value of the characteristic is updated as a result of a read characteristic
      * operation or if a characteristic update notification has been received.
      *
      * @return Cached value of the characteristic
      */
    CARAPI GetValue(
        /* [out] */ ArrayOf<Byte>** result);

    /**
      * Return the stored value of this characteristic.
      *
      * <p>The formatType parameter determines how the characteristic value
      * is to be interpreted. For example, settting formatType to
      * {@link #FORMAT_UINT16} specifies that the first two bytes of the
      * characteristic value at the given offset are interpreted to generate the
      * return value.
      *
      * @param formatType The format type used to interpret the characteristic
      *                   value.
      * @param offset Offset at which the integer value can be found.
      * @return Cached value of the characteristic or null of offset exceeds
      *         value size.
      */
    CARAPI GetIntValue(
        /* [in] */ Int32 formatType,
        /* [in] */ Int32 offset,
        /* [out] */ IInteger32** result);

    /**
      * Return the stored value of this characteristic.
      * <p>See {@link #getValue} for details.
      *
      * @param formatType The format type used to interpret the characteristic
      *                   value.
      * @param offset Offset at which the float value can be found.
      * @return Cached value of the characteristic at a given offset or null
      *         if the requested offset exceeds the value size.
      */
    CARAPI GetFloatValue(
        /* [in] */ Int32 formatType,
        /* [in] */ Int32 offset,
        /* [out] */ IFloat** result);

    /**
      * Return the stored value of this characteristic.
      * <p>See {@link #getValue} for details.
      *
      * @param offset Offset at which the string value can be found.
      * @return Cached value of the characteristic
      */
    CARAPI GetStringValue(
        /* [in] */ Int32 offset,
        /* [out] */ String* result);

    /**
      * Updates the locally stored value of this characteristic.
      *
      * <p>This function modifies the locally stored cached value of this
      * characteristic. To send the value to the remote device, call
      * {@link BluetoothGatt#writeCharacteristic} to send the value to the
      * remote device.
      *
      * @param value New value for this characteristic
      * @return true if the locally stored value has been set, false if the
      *              requested value could not be stored locally.
      */
    CARAPI SetValue(
        /* [in] */ ArrayOf<Byte>* value,
        /* [out] */ Boolean* result);

    /**
      * Set the locally stored value of this characteristic.
      * <p>See {@link #setValue(byte[])} for details.
      *
      * @param value New value for this characteristic
      * @param formatType Integer format type used to transform the value parameter
      * @param offset Offset at which the value should be placed
      * @return true if the locally stored value has been set
      */
    CARAPI SetValue(
        /* [in] */ Int32 value,
        /* [in] */ Int32 formatType,
        /* [in] */ Int32 offset,
        /* [out] */ Boolean* result);

    /**
      * Set the locally stored value of this characteristic.
      * <p>See {@link #setValue(byte[])} for details.
      *
      * @param mantissa Mantissa for this characteristic
      * @param exponent  exponent value for this characteristic
      * @param formatType Float format type used to transform the value parameter
      * @param offset Offset at which the value should be placed
      * @return true if the locally stored value has been set
      */
    CARAPI SetValue(
        /* [in] */ Int32 mantissa,
        /* [in] */ Int32 exponent,
        /* [in] */ Int32 formatType,
        /* [in] */ Int32 offset,
        /* [out] */ Boolean* result);

    /**
      * Set the locally stored value of this characteristic.
      * <p>See {@link #setValue(byte[])} for details.
      *
      * @param value New value for this characteristic
      * @return true if the locally stored value has been set
      */
    CARAPI SetValue(
        /* [in] */ const String& value,
        /* [out] */ Boolean* result);

private:
    CARAPI_(void) InitCharacteristic(
        /* [in] */ IBluetoothGattService* service,
        /* [in] */ IUUID* uuid,
        /* [in] */ Int32 instanceId,
        /* [in] */ Int32 properties,
        /* [in] */ Int32 permissions);

    /**
      * Returns the size of a give value type.
      */
    CARAPI_(Int32) GetTypeLen(
        /* [in] */ Int32 formatType);

    /**
      * Convert a signed byte to an unsigned int.
      */
    CARAPI_(Int32) UnsignedByteToInt(
        /* [in] */ Byte b);

    /**
      * Convert signed bytes to a 16-bit unsigned int.
      */
    CARAPI_(Int32) UnsignedBytesToInt(
        /* [in] */ Byte b0,
        /* [in] */ Byte b1);

    /**
      * Convert signed bytes to a 32-bit unsigned int.
      */
    CARAPI_(Int32) UnsignedBytesToInt(
        /* [in] */ Byte b0,
        /* [in] */ Byte b1,
        /* [in] */ Byte b2,
        /* [in] */ Byte b3);

    /**
      * Convert signed bytes to a 16-bit short float value.
      */
    CARAPI_(Float) BytesToFloat(
        /* [in] */ Byte b0,
        /* [in] */ Byte b1);

    /**
      * Convert signed bytes to a 32-bit short float value.
      */
    CARAPI_(Float) BytesToFloat(
        /* [in] */ Byte b0,
        /* [in] */ Byte b1,
        /* [in] */ Byte b2,
        /* [in] */ Byte b3);

    /**
      * Convert an unsigned integer value to a two's-complement encoded
      * signed value.
      */
    CARAPI_(Int32) UnsignedToSigned(
        /* [in] */ Int32 unsignedInt,
        /* [in] */ Int32 size);

    /**
      * Convert an integer into the signed bits of a given length.
      */
    CARAPI_(Int32) IntToSignedBits(
        /* [in] */ Int32 i,
        /* [in] */ Int32 size);

protected:
    /**
      * The UUID of this characteristic.
      * @hide
      */
    AutoPtr<IUUID> mUuid;
    /**
      * Instance ID for this characteristic.
      * @hide
      */
    Int32 mInstance;
    /**
      * Characteristic properties.
      * @hide
      */
    Int32 mProperties;
    /**
      * Characteristic permissions.
      * @hide
      */
    Int32 mPermissions;
    /**
      * Key size (default = 16).
      * @hide
      */
    Int32 mKeySize;
    /**
      * Write type for this characteristic.
      * See WRITE_TYPE_* constants.
      * @hide
      */
    Int32 mWriteType;
    /**
      * Back-reference to the service this characteristic belongs to.
      * @hide
      */
    AutoPtr<IBluetoothGattService> mService;
    /**
      * The cached value of this characteristic.
      * @hide
      */
    AutoPtr<ArrayOf<Byte> > mValue;
    /**
      * List of descriptors included in this characteristic.
      */
    AutoPtr<IList> mDescriptors;// BluetoothGattDescriptor
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTCHARACTERISTIC_H__
