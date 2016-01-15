#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Hardware.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.External.h>
#include "elastos/droid/media/tv/CTvInputInfo.h"
#include "elastos/droid/media/tv/TvInputInfoTvInputSettings.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/graphics/drawable/CDrawableHelper.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/utility/SparseInt32Array.h"
#include "elastos/droid/R.h"
#include "elastos/droid/utility/Xml.h"

using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Graphics::Drawable::IDrawableHelper;
using Elastos::Droid::Graphics::Drawable::CDrawableHelper;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::R;
using Elastos::Droid::Utility::SparseInt32Array;
using Elastos::Droid::Utility::Xml;
using Elastos::Core::CString;
using Elastos::IO::IInputStream;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

Boolean CTvInputInfo::DEBUG = FALSE;
String CTvInputInfo::TAG("TvInputInfo");

static AutoPtr<ISparseInt32Array> Init()
{
    AutoPtr<ISparseInt32Array> array;
    array = new SparseInt32Array();
    array->Put(ITvInputHardwareInfo::TV_INPUT_TYPE_OTHER_HARDWARE, ITvInputInfo::TYPE_OTHER);
    array->Put(ITvInputHardwareInfo::TV_INPUT_TYPE_TUNER, ITvInputInfo::TYPE_TUNER);
    array->Put(ITvInputHardwareInfo::TV_INPUT_TYPE_COMPOSITE, ITvInputInfo::TYPE_COMPOSITE);
    array->Put(ITvInputHardwareInfo::TV_INPUT_TYPE_SVIDEO, ITvInputInfo::TYPE_SVIDEO);
    array->Put(ITvInputHardwareInfo::TV_INPUT_TYPE_SCART, ITvInputInfo::TYPE_SCART);
    array->Put(ITvInputHardwareInfo::TV_INPUT_TYPE_COMPONENT, ITvInputInfo::TYPE_COMPONENT);
    array->Put(ITvInputHardwareInfo::TV_INPUT_TYPE_VGA, ITvInputInfo::TYPE_VGA);
    array->Put(ITvInputHardwareInfo::TV_INPUT_TYPE_DVI, ITvInputInfo::TYPE_DVI);
    array->Put(ITvInputHardwareInfo::TV_INPUT_TYPE_HDMI, ITvInputInfo::TYPE_HDMI);
    array->Put(ITvInputHardwareInfo::TV_INPUT_TYPE_DISPLAY_PORT, ITvInputInfo::TYPE_DISPLAY_PORT);
    return array;
}

AutoPtr<ISparseInt32Array> CTvInputInfo::sHardwareTypeToTvInputType = Init();

String CTvInputInfo::XML_START_TAG_NAME("tv-input");
String CTvInputInfo::DELIMITER_INFO_IN_ID("/");
String CTvInputInfo::PREFIX_HDMI_DEVICE("HDMI");
String CTvInputInfo::PREFIX_HARDWARE_DEVICE("HW");
Int32 CTvInputInfo::LENGTH_HDMI_PHYSICAL_ADDRESS = 4;
Int32 CTvInputInfo::LENGTH_HDMI_DEVICE_ID = 2;

CAR_INTERFACE_IMPL_2(CTvInputInfo, Object, ITvInputInfo, IParcelable)

CAR_OBJECT_IMPL(CTvInputInfo)

CTvInputInfo::CTvInputInfo()
    : mType(ITvInputInfo::TYPE_TUNER)
    , mIsConnectedToHdmiSwitch(FALSE)
{
}

CTvInputInfo::~CTvInputInfo()
{
}

ECode CTvInputInfo::constructor(
    /* [in] */ IResolveInfo* service,
    /* [in] */ const String& id,
    /* [in] */ const String& parentId,
    /* [in] */ Int32 type)
{
    mService = service;
    mId = id;
    mParentId = parentId;
    mType = type;
    return NOERROR;
}

