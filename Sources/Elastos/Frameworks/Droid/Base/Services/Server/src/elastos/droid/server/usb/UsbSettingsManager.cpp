
#include "usb/UsbSettingsManager.h"
#include "elastos/droid/os/UserHandle.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Utility::IFastXmlSerializer;
using Elastos::Droid::Utility::CFastXmlSerializer;
using Elastos::Droid::Hardware::Usb::IUsbInterface;
using Elastos::Droid::Hardware::Usb::IUsbManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Usb {

// {FA18F3C1-31E9-49AF-9D86-B3FBF893AB03}
extern const InterfaceID EIID_DeviceFilter =
    { 0xfa18f3c1, 0x31e9, 0x49af, { 0x9d, 0x86, 0xb3, 0xfb, 0xf8, 0x93, 0xab, 0x3 } };

// {0DCF6C2C-40B1-46DD-A41D-3BB07F01E38A}
extern const InterfaceID EIID_AccessoryFilter =
    { 0xdcf6c2c, 0x40b1, 0x46dd, { 0xa4, 0x1d, 0x3b, 0xb0, 0x7f, 0x1, 0xe3, 0x8a } };


const Boolean UsbSettingsManager::DEBUG = FALSE;
const String UsbSettingsManager::TAG("UsbSettingsManager");

static AutoPtr<IFile> InitSingleUserSettingsFile()
{
    AutoPtr<IFile> file;
    CFile::New(String("/data/system/usb_device_manager.xml"), (IFile**)&file);
    return file;
}

AutoPtr<IFile> UsbSettingsManager::sSingleUserSettingsFile = InitSingleUserSettingsFile();


UsbSettingsManager::DeviceFilter::DeviceFilter(
    /* [in] */ Int32 vid,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 clasz,
    /* [in] */ Int32 subclass,
    /* [in] */ Int32 protocol)
    : mVendorId(vid)
    , mProductId(pid)
    , mClass(clasz)
    , mSubclass(subclass)
    , mProtocol(protocol)
{}

UsbSettingsManager::DeviceFilter::DeviceFilter(
    /* [in] */ IUsbDevice* device)
{
    device->GetVendorId(&mVendorId);
    device->GetProductId(&mProductId);
    device->GetDeviceClass(&mClass);
    device->GetDeviceSubclass(&mSubclass);
    device->GetDeviceProtocol(&mProtocol);
}

PInterface UsbSettingsManager::DeviceFilter::Probe(
    /* [in]  */ REIID riid)
{
    if ( riid == EIID_IInterface) {
        return this;
    }
    else if (riid == EIID_DeviceFilter) {
        return reinterpret_cast<PInterface>(this);
    }
    return NULL;
}

UInt32 UsbSettingsManager::DeviceFilter::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 UsbSettingsManager::DeviceFilter::Release()
{
    return ElRefBase::Release();
}

ECode UsbSettingsManager::DeviceFilter::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return NOERROR;
}

ECode UsbSettingsManager::DeviceFilter::Read(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ DeviceFilter** filter)
{
    VALIDATE_NOT_NULL(filter);

    Int32 vendorId = -1;
    Int32 productId = -1;
    Int32 deviceClass = -1;
    Int32 deviceSubclass = -1;
    Int32 deviceProtocol = -1;

    Int32 count;
    parser->GetAttributeCount(&count);
    for (Int32 i = 0; i < count; i++) {
        String name;
        parser->GetAttributeName(i, &name);
        // All attribute values are ints
        String attrValue;
        parser->GetAttributeValue(i, &attrValue);
        Int32 value = StringUtils::ParseInt32(attrValue);

        if (String("vendor-id").Equals(name)) {
            vendorId = value;
        }
        else if (String("product-id").Equals(name)) {
            productId = value;
        }
        else if (String("class").Equals(name)) {
            deviceClass = value;
        }
        else if (String("subclass").Equals(name)) {
            deviceSubclass = value;
        }
        else if (String("protocol").Equals(name)) {
            deviceProtocol = value;
        }
    }

    *filter = new DeviceFilter(vendorId, productId, deviceClass, deviceSubclass, deviceProtocol);
    REFCOUNT_ADD(*filter);
    return NOERROR;
}

ECode UsbSettingsManager::DeviceFilter::Write(
    /* [in] */ IXmlSerializer* serializer)
{
    serializer->WriteStartTag(String(NULL), String("usb-device"));
    if (mVendorId != -1) {
        serializer->WriteAttribute(String(NULL), String("vendor-id"), StringUtils::Int32ToString(mVendorId));
    }
    if (mProductId != -1) {
        serializer->WriteAttribute(String(NULL), String("product-id"), StringUtils::Int32ToString(mProductId));
    }
    if (mClass != -1) {
        serializer->WriteAttribute(String(NULL), String("class"), StringUtils::Int32ToString(mClass));
    }
    if (mSubclass != -1) {
        serializer->WriteAttribute(String(NULL), String("subclass"), StringUtils::Int32ToString(mSubclass));
    }
    if (mProtocol != -1) {
        serializer->WriteAttribute(String(NULL), String("protocol"), StringUtils::Int32ToString(mProtocol));
    }
    return serializer->WriteEndTag(String(NULL), String("usb-device"));
}

Boolean UsbSettingsManager::DeviceFilter::Matches(
    /* [in] */ Int32 clasz,
    /* [in] */ Int32 subclass,
    /* [in] */ Int32 protocol)
{
    return ((mClass == -1 || clasz == mClass) &&
            (mSubclass == -1 || subclass == mSubclass) &&
            (mProtocol == -1 || protocol == mProtocol));
}

Boolean UsbSettingsManager::DeviceFilter::Matches(
    /* [in] */ IUsbDevice* device)
{
    Int32 vendorId;
    device->GetVendorId(&vendorId);
    if (mVendorId != -1 && vendorId != mVendorId) {
        return FALSE;
    }

    Int32 productId;
    device->GetProductId(&productId);
    if (mProductId != -1 && productId != mProductId) {
        return FALSE;
    }

    // check device class/subclass/protocol
    Int32 deviceCls;
    device->GetDeviceClass(&deviceCls);
    Int32 deviceSubCls;
    device->GetDeviceSubclass(&deviceSubCls);
    Int32 deviceProtocol;
    device->GetDeviceProtocol(&deviceProtocol);
    if (Matches(deviceCls, deviceSubCls, deviceProtocol)) {
        return TRUE;
    }

    // if device doesn't match, check the interfaces
    Int32 count = 0;
    device->GetInterfaceCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IUsbInterface> intf;
        device->GetInterface(i, (IUsbInterface**)&intf);
        Int32 intfCls;
        intf->GetInterfaceClass(&intfCls);
        Int32 intfSubCls;
        intf->GetInterfaceSubclass(&intfSubCls);
        Int32 intfProtocol;
        intf->GetInterfaceProtocol(&intfProtocol);
        if (Matches(intfCls, intfSubCls, intfProtocol)) {
            return TRUE;
        }
    }

    return FALSE;
}

Boolean UsbSettingsManager::DeviceFilter::Matches(
    /* [in] */ DeviceFilter* filter)
{
    if (mVendorId != -1 && filter->mVendorId != mVendorId) {
        return FALSE;
    }
    if (mProductId != -1 && filter->mProductId != mProductId) {
        return FALSE;
    }

    // check device class/subclass/protocol
    return Matches(filter->mClass, filter->mSubclass, filter->mProtocol);
}

