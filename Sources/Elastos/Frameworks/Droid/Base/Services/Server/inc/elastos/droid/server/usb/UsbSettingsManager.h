
#ifndef __ELASTOS_DROID_SERVER_USB_USBSETTINGSMANAGER_H__
#define __ELASTOS_DROID_SERVER_USB_USBSETTINGSMANAGER_H__

#include "elastos/droid/internal/content/PackageMonitor.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/utility/Xml.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Hardware::Usb::IUsbAccessory;
using Elastos::Droid::Hardware::Usb::IUsbDevice;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Utility::Xml;
using Elastos::IO::IFile;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Usb {

/*
 * This class is used to describe a USB device.
 * When used in HashMaps all values must be specified,
 * but wildcards can be used for any of the fields in
 * the package meta-data.
 */
class DeviceFilter : public Object
{
public:
    DeviceFilter(
        /* [in] */ Int32 vid,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 clasz,
        /* [in] */ Int32 subclass,
        /* [in] */ Int32 protocol,
        /* [in] */ const String& manufacturer,
        /* [in] */ const String& product,
        /* [in] */ const String& serialnum);

    DeviceFilter(
        /* [in] */ IUsbDevice* device);

    CAR_INTERFACE_DECL()

    /*
     * public static DeviceFilter read(XmlPullParser parser) throws XmlPullParserException, IOException {...}
     */
    static CARAPI Read(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ DeviceFilter** filter);

    /*
     * public void write(XmlSerializer serializer) throws IOException {...}
     */
    CARAPI Write(
        /* [in] */ IXmlSerializer* serializer);

    CARAPI_(Boolean) Matches(
        /* [in] */ IUsbDevice* device);

    CARAPI_(Boolean) Matches(
        /* [in] */ DeviceFilter* filter);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* res);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(Boolean) Matches(
        /* [in] */ Int32 clasz,
        /* [in] */ Int32 subclass,
        /* [in] */ Int32 protocol);

private:
    // USB Vendor ID (or -1 for unspecified)
    Int32 mVendorId;
    // USB Product ID (or -1 for unspecified)
    Int32 mProductId;
    // USB device or interface class (or -1 for unspecified)
    Int32 mClass;
    // USB device subclass (or -1 for unspecified)
    Int32 mSubclass;
    // USB device protocol (or -1 for unspecified)
    Int32 mProtocol;
    // USB device manufacturer name string (or null for unspecified)
    String mManufacturerName;
    // USB device product name string (or null for unspecified)
    String mProductName;
    // USB device serial number string (or null for unspecified)
    String mSerialNumber;
};

/*
 * This class is used to describe a USB accessory.
 * When used in HashMaps all values must be specified,
 * but wildcards can be used for any of the fields in
 * the package meta-data.
 */
class AccessoryFilter : public Object
{
public:
    AccessoryFilter(
        /* [in] */ const String& manufacturer,
        /* [in] */ const String& model,
        /* [in] */ const String& ver);

    AccessoryFilter(
        /* [in] */ IUsbAccessory* accessory);

    CAR_INTERFACE_DECL()

    /*
     * public static AccessoryFilter read(XmlPullParser parser) throws XmlPullParserException, IOException {...}
     */
    static CARAPI Read(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ AccessoryFilter** filter);

    /*
     * public void write(XmlSerializer serializer)throws IOException {...}
     */
    CARAPI Write(
        /* [in] */ IXmlSerializer* serializer);

    CARAPI_(Boolean) Matches(
        /* [in] */ IUsbAccessory* accessory);

    CARAPI_(Boolean) Matches(
        /* [in] */ AccessoryFilter* filter);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* res);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    // USB accessory manufacturer (or null for unspecified)
    String mManufacturer;
    // USB accessory model (or null for unspecified)
    String mModel;
    // USB accessory version (or null for unspecified)
    String mVersion;
};

} // namespace Usb
} // namespace Server
} // namepsace Droid
} // namespace Elastos

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Hardware::Usb::IUsbAccessory)
DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Server::Usb::DeviceFilter)
DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Server::Usb::AccessoryFilter)

namespace Elastos {
namespace Droid {
namespace Server {
namespace Usb {

typedef HashMap<Int32, Boolean> Int32BooleanMap;
typedef HashMap<Int32, Boolean>::Iterator Int32BooleanMapIterator;

class UsbSettingsManager : public Object
{
private:
    class MyPackageMonitor : public PackageMonitor
    {
    public:
        MyPackageMonitor(
            /* [in] */ UsbSettingsManager* host);

        CARAPI OnPackageAdded(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid);

        CARAPI OnPackageChanged(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid,
            /* [in] */ ArrayOf<String>* components,
            /* [out] */ Boolean* res);

        CARAPI OnPackageRemoved(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid);
    private:
        UsbSettingsManager* mHost;
    };

public:
    UsbSettingsManager(
        /* [in] */ IContext* context,
        /* [in] */ IUserHandle* userHandle);

    ~UsbSettingsManager();

    CARAPI_(void) DeviceAttached(
        /* [in] */ IUsbDevice* device);

    CARAPI_(void) DeviceDetached(
        /* [in] */ IUsbDevice* device);

    CARAPI_(void) AccessoryAttached(
        /* [in] */ IUsbAccessory* accessory);

