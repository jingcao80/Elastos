
#ifndef __ELASTOS_DROID_HARDWARE_USB_USBACCESSORY_H__
#define __ELASTOS_DROID_HARDWARE_USB_USBACCESSORY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

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
class UsbAccessory
    : public Object
    , public IUsbAccessory
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * UsbAccessory should only be instantiated by UsbService implementation
     * @hide
     */
    CARAPI constructor(
        /* [in] */ const String& manufacturer,
        /* [in] */ const String& model,
        /* [in] */ const String& description,
        /* [in] */ const String& ver,
        /* [in] */ const String& uri,
        /* [in] */ const String& serial);

    /**
     * UsbAccessory should only be instantiated by UsbService implementation
     * @hide
     */
    CARAPI constructor(
        /* [in] */ const ArrayOf<String>& strings);

    /**
     * Returns the manufacturer of the accessory.
     *
     * @return the accessory manufacturer
     */
    CARAPI GetManufacturer(
        /* [out] */ String* manufacturer);

    /**
     * Returns the model name of the accessory.
     *
     * @return the accessory model
     */
    CARAPI GetModel(
        /* [out] */ String* model);

    /**
     * Returns a user visible description of the accessory.
     *
     * @return the accessory description
     */
    CARAPI GetDescription(
        /* [out] */ String* description);

    /**
     * Returns the version of the accessory.
     *
     * @return the accessory version
     */
    CARAPI GetVersion(
        /* [out] */ String* ver);

    /**
     * Returns the URI for the accessory.
     * This is an optional URI that might show information about the accessory
     * or provide the option to download an application for the accessory
     *
     * @return the accessory URI
     */
    CARAPI GetUri(
        /* [out] */ String* uri);

    /**
     * Returns the unique serial number for the accessory.
     * This is an optional serial number that can be used to differentiate
     * between individual accessories of the same model and manufacturer
     *
     * @return the unique serial number
     */
    CARAPI GetSerial(
        /* [out] */ String* serial);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* value);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI CompareTo(
        /* [in] */ IUsbAccessory* accessory,
        /* [out] */ Boolean* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [out] */ IParcel* dest);

private:
    static CARAPI_(Boolean) Compare(
        /* [in] */ const String& s1,
        /* [in] */ const String& s2);

private:
    static const String TAG;

    String mManufacturer;
    String mModel;
    String mDescription;
    String mVersion;
    String mUri;
    String mSerial;
};

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_USB_USBACCESSORY_H__