Boolean UsbSettingsManager::DeviceFilter::Equals(
    /* [in] */ IInterface* obj) const
{
    // can't compare if we have wildcard strings
    if (mVendorId == -1 || mProductId == -1 ||
            mClass == -1 || mSubclass == -1 || mProtocol == -1) {
        return FALSE;
    }
    if (obj->Probe(EIID_DeviceFilter) != NULL) {
        AutoPtr<DeviceFilter> filter = (DeviceFilter*)obj;
        return (filter->mVendorId == mVendorId &&
                filter->mProductId == mProductId &&
                filter->mClass == mClass &&
                filter->mSubclass == mSubclass &&
                filter->mProtocol == mProtocol);
    }
    if (IUsbDevice::Probe(obj)) {
        AutoPtr<IUsbDevice> device = IUsbDevice::Probe(obj);
        Int32 vendorId, productId, deviceClass, subClass, protocol;
        return ((device->GetVendorId(&vendorId), vendorId == mVendorId) &&
                (device->GetProductId(&productId), productId == mProductId) &&
                (device->GetDeviceClass(&deviceClass), deviceClass == mClass) &&
                (device->GetDeviceSubclass(&subClass), subClass == mSubclass) &&
                (device->GetDeviceProtocol(&protocol), protocol == mProtocol));
    }
    return FALSE;
}

Int32 UsbSettingsManager::DeviceFilter::GetHashCode() const
{
    return (((mVendorId << 16) | mProductId) ^
            ((mClass << 16) | (mSubclass << 8) | mProtocol));
}

String UsbSettingsManager::DeviceFilter::ToString()
{
    StringBuilder builder("DeviceFilter[mVendorId=");
    builder += mVendorId;
    builder += ",mProductId=";
    builder += mProductId;
    builder += ",mClass=";
    builder += mClass;
    builder += ",mSubclass=";
    builder += mSubclass;
    builder += ",mProtocol=";
    builder += mProtocol;
    builder += "]";
    return builder.ToString();
}


UsbSettingsManager::AccessoryFilter::AccessoryFilter(
    /* [in] */ const String& manufacturer,
    /* [in] */ const String& model,
    /* [in] */ const String& ver)
    : mManufacturer(manufacturer)
    , mModel(model)
    , mVersion(ver)
{}

UsbSettingsManager::AccessoryFilter::AccessoryFilter(
    /* [in] */ IUsbAccessory* accessory)
{
    accessory->GetManufacturer(&mManufacturer);
    accessory->GetModel(&mModel);
    accessory->GetVersion(&mVersion);
}

PInterface UsbSettingsManager::AccessoryFilter::Probe(
    /* [in]  */ REIID riid)
{
    if ( riid == EIID_IInterface) {
        return this;
    }
    else if (riid == EIID_AccessoryFilter) {
        return reinterpret_cast<PInterface>(this);
    }
    return NULL;
}

UInt32 UsbSettingsManager::AccessoryFilter::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 UsbSettingsManager::AccessoryFilter::Release()
{
    return ElRefBase::Release();
}

ECode UsbSettingsManager::AccessoryFilter::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return NOERROR;
}

ECode UsbSettingsManager::AccessoryFilter::Read(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ AccessoryFilter** filter)
{
    VALIDATE_NOT_NULL(filter);

    String manufacturer;
    String model;
    String version;

    Int32 count = 0;
    parser->GetAttributeCount(&count);
    for (Int32 i = 0; i < count; i++) {
        String name;
        parser->GetAttributeName(i, &name);
        String value;
        parser->GetAttributeValue(i, &value);

        if (String("manufacturer").Equals(name)) {
            manufacturer = value;
        }
        else if (String("model").Equals(name)) {
            model = value;
        }
        else if (String("version").Equals(name)) {
            version = value;
        }
    }

    *filter = new AccessoryFilter(manufacturer, model, version);
    REFCOUNT_ADD(*filter);
    return NOERROR;
}

ECode UsbSettingsManager::AccessoryFilter::Write(
    /* [in] */ IXmlSerializer* serializer)
{
    serializer->WriteStartTag(String(NULL), String("usb-accessory"));
    if (mManufacturer != NULL) {
        serializer->WriteAttribute(String(NULL), String("manufacturer"), mManufacturer);
    }
    if (mModel != NULL) {
        serializer->WriteAttribute(String(NULL), String("model"), mModel);
    }
    if (mVersion != NULL) {
        serializer->WriteAttribute(String(NULL), String("version"), mVersion);
    }
    return serializer->WriteEndTag(String(NULL), String("usb-accessory"));
}

Boolean UsbSettingsManager::AccessoryFilter::Matches(
    /* [in] */ IUsbAccessory* accessory)
{
    String manufacturer;
    if (!mManufacturer.IsNull() && (accessory->GetManufacturer(&manufacturer), !manufacturer.Equals(mManufacturer))) {
        return FALSE;
    }
    String model;
    if (!mModel.IsNull() && (accessory->GetModel(&model), !model.Equals(mModel))) {
        return FALSE;
    }
    String ver;
    if (!mVersion.IsNull() && (accessory->GetVersion(&ver), !ver.Equals(mVersion))) {
        return FALSE;
    }
    return TRUE;
}

Boolean UsbSettingsManager::AccessoryFilter::Matches(
    /* [in] */ AccessoryFilter* filter)
{
    if (!mManufacturer.IsNull() && !filter->mManufacturer.Equals(mManufacturer)) {
        return FALSE;
    }
    if (!mModel.IsNull() && !filter->mModel.Equals(mModel)) {
        return FALSE;
    }
    if (!mVersion.IsNull() && !filter->mVersion.Equals(mVersion)) {
        return FALSE;
    }
    return TRUE;
}

Boolean UsbSettingsManager::AccessoryFilter::Equals(
    /* [in] */ IInterface* obj) const
{
    // can't compare if we have wildcard strings
    if (mManufacturer == NULL || mModel == NULL || mVersion == NULL) {
        return FALSE;
    }
    if (obj->Probe(EIID_AccessoryFilter) != NULL) {
        AutoPtr<AccessoryFilter> filter = (AccessoryFilter*)obj;
        return (mManufacturer.Equals(filter->mManufacturer) &&
                mModel.Equals(filter->mModel) &&
                mVersion.Equals(filter->mVersion));
    }
    if (IUsbAccessory::Probe(obj)) {
        AutoPtr<IUsbAccessory> accessory = IUsbAccessory::Probe(obj);
        String manufacturer, model, version;
        accessory->GetManufacturer(&manufacturer);
        accessory->GetModel(&model);
        accessory->GetVersion(&version);
        return (mManufacturer.Equals(manufacturer) && mModel.Equals(model) && mVersion.Equals(version));
    }
    return FALSE;
}

Int32 UsbSettingsManager::AccessoryFilter::GetHashCode() const
{
    return ((mManufacturer.IsNull() ? 0 : mManufacturer.GetHashCode()) ^
            (mModel.IsNull() ? 0 : mModel.GetHashCode()) ^
            (mVersion.IsNull() ? 0 : mVersion.GetHashCode()));
}

String UsbSettingsManager::AccessoryFilter::ToString()
{
    StringBuilder builder("AccessoryFilter[mManufacturer=\"");
    builder += mManufacturer;
    builder += "\", mModel=\"";
    builder += mModel;
    builder += "\", mVersion=\"";
    builder += mVersion;
    builder += "\"]";
    return builder.ToString();
}