ECode CTvInputInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mId);
    source->ReadString(&mParentId);
    source->ReadString(&mSetupActivity);
    source->ReadString(&mSettingsActivity);
    source->ReadString(&mLabel);
    source->ReadInt32(&mType);
    source->ReadBoolean(&mIsConnectedToHdmiSwitch);
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)&obj);
    mService = IResolveInfo::Probe(obj);
    obj = NULL;
    source->ReadInterfacePtr((Handle32*)&obj);
    mHdmiDeviceInfo = IHdmiDeviceInfo::Probe(obj);
    obj = NULL;
    source->ReadInterfacePtr((Handle32*)&obj);
    mIconUri = IUri::Probe(obj);
    return NOERROR;
}

ECode CTvInputInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mId);
    dest->WriteString(mParentId);
    dest->WriteString(mSetupActivity);
    dest->WriteString(mSettingsActivity);
    dest->WriteString(mLabel);
    dest->WriteInt32(mType);
    dest->WriteBoolean(mIsConnectedToHdmiSwitch);
    dest->WriteInterfacePtr(mService);
    dest->WriteInterfacePtr(mHdmiDeviceInfo);
    dest->WriteInterfacePtr(mIconUri);

    return NOERROR;
}

ECode CTvInputInfo::GetId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mId;
    return NOERROR;
}

ECode CTvInputInfo::GetParentId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mParentId;
    return NOERROR;
}

ECode CTvInputInfo::GetServiceInfo(
    /* [out] */ IServiceInfo** result)
{
    VALIDATE_NOT_NULL(result)
    mService->GetServiceInfo(result);
    return NOERROR;
}

ECode CTvInputInfo::GetComponent(
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IServiceInfo> si;
    mService->GetServiceInfo((IServiceInfo**)&si);
    String pkgName, name;
    IPackageItemInfo::Probe(si)->GetPackageName(&pkgName);
    IPackageItemInfo::Probe(si)->GetName(&name);
    return CComponentName::New(pkgName, name, result);
}

