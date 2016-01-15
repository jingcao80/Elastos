
#ifndef __ELASTOS_DROID_HARDWARE_USB_USBCONFIGURATION_H__
#define __ELASTOS_DROID_HARDWARE_USB_USBCONFIGURATION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

/**
 * A class representing a USB accessory, which is an external hardware component
 * that communicates with an android application over USB.
 * The accessory is the USB host and android the device side of the USB connection.
 *
 * <p>When the accessory connects, it reports its manufacturer and model names,
 * the version of the accessory, and a user visible description of the accessory to the device.
 * The manufacturer, model and version strings are used by the USB Manager to choose
 * an appropriate application for the accessory.
 * The accessory may optionally provide a unique serial number
 * and a URL to for the accessory's website to the device as well.
 *
 * <p>An instance of this class is sent to the application via the
 * {@link UsbManager#ACTION_USB_ACCESSORY_ATTACHED} Intent.
 * The application can then call {@link UsbManager#openAccessory} to open a file descriptor
 * for reading and writing data to and from the accessory.
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>For more information about communicating with USB hardware, read the
 * <a href="{@docRoot}guide/topics/usb/index.html">USB</a> developer guide.</p>
 * </div>
 */
class UsbConfiguration
    : public Object
    , public IUsbConfiguration
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * UsbConfiguration should only be instantiated by UsbService implementation
     * @hide
     */
    CARAPI constructor(
        /* [in] */ Int32 id,
        /* [in] */ const String& name,
        /* [in] */ Int32 attributes,
        /* [in] */ Int32 maxPower);

    /**
     * Returns the configuration's ID field.
     * This is an integer that uniquely identifies the configuration on the device.
     *
     * @return the configuration's ID
     */
    CARAPI GetId(
        /* [out] */ Int32* value);

    /**
     * Returns the configuration's name.
     *
     * @return the configuration's name
     */
    CARAPI GetName(
        /* [out] */ String* str);

    /**
     * Returns the self-powered attribute value configuration's attributes field.
     * This attribute indicates that the device has a power source other than the USB connection.
     *
     * @return the configuration's self-powered attribute
     */
    CARAPI IsSelfPowered(
        /* [out] */ Boolean* value);

    /**
     * Returns the remote-wakeup attribute value configuration's attributes field.
     * This attributes that the device may signal the host to wake from suspend.
     *
     * @return the configuration's remote-wakeup attribute
     */
    CARAPI IsRemoteWakeup(
        /* [out] */ Boolean* value);

    /**
     * Returns the configuration's max power consumption, in milliamps.
     *
     * @return the configuration's max power
     */
    CARAPI GetMaxPower(
        /* [out] */ Int32* value);

    /**
     * Returns the number of {@link UsbInterface}s this configuration contains.
     *
     * @return the number of endpoints
     */
    CARAPI GetInterfaceCount(
        /* [out] */ Int32* value);

    /**
     * Returns the {@link UsbInterface} at the given index.
     *
     * @return the interface
     */
    CARAPI GetInterface(
        /* [in] */ Int32 index,
        /* [out] */ IUsbInterface** outui);

    /**
     * Only used by UsbService implementation
     * @hide
     */
    CARAPI SetInterfaces(
        /* [in] */ ArrayOf<IParcelable*>* interfaces);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [out] */ IParcel* dest);

    // public static final Parcelable.Creator<UsbConfiguration> CREATOR =
    //     new Parcelable.Creator<UsbConfiguration>() {
    //     public UsbConfiguration createFromParcel(Parcel in) {
    //         int id = in.readInt();
    //         String name = in.readString();
    //         int attributes = in.readInt();
    //         int maxPower = in.readInt();
    //         Parcelable[] interfaces = in.readParcelableArray(UsbInterface.class.getClassLoader());
    //         UsbConfiguration configuration = new UsbConfiguration(id, name, attributes, maxPower);
    //         configuration.setInterfaces(interfaces);
    //         return configuration;
    //     }

    //     public UsbConfiguration[] newArray(int size) {
    //         return new UsbConfiguration[size];
    //     }
    // };

private:
    Int32 mId;
    String mName;
    Int32 mAttributes;
    Int32 mMaxPower;
    AutoPtr<ArrayOf<IParcelable*> > mInterfaces;

    /**
     * Mask for "self-powered" bit in the configuration's attributes.
     * @see #getAttributes
     */
    static const Int32 ATTR_SELF_POWERED;

    /**
     * Mask for "remote wakeup" bit in the configuration's attributes.
     * @see #getAttributes
     */
    static const Int32 ATTR_REMOTE_WAKEUP;
};

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_USB_USBCONFIGURATION_H__