UsbSettingsManager::MyPackageMonitor::MyPackageMonitor(
    /* [in] */ UsbSettingsManager* host)
    : mHost(host)
{}

ECode UsbSettingsManager::MyPackageMonitor::OnPackageAdded(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    mHost->HandlePackageUpdate(packageName);
    return NOERROR;
}

ECode UsbSettingsManager::MyPackageMonitor::OnPackageChanged(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ ArrayOf<String>* components)
{
    mHost->HandlePackageUpdate(packageName);
    return NOERROR;
}

ECode UsbSettingsManager::MyPackageMonitor::OnPackageRemoved(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    mHost->ClearDefaults(packageName);
    return NOERROR;
}


UsbSettingsManager::UsbSettingsManager(
    /* [in] */ IContext* context,
    /* [in] */ IUserHandle* userHandle)
{
    mPackageMonitor = new MyPackageMonitor(this);

    if (DEBUG) Slogger::V(TAG, "Creating settings for %p", userHandle);

    if (FAILED(context->CreatePackageContextAsUser(String("android"), 0,
            userHandle, (IContext**)&mUserContext))) {
        Slogger::E(TAG, "Missing android package");
        assert(0);
    }

    mContext = context;
    mUserContext->GetPackageManager((IPackageManager**)&mPackageManager);

    mUser = userHandle;
    Int32 userId;
    userHandle->GetIdentifier(&userId);
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> systemDir;
    env->GetUserSystemDirectory(userId, (IFile**)&systemDir);
    AutoPtr<IFile> baseName;
    CFile::New(systemDir, String("usb_device_manager.xml"), (IFile**)&baseName);
    CAtomicFile::New(baseName, (IAtomicFile**)&mSettingsFile);

    {
        AutoLock lock(mLock);

        Boolean isEquals;
        if (UserHandle::OWNER->Equals(userHandle, &isEquals)) {
            UpgradeSingleUserLocked();
        }
        ReadSettingsLocked();
    }

    mPackageMonitor->Register(mUserContext, NULL, TRUE);
}

UsbSettingsManager::~UsbSettingsManager()
{
    mDevicePermissionMap.Clear();
    mAccessoryPermissionMap.Clear();

    mDevicePreferenceMap.Clear();
    mAccessoryPreferenceMap.Clear();
}

ECode UsbSettingsManager::ReadPreference(
    /* [in] */ IXmlPullParser* parser)
{
    String packageName;
    Int32 count = 0;
    parser->GetAttributeCount(&count);
    for (Int32 i = 0; i < count; i++) {
        String name;
        parser->GetAttributeName(i, &name);
        if (name.Equals("package")) {
            parser->GetAttributeValue(i, &packageName);
            break;
        }
    }
    FAIL_RETURN(XmlUtils::NextElement(parser));
    String elmtName;
    parser->GetName(&elmtName);
    if (elmtName.Equals("usb-device")) {
        AutoPtr<DeviceFilter> filter;
        FAIL_RETURN(UsbSettingsManager::DeviceFilter::Read(parser, (DeviceFilter**)&filter));
        mDevicePreferenceMap[filter] = packageName;
    }
    else if (elmtName.Equals("usb-accessory")) {
        AutoPtr<AccessoryFilter> filter;
        FAIL_RETURN(UsbSettingsManager::AccessoryFilter::Read(parser, (AccessoryFilter**)&filter));
        mAccessoryPreferenceMap[filter] = packageName;
    }
    return XmlUtils::NextElement(parser);
}

void UsbSettingsManager::UpgradeSingleUserLocked()
{
    Boolean isExist;
    if (sSingleUserSettingsFile->Exists(&isExist), isExist) {
        mDevicePreferenceMap.Clear();
        mAccessoryPreferenceMap.Clear();

        AutoPtr<IFileInputStream> fis;
        // try {
        CFileInputStream::New(sSingleUserSettingsFile, (IFileInputStream**)&fis);

        AutoPtr<IXmlPullParser> parser = Xml::NewPullParser();
        parser->SetInput(fis, String(NULL));

        Int32 eventType;

        if (FAILED(XmlUtils::NextElement(parser))) {
            Slogger::W(TAG, "Failed to read single-user settings");
            goto failed;
        }

        while (parser->GetEventType(&eventType), eventType != IXmlPullParser::END_DOCUMENT) {
            String tagName;
            parser->GetName(&tagName);
            if (tagName.Equals("preference")) {
                if (FAILED(ReadPreference(parser))) {
                    Slogger::W(TAG, "Failed to read single-user settings");
                    break;
                }
            }
            else {
                if (FAILED(XmlUtils::NextElement(parser))) {
                    Slogger::W(TAG, "Failed to read single-user settings");
                    break;
                }
            }
        }
        // } catch (IOException e) {
        //     Log.wtf(TAG, "Failed to read single-user settings", e);
        // } catch (XmlPullParserException e) {
        //     Log.wtf(TAG, "Failed to read single-user settings", e);
        // } finally {
        //     IoUtils.closeQuietly(fis);
        // }
failed:
        AutoPtr<IIoUtils> ioUtils;
        CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
        ioUtils->CloseQuietly(fis);

        WriteSettingsLocked();

        // Success or failure, we delete single-user file
        Boolean isDeleted;
        sSingleUserSettingsFile->Delete(&isDeleted);
    }
}

void UsbSettingsManager::ReadSettingsLocked()
{
    if (DEBUG) Slogger::V(TAG, "readSettingsLocked()");

    mDevicePreferenceMap.Clear();
    mAccessoryPreferenceMap.Clear();

    AutoPtr<IFileInputStream> stream;
    // try {
    ECode ec = mSettingsFile->OpenRead((IFileInputStream**)&stream);
    if (FAILED(ec)) {
        if (DEBUG) Slogger::D(TAG, "settings file not found");
        AutoPtr<IIoUtils> ioUtils;
        CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
        ioUtils->CloseQuietly(stream);
        return;
    }
    AutoPtr<IXmlPullParser> parser = Xml::NewPullParser();
    parser->SetInput(stream, String(NULL));

    Int32 eventType;

    if (FAILED(XmlUtils::NextElement(parser))) {
        Slogger::E(TAG, "error reading settings file, deleting to start fresh 0x%08x", ec);
        mSettingsFile->Delete();
        goto failed;
    }
    while (parser->GetEventType(&eventType), eventType != IXmlPullParser::END_DOCUMENT) {
        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals("preference")) {
            ec = ReadPreference(parser);
            if (FAILED(ec)) {
                Slogger::E(TAG, "error reading settings file, deleting to start fresh 0x%08x", ec);
                mSettingsFile->Delete();
                break;
            }
        }
        else {
            if (FAILED(XmlUtils::NextElement(parser))) {
                Slogger::E(TAG, "error reading settings file, deleting to start fresh 0x%08x", ec);
                mSettingsFile->Delete();
                break;
            }
        }
    }
    // } catch (FileNotFoundException e) {
    //     if (DEBUG) Slog.d(TAG, "settings file not found");
    // } catch (Exception e) {
    //     Slog.e(TAG, "error reading settings file, deleting to start fresh", e);
    //     mSettingsFile.delete();
    // } finally {
    //     IoUtils.closeQuietly(stream);
    // }

failed:
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    ioUtils->CloseQuietly(stream);
}