    CARAPI_(void) AccessoryDetached(
        /* [in] */ IUsbAccessory* accessory);

    CARAPI_(Boolean) HasPermission(
        /* [in] */ IUsbDevice* device);

    CARAPI_(Boolean) HasPermission(
        /* [in] */ IUsbAccessory* accessory);

    CARAPI CheckPermission(
        /* [in] */ IUsbDevice* device);

    CARAPI CheckPermission(
        /* [in] */ IUsbAccessory* accessory);

    CARAPI RequestPermission(
        /* [in] */ IUsbDevice* device,
        /* [in] */ const String& packageName,
        /* [in] */ IPendingIntent* pi);

    CARAPI RequestPermission(
        /* [in] */ IUsbAccessory* accessory,
        /* [in] */ const String& packageName,
        /* [in] */ IPendingIntent* pi);

    CARAPI_(void) SetDevicePackage(
        /* [in] */ IUsbDevice* device,
        /* [in] */ const String& packageName);

    CARAPI_(void) SetAccessoryPackage(
        /* [in] */ IUsbAccessory* accessory,
        /* [in] */ const String& packageName);

    CARAPI_(void) GrantDevicePermission(
        /* [in] */ IUsbDevice* device,
        /* [in] */ Int32 uid);

    CARAPI_(void) GrantAccessoryPermission(
        /* [in] */ IUsbAccessory* accessory,
        /* [in] */ Int32 uid);

    CARAPI_(Boolean) HasDefaults(
        /* [in] */ const String& packageName);

    CARAPI_(void) ClearDefaults(
        /* [in] */ const String& packageName);

private:
    CARAPI ReadPreference(
        /* [in] */ IXmlPullParser* parser);

    /**
     * Upgrade any single-user settings from {@link #sSingleUserSettingsFile}.
     * Should only by called by owner.
     */
    CARAPI_(void) UpgradeSingleUserLocked();

    CARAPI_(void) ReadSettingsLocked();

    CARAPI_(void) WriteSettingsLocked();

    CARAPI_(Boolean) PackageMatchesLocked(
        /* [in] */ IResolveInfo* info,
        /* [in] */ const String& metaDataName,
        /* [in] */ IUsbDevice* device,
        /* [in] */ IUsbAccessory* accessory);

    CARAPI_(AutoPtr< List<AutoPtr<IResolveInfo> > >) GetDeviceMatchesLocked(
        /* [in] */ IUsbDevice* device,
        /* [in] */ IIntent* intent);

    CARAPI_(AutoPtr< List<AutoPtr<IResolveInfo> > >) GetAccessoryMatchesLocked(
        /* [in] */ IUsbAccessory* accessory,
        /* [in] */ IIntent* intent);

    CARAPI_(void) ResolveActivity(
        /* [in] */ IIntent* intent,
        /* [in] */ List<AutoPtr<IResolveInfo> >* matches,
        /* [in] */ const String& defaultPackage,
        /* [in] */ IUsbDevice* device,
        /* [in] */ IUsbAccessory* accessory);

    CARAPI_(Boolean) ClearCompatibleMatchesLocked(
        /* [in] */ const String& packageName,
        /* [in] */ DeviceFilter* filter);

    CARAPI_(Boolean) ClearCompatibleMatchesLocked(
        /* [in] */ const String& packageName,
        /* [in] */ AccessoryFilter* filter);

    CARAPI_(Boolean) HandlePackageUpdateLocked(
        /* [in] */ const String& packageName,
        /* [in] */ IActivityInfo* aInfo,
        /* [in] */ const String& metaDataName);

    CARAPI_(void) HandlePackageUpdate(
        /* [in] */ const String& packageName);

    CARAPI RequestPermissionDialog(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& packageName,
        /* [in] */ IPendingIntent* pi);

    CARAPI_(Boolean) ClearPackageDefaultsLocked(
        /* [in] */ const String& packageName);

private:
    static const String TAG;
    static const Boolean DEBUG;

    /** Legacy settings file, before multi-user */
    static AutoPtr<IFile> sSingleUserSettingsFile;

    AutoPtr<IUserHandle> mUser;
    AutoPtr<IAtomicFile> mSettingsFile;
    Boolean mDisablePermissionDialogs;

    AutoPtr<IContext> mContext;
    AutoPtr<IContext> mUserContext;
    AutoPtr<IPackageManager> mPackageManager;

    // Temporary mapping USB device name to list of UIDs with permissions for the device
    HashMap< String, AutoPtr<Int32BooleanMap> > mDevicePermissionMap;

    // Temporary mapping UsbAccessory to list of UIDs with permissions for the accessory
    HashMap< AutoPtr<IUsbAccessory>, AutoPtr<Int32BooleanMap> > mAccessoryPermissionMap;

    // Maps DeviceFilter to user preferred application package
    HashMap< AutoPtr<DeviceFilter>, String> mDevicePreferenceMap;

    // Maps AccessoryFilter to user preferred application package
    HashMap< AutoPtr<AccessoryFilter>, String> mAccessoryPreferenceMap;

    Object mLock;

    AutoPtr<MyPackageMonitor> mPackageMonitor;
};

} // namespace Usb
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_USB_USBSETTINGSMANAGER_H__