ECode CTvInputInfo::CreateSetupIntent(
    /* [out] */ IIntent** result)
{
    VALIDATE_NOT_NULL(result)
    if (!mSetupActivity.IsEmpty()) {
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
        AutoPtr<IServiceInfo> si;
        mService->GetServiceInfo((IServiceInfo**)&si);
        String pkgName;
        IPackageItemInfo::Probe(si)->GetPackageName(&pkgName);
        intent->SetClassName(pkgName, mSetupActivity);
        String id;
        GetId(&id);
        intent->PutExtra(ITvInputInfo::EXTRA_INPUT_ID, id);
        *result = intent;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    *result = NULL;
    return NOERROR;
}

ECode CTvInputInfo::CreateSettingsIntent(
    /* [out] */ IIntent** result)
{
    VALIDATE_NOT_NULL(result)
    if (!mSettingsActivity.IsEmpty()) {
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
        AutoPtr<IServiceInfo> si;
        mService->GetServiceInfo((IServiceInfo**)&si);
        String pkgName;
        IPackageItemInfo::Probe(si)->GetPackageName(&pkgName);
        intent->SetClassName(pkgName, mSettingsActivity);
        String id;
        GetId(&id);
        intent->PutExtra(ITvInputInfo::EXTRA_INPUT_ID, id);
        *result = intent;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    *result = NULL;
    return NOERROR;
}

ECode CTvInputInfo::GetType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mType;
    return NOERROR;
}

ECode CTvInputInfo::GetHdmiDeviceInfo(
    /* [out] */ IHdmiDeviceInfo** result)
{
    VALIDATE_NOT_NULL(result)
    if (mType == ITvInputInfo::TYPE_HDMI) {
        *result = mHdmiDeviceInfo;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    *result = NULL;
    return NOERROR;
}

ECode CTvInputInfo::IsPassthroughInput(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mType != ITvInputInfo::TYPE_TUNER);
    return NOERROR;
}

ECode CTvInputInfo::IsConnectedToHdmiSwitch(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsConnectedToHdmiSwitch;
    return NOERROR;
}

ECode CTvInputInfo::IsHidden(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 myUserId = CUserHandle::GetMyUserId();
    *result = TvInputInfoTvInputSettings::IsHidden(context, mId, myUserId);
    return NOERROR;
}

ECode CTvInputInfo::LoadLabel(
    /* [in] */ IContext* context,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    if (mLabel.IsEmpty()) {
        AutoPtr<IPackageManager> pm;
        context->GetPackageManager((IPackageManager**)&pm);
        return mService->LoadLabel(pm, result);
    } else {
        return CString::New(mLabel, result);
    }
}

ECode CTvInputInfo::LoadCustomLabel(
    /* [in] */ IContext* context,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 myUserId = CUserHandle::GetMyUserId();
    String str = TvInputInfoTvInputSettings::GetCustomLabel(context, mId, myUserId);
    return CString::New(str, result);
}

ECode CTvInputInfo::LoadIcon(
    /* [in] */ IContext* context,
    /* [out] */ IDrawable** result)
{
    VALIDATE_NOT_NULL(result)
    if (mIconUri == NULL) {
        AutoPtr<IDrawable> d = LoadServiceIcon(context);
        *result = d;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    // try (InputStream is = context.getContentResolver().openInputStream(mIconUri)) {
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IInputStream> is;
    cr->OpenInputStream(mIconUri, (IInputStream**)&is);

    AutoPtr<IDrawableHelper> helper;
    CDrawableHelper::AcquireSingleton((IDrawableHelper**)&helper);
    AutoPtr<IDrawable> drawable;
    helper->CreateFromStream(is, String(NULL), (IDrawable**)&drawable);
    if (drawable == NULL) {
        AutoPtr<IDrawable> d = LoadServiceIcon(context);
        *result = d;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    *result = drawable;
    REFCOUNT_ADD(*result)
    return NOERROR;
    // } catch (IOException e) {
    //     Logger::W(TAG, "Loading the default icon due to a failure on loading " + mIconUri, e);
    //     *result = LoadServiceIcon(context);
    //     REFCOUNT_ADD(*result)
    //     return NOERROR;
    // }
    // return NOERROR;
}

ECode CTvInputInfo::CreateTvInputInfo(
    /* [in] */ IContext* context,
    /* [in] */ IResolveInfo* service,
    /* [out] */ ITvInputInfo** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IServiceInfo> si;
    service->GetServiceInfo((IServiceInfo**)&si);
    String pkgName, name;
    IPackageItemInfo::Probe(si)->GetPackageName(&pkgName);
    IPackageItemInfo::Probe(si)->GetName(&name);

    AutoPtr<IComponentName> cn;
    CComponentName::New(pkgName, name, (IComponentName**)&cn);
    return CreateTvInputInfo(context, service,
            GenerateInputIdForComponentName(cn),
            String(NULL), TYPE_TUNER, String(NULL), NULL, FALSE, result);
}

ECode CTvInputInfo::CreateTvInputInfo(
    /* [in] */ IContext* context,
    /* [in] */ IResolveInfo* service,
    /* [in] */ IHdmiDeviceInfo* hdmiDeviceInfo,
    /* [in] */ const String& parentId,
    /* [in] */ const String& label,
    /* [in] */ IUri* iconUri,
    /* [out] */ ITvInputInfo** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 addr;
//TODO: Need IHdmiDeviceInfo
    // hdmiDeviceInfo->GetPhysicalAddress(&addr);
    Boolean isConnectedToHdmiSwitch = (addr & 0x0FFF) != 0;

    AutoPtr<IServiceInfo> si;
    service->GetServiceInfo((IServiceInfo**)&si);
    String pkgName, name;
    IPackageItemInfo::Probe(si)->GetPackageName(&pkgName);
    IPackageItemInfo::Probe(si)->GetName(&name);

    AutoPtr<IComponentName> cn;
    CComponentName::New(pkgName, name, (IComponentName**)&cn);

    AutoPtr<ITvInputInfo> input;
    CreateTvInputInfo(context, service, GenerateInputIdForHdmiDevice(cn, hdmiDeviceInfo),
            parentId, TYPE_HDMI, label, iconUri, isConnectedToHdmiSwitch, (ITvInputInfo**)&input);
    ((CTvInputInfo*)input.Get())->mHdmiDeviceInfo = hdmiDeviceInfo;
    *result = input;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTvInputInfo::CreateTvInputInfo(
    /* [in] */ IContext* context,
    /* [in] */ IResolveInfo* service,
    /* [in] */ ITvInputHardwareInfo* hardwareInfo,
    /* [in] */ const String& label,
    /* [in] */ IUri* iconUri,
    /* [out] */ ITvInputInfo** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 type;
    hardwareInfo->GetType(&type);
    Int32 inputType;
    sHardwareTypeToTvInputType->Get(type, ITvInputInfo::TYPE_TUNER, &inputType);

    AutoPtr<IServiceInfo> si;
    service->GetServiceInfo((IServiceInfo**)&si);
    String pkgName, name;
    IPackageItemInfo::Probe(si)->GetPackageName(&pkgName);
    IPackageItemInfo::Probe(si)->GetName(&name);

    AutoPtr<IComponentName> cn;
    CComponentName::New(pkgName, name, (IComponentName**)&cn);

    return CreateTvInputInfo(context, service, GenerateInputIdForHardware(cn, hardwareInfo),
            String(NULL), inputType, label, iconUri, FALSE, result);
}

ECode CTvInputInfo::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mId.GetHashCode();
    return NOERROR;
}

ECode CTvInputInfo::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (THIS_PROBE(IInterface) == IInterface::Probe(o)) {
        *result = TRUE;
        return NOERROR;
    }

    if (ITvInputInfo::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<ITvInputInfo> obj = ITvInputInfo::Probe(o);
    *result = mId.Equals(((CTvInputInfo*)obj.Get())->mId);
    return NOERROR;
}

ECode CTvInputInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IServiceInfo> si;
    mService->GetServiceInfo((IServiceInfo**)&si);
    String pkgName, name;
    IPackageItemInfo::Probe(si)->GetPackageName(&pkgName);
    IPackageItemInfo::Probe(si)->GetName(&name);

    StringBuilder sb("TvInputInfo{id=");
    sb += mId;
    sb += ", pkg=";
    sb += pkgName;
    sb += ", service=";
    sb += name;
    sb += "}";
    sb.ToString(result);
    return NOERROR;
}

AutoPtr<IDrawable> CTvInputInfo::LoadServiceIcon(
    /* [in] */ IContext* context)
{
    AutoPtr<IServiceInfo> si;
    mService->GetServiceInfo((IServiceInfo**)&si);
    // String pkgName, name;
    Int32 icon;
    IPackageItemInfo::Probe(si)->GetIcon(&icon);
    AutoPtr<IApplicationInfo> ai;
    IComponentInfo::Probe(si)->GetApplicationInfo((IApplicationInfo**)&ai);
    Int32 aiIcon;
    IPackageItemInfo::Probe(ai)->GetIcon(&aiIcon);

    if (icon == 0 && aiIcon == 0) {
        return NULL;
    }
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IDrawable> drawable;
    IPackageItemInfo::Probe(si)->LoadIcon(pm, (IDrawable**)&drawable);
    return drawable;
}

ECode CTvInputInfo::CreateTvInputInfo(
    /* [in] */ IContext* context,
    /* [in] */ IResolveInfo* service,
    /* [in] */ const String& id,
    /* [in] */ const String& parentId,
    /* [in] */ Int32 inputType,
    /* [in] */ const String& label,
    /* [in] */ IUri* iconUri,
    /* [in] */ Boolean isConnectedToHdmiSwitch,
    /* [out] */ ITvInputInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IServiceInfo> si;
    service->GetServiceInfo((IServiceInfo**)&si);
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IXmlResourceParser> parser;
    // try {
    IPackageItemInfo::Probe(si)->LoadXmlMetaData(pm, ITvInputService::SERVICE_META_DATA, (IXmlResourceParser**)&parser);
    if (parser == NULL) {
        // throw new XmlPullParserException("No " + TvInputService.SERVICE_META_DATA
        //         + " meta-data for " + si.name);
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    AutoPtr<IApplicationInfo> ai;
    IComponentInfo::Probe(si)->GetApplicationInfo((IApplicationInfo**)&ai);
    AutoPtr<IResources> res;
    pm->GetResourcesForApplication(ai, (IResources**)&res);
    AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(IXmlPullParser::Probe(parser));

    Int32 type;
    while ((IXmlPullParser::Probe(parser)->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && type != IXmlPullParser::START_TAG) {
    }

    String nodeName;
    IXmlPullParser::Probe(parser)->GetName(&nodeName);
    if (!XML_START_TAG_NAME.Equals(nodeName)) {
        // throw new XmlPullParserException(
        //         "Meta-data does not start with tv-input-service tag in " + si.name);
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    AutoPtr<CTvInputInfo> input;
    CTvInputInfo::NewByFriend(service, id, parentId, inputType, (CTvInputInfo**)&input);
    AutoPtr<ITypedArray> sa;
    assert(0);
//TODO: Need R::styleable::TvInputService
    // res->ObtainAttributes(attrs, R::styleable::TvInputService, (ITypedArray**)&sa);
    sa->GetString(R::styleable::TvInputService_setupActivity, &input->mSetupActivity);
    if (DEBUG) {
        // Logger::D(TAG, "Setup activity loaded. [" + input.mSetupActivity + "] for " + si.name);
    }
    sa->GetString(R::styleable::TvInputService_settingsActivity, &input->mSettingsActivity);
    if (DEBUG) {
        // Logger::D(TAG, "Settings activity loaded. [" + input.mSettingsActivity + "] for " + si.name);
    }
    sa->Recycle();

    input->mLabel = label;
    input->mIconUri = iconUri;
    input->mIsConnectedToHdmiSwitch = isConnectedToHdmiSwitch;
    *result = ITvInputInfo::Probe(input);
    REFCOUNT_ADD(*result)
    // } catch (NameNotFoundException e) {
    //     throw new XmlPullParserException("Unable to create context for: " + si.packageName);
    // } finally {
    if (parser != NULL) {
        parser->Close();
    }
    // }
    return NOERROR;
}

String CTvInputInfo::GenerateInputIdForComponentName(
    /* [in] */ IComponentName* name)
{
    String str;
    name->FlattenToShortString(&str);
    return str;
}

String CTvInputInfo::GenerateInputIdForHdmiDevice(
    /* [in] */ IComponentName* name,
    /* [in] */ IHdmiDeviceInfo* deviceInfo)
{
    // Example of the format : "/HDMI%04X%02X"
    // String format = String.format("%s%s%%0%sX%%0%sX", DELIMITER_INFO_IN_ID, PREFIX_HDMI_DEVICE,
    //         LENGTH_HDMI_PHYSICAL_ADDRESS, LENGTH_HDMI_DEVICE_ID);
    // return name.flattenToShortString() + String.format(format,
    //         deviceInfo.getPhysicalAddress(), deviceInfo.getId());
    String str;
    name->FlattenToShortString(&str);
    Int32 addr, id;
//TODO: Need IHdmiDeviceInfo
    // deviceInfo->GetId(&id);
    // deviceInfo->GetPhysicalAddress(&addr);
    return str + "/HDMI" + addr + id;
}

String CTvInputInfo::GenerateInputIdForHardware(
    /* [in] */ IComponentName* name,
    /* [in] */ ITvInputHardwareInfo* hardwareInfo)
{
    // return name.flattenToShortString() + String.format("%s%s%d",
    //         DELIMITER_INFO_IN_ID, PREFIX_HARDWARE_DEVICE, hardwareInfo.getDeviceId());
    String str;
    name->FlattenToShortString(&str);
    Int32 deviceId;
    hardwareInfo->GetDeviceId(&deviceId);
    return str + "/HW" + deviceId;
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