void UsbSettingsManager::WriteSettingsLocked()
{
    if (DEBUG) Slogger::V(TAG, "writeSettingsLocked()");

    AutoPtr<IFileOutputStream> fos;
    // try {
    ECode ec = mSettingsFile->StartWrite((IFileOutputStream**)&fos);
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "Failed to write settings 0x%08x", ec);
        if (fos != NULL) {
            mSettingsFile->FailWrite(fos);
        }
        return;
    }

    AutoPtr<IFastXmlSerializer> serializer;
    CFastXmlSerializer::New((IFastXmlSerializer**)&serializer);
    ec = serializer->SetOutput(fos, String("utf-8"));
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "Failed to write settings 0x%08x", ec);
        if (fos != NULL) {
            mSettingsFile->FailWrite(fos);
        }
    }

    AutoPtr<IBoolean> b;
    CBoolean::New(TRUE, (IBoolean**)&b);
    ec = serializer->StartDocument(String(NULL), b);
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "Failed to write settings 0x%08x", ec);
        if (fos != NULL) {
            mSettingsFile->FailWrite(fos);
        }
    }

    ec = serializer->SetFeature(String("http://xmlpull.org/v1/doc/features.html#indent-output"), TRUE);
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "Failed to write settings 0x%08x", ec);
        if (fos != NULL) {
            mSettingsFile->FailWrite(fos);
        }
    }

    ec = serializer->WriteStartTag(String(NULL), String("settings"));
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "Failed to write settings 0x%08x", ec);
        if (fos != NULL) {
            mSettingsFile->FailWrite(fos);
        }
    }

    HashMap< AutoPtr<DeviceFilter>, String, HashPK_DeviceFilter, PKEq_DeviceFilter >::Iterator devFilterIt
            = mDevicePreferenceMap.Begin();
    for (; devFilterIt != mDevicePreferenceMap.End(); ++devFilterIt) {
        AutoPtr<DeviceFilter> filter = devFilterIt->mFirst;
        ec = serializer->WriteStartTag(String(NULL), String("preference"));
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::E(TAG, "Failed to write settings 0x%08x", ec);
            if (fos != NULL) {
                mSettingsFile->FailWrite(fos);
            }
        }
        ec = serializer->WriteAttribute(String(NULL), String("package"), devFilterIt->mSecond);
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::E(TAG, "Failed to write settings 0x%08x", ec);
            if (fos != NULL) {
                mSettingsFile->FailWrite(fos);
            }
        }
        filter->Write(serializer);
        ec = serializer->WriteEndTag(String(NULL), String("preference"));
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::E(TAG, "Failed to write settings 0x%08x", ec);
            if (fos != NULL) {
                mSettingsFile->FailWrite(fos);
            }
        }
    }

    HashMap< AutoPtr<AccessoryFilter>, String, HashPK_AccessoryFilter, PKEq_AccessoryFilter >::Iterator accFilterIt
            = mAccessoryPreferenceMap.Begin();
    for (; accFilterIt != mAccessoryPreferenceMap.End(); ++accFilterIt) {
        AutoPtr<AccessoryFilter> filter = accFilterIt->mFirst;
        ec = serializer->WriteStartTag(String(NULL), String("preference"));
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::E(TAG, "Failed to write settings 0x%08x", ec);
            if (fos != NULL) {
                mSettingsFile->FailWrite(fos);
            }
        }
        ec = serializer->WriteAttribute(String(NULL), String("package"), accFilterIt->mSecond);
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::E(TAG, "Failed to write settings 0x%08x", ec);
            if (fos != NULL) {
                mSettingsFile->FailWrite(fos);
            }
        }
        filter->Write(serializer);
        ec = serializer->WriteEndTag(String(NULL), String("preference"));
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::E(TAG, "Failed to write settings 0x%08x", ec);
            if (fos != NULL) {
                mSettingsFile->FailWrite(fos);
            }
        }
    }


    ec = serializer->WriteEndTag(String(NULL), String("settings"));
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "Failed to write settings 0x%08x", ec);
        if (fos != NULL) {
            mSettingsFile->FailWrite(fos);
        }
    }
    ec = serializer->EndDocument();
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "Failed to write settings 0x%08x", ec);
        if (fos != NULL) {
            mSettingsFile->FailWrite(fos);
        }
    }

    mSettingsFile->FinishWrite(fos);
    return;
    // } catch (IOException e) {
    //     Slog.e(TAG, "Failed to write settings", e);
    //     if (fos != null) {
    //         mSettingsFile.failWrite(fos);
    //     }
    // }
}

Boolean UsbSettingsManager::PackageMatchesLocked(
    /* [in] */ IResolveInfo* info,
    /* [in] */ const String& metaDataName,
    /* [in] */ IUsbDevice* device,
    /* [in] */ IUsbAccessory* accessory)
{
    AutoPtr<IActivityInfo> ai;
    info->GetActivityInfo((IActivityInfo**)&ai);

    AutoPtr<IXmlResourceParser> parser;

    // try {
    ECode ec = ai->LoadXmlMetaData(mPackageManager, metaDataName, (IXmlResourceParser**)&parser);
    if (FAILED(ec)) {
        String str;
        info->ToString(&str);
        Slogger::W(TAG, "Unable to load component info %s 0x%08x", str.string(), ec);
        goto failed;
    }
    if (parser == NULL) {
        Slogger::W(TAG, "no meta-data for %p", info);
        return FALSE;
    }

    ec = XmlUtils::NextElement(parser);
    if (FAILED(ec)) {
        String str;
        info->ToString(&str);
        Slogger::W(TAG, "Unable to load component info %s 0x%08x", str.string(), ec);
        goto failed;
    }

    Int32 eventType;
    while (parser->GetEventType(&eventType), eventType != IXmlPullParser::END_DOCUMENT) {
        String tagName;
        parser->GetName(&tagName);
        if (device != NULL && tagName.Equals("usb-device")) {
            AutoPtr<DeviceFilter> filter;
            ec = UsbSettingsManager::DeviceFilter::Read(parser, (DeviceFilter**)&filter);
            if (FAILED(ec)) {
                String str;
                info->ToString(&str);
                Slogger::W(TAG, "Unable to load component info %s 0x%08x", str.string(), ec);
                break;
            }
            if (filter->Matches(device)) {
                if (parser) parser->Close();
                return TRUE;
            }
        }
        else if (accessory != NULL && tagName.Equals("usb-accessory")) {
            AutoPtr<AccessoryFilter> filter;
            ec = UsbSettingsManager::AccessoryFilter::Read(parser, (AccessoryFilter**)&filter);
            if (FAILED(ec)) {
                String str;
                info->ToString(&str);
                Slogger::W(TAG, "Unable to load component info %s 0x%08x", str.string(), ec);
                break;
            }
            if (filter->Matches(accessory)) {
                if (parser) parser->Close();
                return TRUE;
            }
        }
        ec = XmlUtils::NextElement(parser);
        if (FAILED(ec)) {
            String str;
            info->ToString(&str);
            Slogger::W(TAG, "Unable to load component info %s 0x%08x", str.string(), ec);
            break;
        }
    }
    // } catch (Exception e) {
    //     Slog.w(TAG, "Unable to load component info " + info.toString(), e);
    // } finally {
    //     if (parser != null) parser.close();
    // }
failed:
    if (parser != NULL) {
        parser->Close();
    }
    return FALSE;
}

