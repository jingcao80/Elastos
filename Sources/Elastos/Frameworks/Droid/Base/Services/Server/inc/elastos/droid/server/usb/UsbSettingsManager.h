
#ifndef __ELASTOS_DROID_SERVER_USB_USBSETTINGSMANAGER_H__
#define __ELASTOS_DROID_SERVER_USB_USBSETTINGSMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/content/PackageMonitor.h"
#include "util/Xml.h"
#include "util/XmlUtils.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Hardware::Usb::IUsbAccessory;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Hardware::Usb::IUsbDevice;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Utility::XmlUtils;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IIoUtils;
using Elastos::IO::CIoUtils;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;


namespace Elastos {
namespace Droid {
namespace Server {
namespace Usb {

extern const InterfaceID EIID_DeviceFilter;
extern const InterfaceID EIID_AccessoryFilter;

typedef HashMap<Int32, Boolean> Int32BooleanMap;
typedef HashMap<Int32, Boolean>::Iterator Int32BooleanMapIterator;


class UsbSettingsManager : public ElRefBase
{
private:
    /*
     * This class is used to describe a USB device.
     * When used in HashMaps all values must be specified,
     * but wildcards can be used for any of the fields in
     * the package meta-data.
     */
    class DeviceFilter
        : public ElRefBase
        , public IInterface
    {
    public:
        DeviceFilter(
            /* [in] */ Int32 vid,
            /* [in] */ Int32 pid,
            /* [in] */ Int32 clasz,
            /* [in] */ Int32 subclass,
            /* [in] */ Int32 protocol);

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

        CARAPI_(Boolean) Equals(
            /* [in] */ IInterface* obj) const;

        CARAPI_(Int32) GetHashCode() const;

        CARAPI_(String) ToString();

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
    };

    /*
     * This class is used to describe a USB accessory.
     * When used in HashMaps all values must be specified,
     * but wildcards can be used for any of the fields in
     * the package meta-data.
     */
    class AccessoryFilter
        : public ElRefBase
        , public IInterface
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

        CARAPI_(Boolean) Equals(
            /* [in] */ IInterface* obj) const;

        CARAPI_(Int32) GetHashCode() const;

        CARAPI_(String) ToString();

    private:
        // USB accessory manufacturer (or null for unspecified)
        String mManufacturer;
        // USB accessory model (or null for unspecified)
        String mModel;
        // USB accessory version (or null for unspecified)
        String mVersion;
    };

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
            /* [in] */ ArrayOf<String>* components);

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

public:
    struct HashPK_DeviceFilter
    {
        size_t operator()(DeviceFilter* s) const
        {
            return (size_t)s->GetHashCode();
        }
    };

    struct PKEq_DeviceFilter
    {
        Boolean operator()(DeviceFilter* x, IInterface* y) const
        {
            return x->Equals(y);
        }
    };

    struct HashPK_AccessoryFilter
    {
        size_t operator()(const AccessoryFilter* s) const
        {
            return (size_t)s->GetHashCode();
        }
    };

    struct PKEq_AccessoryFilter
    {
        Boolean operator()(const AccessoryFilter* x, const IInterface* y) const
        {
            return x->Equals(const_cast<IInterface*>(y));
        }
    };

private:
    static const String TAG;
    static const Boolean DEBUG;

    /** Legacy settings file, before multi-user */
    static AutoPtr<IFile> sSingleUserSettingsFile;

    AutoPtr<IUserHandle> mUser;
    AutoPtr<IAtomicFile> mSettingsFile;

    AutoPtr<IContext> mContext;
    AutoPtr<IContext> mUserContext;
    AutoPtr<IPackageManager> mPackageManager;

    // Temporary mapping USB device name to list of UIDs with permissions for the device
    HashMap< String, AutoPtr<Int32BooleanMap> > mDevicePermissionMap;

    // Temporary mapping UsbAccessory to list of UIDs with permissions for the accessory
    HashMap< AutoPtr<IUsbAccessory>, AutoPtr<Int32BooleanMap> > mAccessoryPermissionMap;

    // Maps DeviceFilter to user preferred application package
    HashMap< AutoPtr<DeviceFilter>, String, HashPK_DeviceFilter, PKEq_DeviceFilter > mDevicePreferenceMap;

    // Maps AccessoryFilter to user preferred application package
    HashMap< AutoPtr<AccessoryFilter>, String, HashPK_AccessoryFilter, PKEq_AccessoryFilter > mAccessoryPreferenceMap;

    Object mLock;

    AutoPtr<MyPackageMonitor> mPackageMonitor;

    friend class MyPackageMonitor;
};

} // namespace Usb
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_USB_USBSETTINGSMANAGER_H__