AutoPtr<List<AutoPtr<IResolveInfo> > > UsbSettingsManager::GetDeviceMatchesLocked(
    /* [in] */ IUsbDevice* device,
    /* [in] */ IIntent* intent)
{
    AutoPtr<List<AutoPtr<IResolveInfo> > > matches = new List<AutoPtr<IResolveInfo> >();
    AutoPtr<IObjectContainer> resolveInfos;
    mPackageManager->QueryIntentActivities(intent, IPackageManager::GET_META_DATA, (IObjectContainer**)&resolveInfos);
    AutoPtr<IObjectEnumerator> enumerator;
    resolveInfos->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
    Boolean hasNext;
    while (enumerator->MoveNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        enumerator->Current((IInterface**)&obj);
        IResolveInfo* resolveInfo = IResolveInfo::Probe(obj);
        String action;
        intent->GetAction(&action);
        if (PackageMatchesLocked(resolveInfo, action, device, NULL)) {
            matches->PushBack(resolveInfo);
        }
    }
    return matches;
}

AutoPtr<List<AutoPtr<IResolveInfo> > > UsbSettingsManager::GetAccessoryMatchesLocked(
    /* [in] */ IUsbAccessory* accessory,
    /* [in] */ IIntent* intent)
{
    AutoPtr<List<AutoPtr<IResolveInfo> > > matches = new List<AutoPtr<IResolveInfo> >();
    AutoPtr<IObjectContainer> resolveInfos;
    mPackageManager->QueryIntentActivities(intent, IPackageManager::GET_META_DATA, (IObjectContainer**)&resolveInfos);
    AutoPtr<IObjectEnumerator> enumerator;
    resolveInfos->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
    Boolean hasNext;
    while (enumerator->MoveNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        enumerator->Current((IInterface**)&obj);
        IResolveInfo* resolveInfo = IResolveInfo::Probe(obj);
        String action;
        intent->GetAction(&action);
        if (PackageMatchesLocked(resolveInfo, action, NULL, accessory)) {
            matches->PushBack(resolveInfo);
        }
    }
    return matches;
}

void UsbSettingsManager::DeviceAttached(
    /* [in] */ IUsbDevice* device)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IUsbManager::ACTION_USB_DEVICE_ATTACHED, (IIntent**)&intent);
    intent->PutExtra(IUsbManager::EXTRA_DEVICE, IParcelable::Probe(device));
    intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);

    AutoPtr<List<AutoPtr<IResolveInfo> > > matches;
    String defaultPackage;
    {
        AutoLock lock(mLock);
        matches = GetDeviceMatchesLocked(device, intent);
        // Launch our default activity directly, if we have one.
        // Otherwise we will start the UsbResolverActivity to allow the user to choose.
        AutoPtr<DeviceFilter> filter = new DeviceFilter(device);
        HashMap< AutoPtr<DeviceFilter>, String, HashPK_DeviceFilter, PKEq_DeviceFilter >::Iterator it
                = mDevicePreferenceMap.Find(filter);
        if (it != mDevicePreferenceMap.End()) {
            defaultPackage = it->mSecond;
        }
    }

    // Send broadcast to running activity with registered intent
    mUserContext->SendBroadcast(intent);

    // Start activity with registered intent
    ResolveActivity(intent, matches, defaultPackage, device, NULL);
}

void UsbSettingsManager::DeviceDetached(
    /* [in] */ IUsbDevice* device)
{
    // clear temporary permissions for the device
    String name;
    device->GetDeviceName(&name);
    mDevicePermissionMap.Erase(name);

    AutoPtr<IIntent> intent;
    CIntent::New(IUsbManager::ACTION_USB_DEVICE_DETACHED, (IIntent**)&intent);
    intent->PutExtra(IUsbManager::EXTRA_DEVICE, IParcelable::Probe(device));
    if (DEBUG) Slogger::D(TAG, "usbDeviceRemoved, sending %p", intent.Get());
    mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
}

void UsbSettingsManager::AccessoryAttached(
    /* [in] */ IUsbAccessory* accessory)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IUsbManager::ACTION_USB_ACCESSORY_ATTACHED, (IIntent**)&intent);
    intent->PutExtra(IUsbManager::EXTRA_ACCESSORY, IParcelable::Probe(accessory));
    intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);

    AutoPtr<List<AutoPtr<IResolveInfo> > > matches;
    String defaultPackage;
    {
        AutoLock lock(mLock);
        matches = GetAccessoryMatchesLocked(accessory, intent);
        /*
         * Launch our default activity directly, if we have one.
         * Otherwise we will start the UsbResolverActivity to allow the user to choose.
         */
        AutoPtr<AccessoryFilter> filter = new AccessoryFilter(accessory);
        HashMap< AutoPtr<AccessoryFilter>, String, HashPK_AccessoryFilter, PKEq_AccessoryFilter >::Iterator it
                = mAccessoryPreferenceMap.Find(filter);
        if (it != mAccessoryPreferenceMap.End()) {
            defaultPackage = it->mSecond;
        }
    }

    ResolveActivity(intent, matches, defaultPackage, NULL, accessory);
}

void UsbSettingsManager::AccessoryDetached(
    /* [in] */ IUsbAccessory* accessory)
{
    // clear temporary permissions for the accessory
    mAccessoryPermissionMap.Erase(accessory);

    AutoPtr<IIntent> intent;
    CIntent::New(IUsbManager::ACTION_USB_ACCESSORY_DETACHED, (IIntent**)&intent);
    intent->PutExtra(IUsbManager::EXTRA_ACCESSORY, IParcelable::Probe(accessory));
    mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
}

void UsbSettingsManager::ResolveActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ List<AutoPtr<IResolveInfo> >* matches,
    /* [in] */ const String& defaultPackage,
    /* [in] */ IUsbDevice* device,
    /* [in] */ IUsbAccessory* accessory)
{
    // don't show the resolver activity if there are no choices available
    if (matches->Begin() == matches->End()) {
        if (accessory != NULL) {
            String uri;
            accessory->GetUri(&uri);
            if (!uri.IsNullOrEmpty()) {
                // display URI to user
                // start UsbResolverActivity so user can choose an activity
                AutoPtr<IIntent> dialogIntent;
                CIntent::New((IIntent**)&dialogIntent);
                dialogIntent->SetClassName(String("com.android.systemui"), String("com.android.systemui.usb.UsbAccessoryUriActivity"));
                dialogIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
                dialogIntent->PutExtra(IUsbManager::EXTRA_ACCESSORY, IParcelable::Probe(accessory));
                dialogIntent->PutExtra(String("uri"), uri);
                // try {
                if (FAILED(mUserContext->StartActivityAsUser(dialogIntent, mUser))) {
                    Slogger::E(TAG, "unable to start UsbAccessoryUriActivity");
                }
                // } catch (ActivityNotFoundException e) {
                //     Slog.e(TAG, "unable to start UsbAccessoryUriActivity");
                // }
            }
        }

        // do nothing
        return;
    }

    AutoPtr<IResolveInfo> defaultRI;
    Int32 count = matches->GetSize();
    if (count == 1 && defaultPackage == NULL) {
        // Check to see if our single choice is on the system partition.
        // If so, treat it as our default without calling UsbResolverActivity
        AutoPtr<IResolveInfo> rInfo = *matches->Begin();
        AutoPtr<IActivityInfo> activityInfo;
        rInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
        if (activityInfo != NULL) {
            AutoPtr<IApplicationInfo> applicationInfo;
            activityInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
            if (applicationInfo != NULL) {
                Int32 flags;
                applicationInfo->GetFlags(&flags);
                if ((flags & IApplicationInfo::FLAG_SYSTEM) != 0) {
                    defaultRI = rInfo;
                }
            }
        }
    }

    if (defaultRI == NULL && !defaultPackage.IsNull()) {
        // look for default activity
        for (int i = 0; i < count; i++) {
            AutoPtr<IResolveInfo> rInfo = (*matches)[i];
            AutoPtr<IActivityInfo> aInfo;
            rInfo->GetActivityInfo((IActivityInfo**)&aInfo);
            if (aInfo != NULL) {
                String pName;
                aInfo->GetPackageName(&pName);
                if (defaultPackage.Equals(pName)) {
                    defaultRI = rInfo;
                    break;
                }
            }
        }
    }

    if (defaultRI != NULL) {
        // grant permission for default activity
        if (device != NULL) {
            AutoPtr<IActivityInfo> activityInfo;
            defaultRI->GetActivityInfo((IActivityInfo**)&activityInfo);
            AutoPtr<IApplicationInfo> appInfo;
            activityInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 uid;
            appInfo->GetUid(&uid);
            GrantDevicePermission(device, uid);
        }
        else if (accessory != NULL) {
            AutoPtr<IActivityInfo> activityInfo;
            defaultRI->GetActivityInfo((IActivityInfo**)&activityInfo);
            AutoPtr<IApplicationInfo> appInfo;
            activityInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 uid;
            appInfo->GetUid(&uid);
            GrantAccessoryPermission(accessory, uid);
        }

        // start default activity directly
        // try {
        AutoPtr<IActivityInfo> activityInfo;
        defaultRI->GetActivityInfo((IActivityInfo**)&activityInfo);
        String packageName;
        activityInfo->GetPackageName(&packageName);
        String name;
        activityInfo->GetName(&name);
        AutoPtr<IComponentName> compName;
        CComponentName::New(packageName, name, (IComponentName**)&compName);
        intent->SetComponent(compName);

        if (FAILED(mUserContext->StartActivityAsUser(intent, mUser))) {
            Slogger::E(TAG, "startActivity failed");
        }
        // } catch (ActivityNotFoundException e) {
        //     Slog.e(TAG, "startActivity failed", e);
        // }
    }
    else {
        AutoPtr<IIntent> resolverIntent;
        CIntent::New((IIntent**)&resolverIntent);
        resolverIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);

        if (count == 1) {
            // start UsbConfirmActivity if there is only one choice
            resolverIntent->SetClassName(String("com.android.systemui"), String("com.android.systemui.usb.UsbConfirmActivity"));

            AutoPtr<IResolveInfo> info = *matches->Begin();
            resolverIntent->PutExtra(String("rinfo"), IParcelable::Probe(info));\

            if (device != NULL) {
                resolverIntent->PutExtra(IUsbManager::EXTRA_DEVICE, IParcelable::Probe(device));
            }
            else {
                resolverIntent->PutExtra(IUsbManager::EXTRA_ACCESSORY, IParcelable::Probe(accessory));
            }
        }
        else {
            // start UsbResolverActivity so user can choose an activity
            resolverIntent->SetClassName(String("com.android.systemui"), String("com.android.systemui.usb.UsbResolverActivity"));
            AutoPtr< ArrayOf<IParcelable*> > matchesArray = ArrayOf<IParcelable*>::Alloc(count);
            List<AutoPtr<IResolveInfo> >::Iterator it = matches->Begin();
            for (Int32 i = 0; it != matches->End(); ++it, ++i) {
                AutoPtr<IParcelable> parcel = IParcelable::Probe((*it).Get());
                matchesArray->Set(i, parcel);
            }
            resolverIntent->PutExtra(String("rlist"), matchesArray);
            resolverIntent->PutExtra(IIntent::EXTRA_INTENT, IParcelable::Probe(intent));
        }
        // try {
        if (FAILED(mUserContext->StartActivityAsUser(resolverIntent, mUser))) {
            Slogger::E(TAG, "unable to start activity %p", resolverIntent.Get());
        }
        // } catch (ActivityNotFoundException e) {
        //     Slog.e(TAG, "unable to start activity " + resolverIntent);
        // }
    }
}

Boolean UsbSettingsManager::ClearCompatibleMatchesLocked(
    /* [in] */ const String& packageName,
    /* [in] */ DeviceFilter* filter)
{
    Boolean changed = FALSE;
    HashMap< AutoPtr<DeviceFilter>, String, HashPK_DeviceFilter, PKEq_DeviceFilter >::Iterator it
            = mDevicePreferenceMap.Begin();
    for (; it != mDevicePreferenceMap.End();) {
        AutoPtr<DeviceFilter> test = it->mFirst;
        if (filter->Matches(test)) {
            mDevicePreferenceMap.Erase(it++);
            changed = TRUE;
        }
        else {
            ++it;
        }
    }
    return changed;
}

Boolean UsbSettingsManager::ClearCompatibleMatchesLocked(
    /* [in] */ const String& packageName,
    /* [in] */ AccessoryFilter* filter)
{
    Boolean changed = FALSE;
    HashMap< AutoPtr<AccessoryFilter>, String, HashPK_AccessoryFilter, PKEq_AccessoryFilter >::Iterator it
            = mAccessoryPreferenceMap.Begin();
    for (; it != mAccessoryPreferenceMap.End();) {
        AutoPtr<AccessoryFilter> test = it->mFirst;
        if (filter->Matches(test)) {
            mAccessoryPreferenceMap.Erase(it++);
            changed = TRUE;
        }
        else {
            ++it;
        }
    }
    return changed;
}

Boolean UsbSettingsManager::HandlePackageUpdateLocked(
    /* [in] */ const String& packageName,
    /* [in] */ IActivityInfo* aInfo,
    /* [in] */ const String& metaDataName)
{
    AutoPtr<IXmlResourceParser> parser;
    Boolean changed = FALSE;

    // try {
    ECode ec = aInfo->LoadXmlMetaData(mPackageManager, metaDataName, (IXmlResourceParser**)&parser);
    if (FAILED(ec)) {
        String s;
        aInfo->ToString(&s);
        Slogger::W(TAG, "Unable to load component info %s 0x%08x", s.string(), ec);
        return FALSE;
    }
    if (parser == NULL) {
        return FALSE;
    }

    ec = XmlUtils::NextElement(parser);
    if (FAILED(ec)) {
        String s;
        aInfo->ToString(&s);
        Slogger::W(TAG, "Unable to load component info %s 0x%08x", s.string(), ec);
        if (parser != NULL) {
            parser->Close();
        }
        return changed;
    }
    Int32 eventType;
    while (parser->GetEventType(&eventType), eventType != IXmlPullParser::END_DOCUMENT) {
        String tagName;
        parser->GetName(&tagName);

        if (tagName.Equals("usb-device")) {
            AutoPtr<DeviceFilter> filter;
            ec = UsbSettingsManager::DeviceFilter::Read(parser, (DeviceFilter**)&filter);
            if (FAILED(ec)) {
                String s;
                aInfo->ToString(&s);
                Slogger::W(TAG, "Unable to load component info %s 0x%08x", s.string(), ec);
                if (parser != NULL) {
                    parser->Close();
                }
                return changed;
            }
            if (ClearCompatibleMatchesLocked(packageName, filter)) {
                changed = TRUE;
            }
        }
        else if (tagName.Equals("usb-accessory")) {
            AutoPtr<AccessoryFilter> filter;
            ec = UsbSettingsManager::AccessoryFilter::Read(parser, (AccessoryFilter**)&filter);
            if (FAILED(ec)) {
                String s;
                aInfo->ToString(&s);
                Slogger::W(TAG, "Unable to load component info %s 0x%08x", s.string(), ec);
                if (parser != NULL) {
                    parser->Close();
                }
                return changed;
            }
            if (ClearCompatibleMatchesLocked(packageName, filter)) {
                changed = TRUE;
            }
        }
        ec = XmlUtils::NextElement(parser);
        if (FAILED(ec)) {
            String s;
            aInfo->ToString(&s);
            Slogger::W(TAG, "Unable to load component info %s 0x%08x", s.string(), ec);
            if (parser != NULL) {
                parser->Close();
            }
            return changed;
        }
    }

    if (parser != NULL) {
        parser->Close();
    }
    // } catch (Exception e) {
    //     Slog.w(TAG, "Unable to load component info " + aInfo.toString(), e);
    // } finally {
    //     if (parser != null) parser.close();
    // }

    return changed;
}

void UsbSettingsManager::HandlePackageUpdate(
    /* [in] */ const String& packageName)
{
    /*
     * Check to see if the package supports any USB devices or accessories.
     * If so, clear any non-matching preferences for matching devices/accessories.
     */
    AutoLock lock(mLock);

    AutoPtr<IPackageInfo> info;
    Boolean changed = FALSE;

    ECode ec = mPackageManager->GetPackageInfo(packageName,
            IPackageManager::GET_ACTIVITIES | IPackageManager::GET_META_DATA, (IPackageInfo**)&info);
    if (FAILED(ec)) {
        Slogger::E(TAG, "handlePackageUpdate could not find package %s 0x%08x", packageName.string(), ec);
        return;
    }

    AutoPtr< ArrayOf<IActivityInfo*> > activities;
    info->GetActivities((ArrayOf< IActivityInfo* >**)&activities);
    if (activities == NULL) return;
    for (Int32 i = 0, size = activities->GetLength(); i < size; i++) {
        // check for meta-data, both for devices and accessories
        if (HandlePackageUpdateLocked(packageName, (*activities)[i], IUsbManager::ACTION_USB_DEVICE_ATTACHED)) {
            changed = TRUE;
        }

        if (HandlePackageUpdateLocked(packageName, (*activities)[i], IUsbManager::ACTION_USB_ACCESSORY_ATTACHED)) {
            changed = TRUE;
        }
    }

    if (changed) {
        WriteSettingsLocked();
    }
}

Boolean UsbSettingsManager::HasPermission(
    /* [in] */ IUsbDevice* device)
{
    AutoLock lock(mLock);
    String name;
    device->GetDeviceName(&name);
    AutoPtr<Int32BooleanMap> uidList;
    HashMap< String, AutoPtr<Int32BooleanMap> >::Iterator it = mDevicePermissionMap.Find(name);
    if (it != mDevicePermissionMap.End()) {
        uidList = it->mSecond;
    }
    if (uidList == NULL) {
        return FALSE;
    }
    Int32BooleanMapIterator resultIt = uidList->Find(Binder::GetCallingUid());
    Boolean result = FALSE;
    if (resultIt != uidList->End()) {
        result = resultIt->mSecond;
    }
    return result;
}

Boolean UsbSettingsManager::HasPermission(
    /* [in] */ IUsbAccessory* accessory)
{
    AutoLock lock(mLock);
    AutoPtr<Int32BooleanMap> uidList;
    HashMap< AutoPtr<IUsbAccessory>, AutoPtr<Int32BooleanMap> >::Iterator it = mAccessoryPermissionMap.Find(accessory);
    if (it != mAccessoryPermissionMap.End()) {
        uidList = it->mSecond;
    }
    if (uidList == NULL) {
        return FALSE;
    }
    Int32BooleanMapIterator resultIt = uidList->Find(Binder::GetCallingUid());
    Boolean result = FALSE;
    if (resultIt != uidList->End()) {
        result = resultIt->mSecond;
    }
    return result;
}

ECode UsbSettingsManager::CheckPermission(
    /* [in] */ IUsbDevice* device)
{
    if (!HasPermission(device)) {
        Slogger::E(TAG, "User has not given permission to device %p", device);
        return E_SECURITY_EXCEPTION;
        // throw new SecurityException("User has not given permission to device " + device);
    }
    return NOERROR;
}

ECode UsbSettingsManager::CheckPermission(
    /* [in] */ IUsbAccessory* accessory)
{
    if (!HasPermission(accessory)) {
        Slogger::E(TAG, "User has not given permission to accessory %p", accessory);
        return E_SECURITY_EXCEPTION;
        // throw new SecurityException("User has not given permission to accessory " + accessory);
    }
    return NOERROR;
}

ECode UsbSettingsManager::RequestPermissionDialog(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& packageName,
    /* [in] */ IPendingIntent* pi)
{
    Int32 uid = Binder::GetCallingUid();

    // compare uid with packageName to foil apps pretending to be someone else
    // try {
    AutoPtr<IApplicationInfo> appInfo;
    ECode ec = mPackageManager->GetApplicationInfo(packageName, 0, (IApplicationInfo**)&appInfo);
    if (FAILED(ec)) {
        Slogger::E(TAG, "package %s not found", packageName.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 appInfoUid;
    appInfo->GetUid(&appInfoUid);
    if (appInfoUid != uid) {
        Slogger::E(TAG, "package %s does not match caller's uid %d", packageName.string(), uid);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("package " + packageName +
        //         " does not match caller's uid " + uid);
    }
    // } catch (PackageManager.NameNotFoundException e) {
    //     throw new IllegalArgumentException("package " + packageName + " not found");
    // }

    Int64 identity = Binder::ClearCallingIdentity();
    intent->SetClassName(String("com.android.systemui"), String("com.android.systemui.usb.UsbPermissionActivity"));
    intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    intent->PutExtra(IIntent::EXTRA_INTENT, IParcelable::Probe(pi));
    intent->PutExtra(String("package"), packageName);
    intent->PutExtra(IIntent::EXTRA_UID, uid);
    // try {
    ec = mUserContext->StartActivityAsUser(intent, mUser);
    if (FAILED(ec)) {
        Slogger::E(TAG, "unable to start UsbPermissionActivity");
    }
    // } catch (ActivityNotFoundException e) {
    //     Slog.e(TAG, "unable to start UsbPermissionActivity");
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return NOERROR;
}

ECode UsbSettingsManager::RequestPermission(
    /* [in] */ IUsbDevice* device,
    /* [in] */ const String& packageName,
    /* [in] */ IPendingIntent* pi)
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);

    // respond immediately if permission has already been granted
    if (HasPermission(device)) {
        intent->PutExtra(IUsbManager::EXTRA_DEVICE, IParcelable::Probe(device));
        intent->PutBooleanExtra(IUsbManager::EXTRA_PERMISSION_GRANTED, TRUE);
        // try {
        ECode ec = pi->Send(mUserContext, 0, intent);
        if (FAILED(ec) && DEBUG == TRUE) {
            Slogger::D(TAG, "requestPermission PendingIntent was cancelled");
        }
        // } catch (PendingIntent.CanceledException e) {
        //     if (DEBUG) Slog.d(TAG, "requestPermission PendingIntent was cancelled");
        // }
        return NOERROR;
    }

    // start UsbPermissionActivity so user can choose an activity
    intent->PutExtra(IUsbManager::EXTRA_DEVICE, IParcelable::Probe(device));
    return RequestPermissionDialog(intent, packageName, pi);
}

ECode UsbSettingsManager::RequestPermission(
    /* [in] */ IUsbAccessory* accessory,
    /* [in] */ const String& packageName,
    /* [in] */ IPendingIntent* pi)
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);

    // respond immediately if permission has already been granted
    if (HasPermission(accessory)) {
        intent->PutExtra(IUsbManager::EXTRA_ACCESSORY, IParcelable::Probe(accessory));
        intent->PutBooleanExtra(IUsbManager::EXTRA_PERMISSION_GRANTED, TRUE);
        // try {
        ECode ec = pi->Send(mUserContext, 0, intent);
        if (FAILED(ec) && DEBUG == TRUE) {
            Slogger::D(TAG, "requestPermission PendingIntent was cancelled");
        }
        // } catch (PendingIntent.CanceledException e) {
        //     if (DEBUG) Slog.d(TAG, "requestPermission PendingIntent was cancelled");
        // }
        return NOERROR;
    }

    intent->PutExtra(IUsbManager::EXTRA_ACCESSORY, IParcelable::Probe(accessory));
    return RequestPermissionDialog(intent, packageName, pi);
}

void UsbSettingsManager::SetDevicePackage(
    /* [in] */ IUsbDevice* device,
    /* [in] */ const String& packageName)
{
    AutoPtr<DeviceFilter> filter = new DeviceFilter(device);
    Boolean changed = FALSE;

    AutoLock lock(mLock);
    if (packageName.IsNull()) {
        HashMap< AutoPtr<DeviceFilter>, String, HashPK_DeviceFilter, PKEq_DeviceFilter >::Iterator it
                = mDevicePreferenceMap.Find(filter);
        if (it != mDevicePreferenceMap.End()) {
            mDevicePreferenceMap.Erase(it);
            changed = TRUE;
        }
    }
    else {
        String name = mDevicePreferenceMap[filter];
        changed = !packageName.Equals(name);
        if (changed) {
            mDevicePreferenceMap[filter] = packageName;
        }
    }
    if (changed == TRUE) {
        WriteSettingsLocked();
    }
}

void UsbSettingsManager::SetAccessoryPackage(
    /* [in] */ IUsbAccessory* accessory,
    /* [in] */ const String& packageName)
{
    AutoPtr<AccessoryFilter> filter = new AccessoryFilter(accessory);
    Boolean changed = FALSE;

    AutoLock lock(mLock);
    if (packageName.IsNull()) {
        HashMap< AutoPtr<AccessoryFilter>, String, HashPK_AccessoryFilter, PKEq_AccessoryFilter >::Iterator it
                = mAccessoryPreferenceMap.Find(filter);
        if (it != mAccessoryPreferenceMap.End()) {
            mAccessoryPreferenceMap.Erase(it);
            changed = TRUE;
        }
    }
    else {
        String name = mAccessoryPreferenceMap[filter];
        changed = !packageName.Equals(name);
        if (changed) {
            mAccessoryPreferenceMap[filter] = packageName;
        }
    }
    if (changed == TRUE) {
        WriteSettingsLocked();
    }
}

void UsbSettingsManager::GrantDevicePermission(
    /* [in] */ IUsbDevice* device,
    /* [in] */ Int32 uid)
{
    AutoLock lock(mLock);

    String deviceName;
    device->GetDeviceName(&deviceName);
    AutoPtr<Int32BooleanMap> uidList = mDevicePermissionMap[deviceName];
    if (uidList == NULL) {
        uidList = new Int32BooleanMap(1);
        mDevicePermissionMap[deviceName] = uidList;
    }
    (*uidList)[uid] = TRUE;
}

void UsbSettingsManager::GrantAccessoryPermission(
    /* [in] */ IUsbAccessory* accessory,
    /* [in] */ Int32 uid)
{
    AutoLock lock(mLock);

    AutoPtr<Int32BooleanMap> uidList = mAccessoryPermissionMap[accessory];
    if (uidList == NULL) {
        uidList = new Int32BooleanMap(1);
        mAccessoryPermissionMap[accessory] = uidList;
    }
    (*uidList)[uid] = TRUE;
}

Boolean UsbSettingsManager::HasDefaults(
    /* [in] */ const String& packageName)
{
    AutoLock lock(mLock);
    HashMap< AutoPtr<DeviceFilter>, String, HashPK_DeviceFilter, PKEq_DeviceFilter >::Iterator it1
            = mDevicePreferenceMap.Begin();
    for (; it1 != mDevicePreferenceMap.End(); ++it1) {
        if (packageName == it1->mSecond) return TRUE;
    }
    HashMap< AutoPtr<AccessoryFilter>, String, HashPK_AccessoryFilter, PKEq_AccessoryFilter >::Iterator it2
            = mAccessoryPreferenceMap.Begin();
    for (; it2 != mAccessoryPreferenceMap.End(); ++it2) {
        if (packageName == it2->mSecond) return TRUE;
    }
    return FALSE;
}

void UsbSettingsManager::ClearDefaults(
    /* [in] */ const String& packageName)
{
    AutoLock lock(mLock);
    if (ClearPackageDefaultsLocked(packageName)) {
        WriteSettingsLocked();
    }
}

Boolean UsbSettingsManager::ClearPackageDefaultsLocked(
    /* [in] */ const String& packageName)
{
    Boolean cleared = FALSE;

    AutoLock lock(mLock);
    Boolean isContain = FALSE;
    HashMap< AutoPtr<DeviceFilter>, String, HashPK_DeviceFilter, PKEq_DeviceFilter >::Iterator it1
            = mDevicePreferenceMap.Begin();
    for (; it1 != mDevicePreferenceMap.End(); ++it1) {
        if (packageName == it1->mSecond) {
            isContain = TRUE;
            break;
        }
    }
    if (isContain) {
        // make a copy of the key set to avoid ConcurrentModificationException
        it1 = mDevicePreferenceMap.Begin();
        for (; it1 != mDevicePreferenceMap.End();) {
            if (packageName.Equals(it1->mSecond)) {
                mDevicePreferenceMap.Erase(it1++);
                cleared = TRUE;
            }
            else ++it1;
        }
    }
    isContain = FALSE;
    HashMap< AutoPtr<AccessoryFilter>, String, HashPK_AccessoryFilter, PKEq_AccessoryFilter >::Iterator it2
            = mAccessoryPreferenceMap.Begin();
    for (; it2 != mAccessoryPreferenceMap.End(); ++it2) {
        if (packageName == it2->mSecond) {
            isContain = TRUE;
            break;
        }
    }
    if (isContain) {
        // make a copy of the key set to avoid ConcurrentModificationException
        it2 = mAccessoryPreferenceMap.Begin();
        for (; it2 != mAccessoryPreferenceMap.End();) {
            if (packageName.Equals(it2->mSecond)) {
                mAccessoryPreferenceMap.Erase(it2++);
                cleared = TRUE;
            }
            else ++it2;
        }
    }
    return cleared;
}

} // namespace Usb
} // namespace Server
} // namepsace Droid
} // namespace Elastos
