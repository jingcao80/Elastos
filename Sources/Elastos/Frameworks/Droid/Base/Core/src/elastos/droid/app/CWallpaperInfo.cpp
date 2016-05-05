
#include "elastos/droid/ext/frameworkdef.h"
#include <Elastos.CoreLibrary.External.h>
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Service.h"
#include "elastos/droid/app/CWallpaperInfo.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/pm/CResolveInfo.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::CResolveInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Service::Wallpaper::IWallpaperService;

namespace Elastos {
namespace Droid {
namespace App {

const String CWallpaperInfo::TAG("WallpaperInfo");

CAR_INTERFACE_IMPL_2(CWallpaperInfo, Object, IWallpaperInfo, IParcelable)

CAR_OBJECT_IMPL(CWallpaperInfo)

CWallpaperInfo::CWallpaperInfo()
    : mThumbnailResource(0)
    , mAuthorResource(0)
    , mDescriptionResource(0)
{}

ECode CWallpaperInfo::GetPackageName(
    /* [out] */ String* pkgName)
{
    VALIDATE_NOT_NULL(pkgName);
    AutoPtr<IServiceInfo> serviceInfo;
    FAIL_RETURN(mService->GetServiceInfo((IServiceInfo**)&serviceInfo));
    return IPackageItemInfo::Probe(serviceInfo)->GetPackageName(pkgName);
}

ECode CWallpaperInfo::GetServiceName(
    /* [out] */ String* sName)
{
    VALIDATE_NOT_NULL(sName);
    AutoPtr<IServiceInfo> serviceInfo;
    FAIL_RETURN(mService->GetServiceInfo((IServiceInfo**)&serviceInfo));
    return IPackageItemInfo::Probe(serviceInfo)->GetName(sName);
}

ECode CWallpaperInfo::GetServiceInfo(
    /* [out] */ IServiceInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return mService->GetServiceInfo(info);
}

ECode CWallpaperInfo::GetComponent(
    /* [out] */ IComponentName** component)
{
    AutoPtr<IServiceInfo> serviceInfo;
    FAIL_RETURN(mService->GetServiceInfo((IServiceInfo**)&serviceInfo));
    String packageName, name;
    IPackageItemInfo::Probe(serviceInfo)->GetPackageName(&packageName);
    IPackageItemInfo::Probe(serviceInfo)->GetName(&name);
    return CComponentName::New(packageName, name, component);
}

ECode CWallpaperInfo::LoadLabel(
    /* [in] */ IPackageManager* pm,
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label);
    return mService->LoadLabel(pm, label);
}

ECode CWallpaperInfo::LoadIcon(
    /* [in] */ IPackageManager* pm,
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon);
    return mService->LoadIcon(pm, icon);
}

ECode CWallpaperInfo::LoadThumbnail(
    /* [in] */ IPackageManager* pm,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);

    if (mThumbnailResource < 0) {
        *drawable = NULL;
        return NOERROR;
    }

    AutoPtr<IServiceInfo> serviceInfo;
    FAIL_RETURN(mService->GetServiceInfo((IServiceInfo**)&serviceInfo));
    String packageName;
    IPackageItemInfo::Probe(serviceInfo)->GetPackageName(&packageName);
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(serviceInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    return pm->GetDrawable(packageName, mThumbnailResource, appInfo, drawable);
}

ECode CWallpaperInfo::LoadAuthor(
    /* [in] */ IPackageManager* pm,
    /* [out] */ ICharSequence** author)
{
    VALIDATE_NOT_NULL(author);
    if (mAuthorResource <= 0) {
        *author = NULL;
        return E_NOT_FOUND_EXCEPTION;
    }
    String packageName;
    mService->GetResolvePackageName(&packageName);
    AutoPtr<IApplicationInfo> applicationInfo;
    if (packageName.IsNull()) {
        AutoPtr<IServiceInfo> serviceInfo;
        FAIL_RETURN(mService->GetServiceInfo((IServiceInfo**)&serviceInfo));
        IPackageItemInfo::Probe(serviceInfo)->GetPackageName(&packageName);
        IComponentInfo::Probe(serviceInfo)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    }
    return pm->GetText(packageName, mAuthorResource, applicationInfo, author);
}

ECode CWallpaperInfo::LoadDescription(
    /* [in] */ IPackageManager* pm,
    /* [out] */ ICharSequence** summary)
{
    VALIDATE_NOT_NULL(summary);
    String packageName;
    mService->GetResolvePackageName(&packageName);
    AutoPtr<IApplicationInfo> applicationInfo;
    if (packageName.IsNull()) {
        AutoPtr<IServiceInfo> serviceInfo;
        FAIL_RETURN(mService->GetServiceInfo((IServiceInfo**)&serviceInfo));
        IPackageItemInfo::Probe(serviceInfo)->GetPackageName(&packageName);
        IComponentInfo::Probe(serviceInfo)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    }
    AutoPtr<IServiceInfo> serviceInfo;
    FAIL_RETURN(mService->GetServiceInfo((IServiceInfo**)&serviceInfo));
    Int32 desc;
    IComponentInfo::Probe(serviceInfo)->GetDescriptionRes(&desc);
    if (desc != 0) {
        return pm->GetText(packageName, desc, applicationInfo, summary);

    }
    if (mDescriptionResource <= 0) {
        *summary = NULL;
        return E_NOT_FOUND_EXCEPTION;
    }

    applicationInfo = NULL;
    IComponentInfo::Probe(serviceInfo)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    return pm->GetText(packageName, mDescriptionResource, applicationInfo, summary);
}

ECode CWallpaperInfo::GetSettingsActivity(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mSettingsActivityName;
    return NOERROR;
}

ECode CWallpaperInfo::Dump(
    /* [in] */ IPrinter *pw,
    /* [in] */ const String &prefix)
{
    pw->Println(prefix + String("Service:"));
    mService->Dump(pw, prefix + String("  "));
    pw->Println(prefix + String("mSettingsActivityName=") + mSettingsActivityName);
    return NOERROR;
}

ECode CWallpaperInfo::SetService(
    /* [in] */ IResolveInfo* service)
{
    mService = service;
    return NOERROR;
}

ECode CWallpaperInfo::GetService(
    /* [out] */ IResolveInfo** service)
{
    VALIDATE_NOT_NULL(service);
    *service = mService;
    REFCOUNT_ADD(*service);
    return NOERROR;
}

ECode CWallpaperInfo::SetSettingsActivityName(
    /* [in] */ const String& name)
{
    mSettingsActivityName = name;
    return NOERROR;
}

ECode CWallpaperInfo::GetSettingsActivityName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mSettingsActivityName;
    return NOERROR;
}

ECode CWallpaperInfo::SetThumbnailResource(
    /* [in] */ Int32 thumbnail)
{
    mThumbnailResource = thumbnail;
    return NOERROR;
}

ECode CWallpaperInfo::GetThumbnailResource(
    /* [out] */ Int32* thumbnail)
{
    VALIDATE_NOT_NULL(thumbnail);
    *thumbnail = mThumbnailResource;
    return NOERROR;
}

ECode CWallpaperInfo::SetAuthorResource(
    /* [in] */ Int32 author)
{
    mAuthorResource = author;
    return NOERROR;
}

ECode CWallpaperInfo::GetAuthorResource(
    /* [out] */ Int32* author)
{
    VALIDATE_NOT_NULL(author);
    *author = mAuthorResource;
    return NOERROR;
}

ECode CWallpaperInfo::SetDescriptionResource(
    /* [in] */ Int32 description)
{
    mDescriptionResource = description;
    return NOERROR;
}

ECode CWallpaperInfo::GetDescriptionResource(
    /* [out] */ Int32* description)
{
    VALIDATE_NOT_NULL(description);
    *description = mDescriptionResource;
    return NOERROR;
}

ECode CWallpaperInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mSettingsActivityName);
    dest->WriteInt32(mThumbnailResource);
    dest->WriteInt32(mAuthorResource);
    dest->WriteInt32(mDescriptionResource);
    IParcelable* p = IParcelable::Probe(mService);
    if (p != NULL) {
        dest->WriteInt32(1);
        p->WriteToParcel(dest);
    }
    else {
        dest->WriteInt32(0);
    }
    return NOERROR;
}

ECode CWallpaperInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mSettingsActivityName);
    source->ReadInt32(&mThumbnailResource);
    source->ReadInt32(&mAuthorResource);
    source->ReadInt32(&mDescriptionResource);
    Int32 ival;
    source->ReadInt32(&ival);
    if (ival == 1) {
        assert(mService == NULL);
        CResolveInfo::New((IResolveInfo**)&mService);
        IParcelable::Probe(mService)->ReadFromParcel(source);
    }
    return NOERROR;
}

ECode CWallpaperInfo::constructor()
{
    return NOERROR;
}

ECode CWallpaperInfo::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IResolveInfo* service)
{
    mService = service;
    AutoPtr<IServiceInfo> si;
    FAIL_RETURN(service->GetServiceInfo((IServiceInfo**)&si));

    AutoPtr<IPackageManager> pm;
    FAIL_RETURN(context->GetPackageManager((IPackageManager**)&pm));
    String settingsActivityComponent;
    Int32 thumbnailRes = -1;
    Int32 authorRes = -1;
    Int32 descriptionRes = -1;

    AutoPtr<IXmlResourceParser> parser;
    IPackageItemInfo* pi = IPackageItemInfo::Probe(si);
    // try {
    ECode ec = pi->LoadXmlMetaData(pm, IWallpaperService::SERVICE_META_DATA,
            (IXmlResourceParser**)&parser);
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION || parser == NULL) {
        String packageName;
        pi->GetPackageName(&packageName);
        Slogger::E(TAG, "Unable to create context for: %s", (const char*)packageName);
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(si)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    AutoPtr<IResources> res;
    ASSERT_SUCCEEDED(pm->GetResourcesForApplication(appInfo, (IResources**)&res));

    IXmlPullParser* p = IXmlPullParser::Probe(parser);
    AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(p);

    Int32 type;
    while (p->Next(&type), type != IXmlPullParser::END_DOCUMENT
            && type != IXmlPullParser::START_TAG) {
    }

    String nodeName;
    p->GetName(&nodeName);
    if (!String("wallpaper").Equals(nodeName)) {
        Slogger::E(TAG, "Meta-data does not start with wallpaper tag");
        parser->Close();
        return E_XML_PULL_PARSER_EXCEPTION;
        // throw new XmlPullParserException(
        //         "Meta-data does not start with wallpaper tag");
    }

    AutoPtr<ITypedArray> sa;
    Int32 size = ArraySize(R::styleable::Wallpaper);
    AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
    layout->Copy(R::styleable::Wallpaper, size);
    ASSERT_SUCCEEDED(res->ObtainAttributes(attrs, layout, (ITypedArray**)&sa));
    sa->GetString(
            R::styleable::Wallpaper_settingsActivity, &settingsActivityComponent);

    sa->GetResourceId(R::styleable::Wallpaper_thumbnail, -1, &thumbnailRes);
    sa->GetResourceId(R::styleable::Wallpaper_author, -1, &authorRes);
    sa->GetResourceId(R::styleable::Wallpaper_description, -1, &descriptionRes);

    sa->Recycle();

    if (parser != NULL) parser->Close();
    // } catch (NameNotFoundException e) {
    //     throw new XmlPullParserException(
    //             "Unable to create context for: " + si.packageName);
    // } finally {
    //     if (parser != null) parser.close();
    // }

    mSettingsActivityName = settingsActivityComponent;
    mThumbnailResource = thumbnailRes;
    mAuthorResource = authorRes;
    mDescriptionResource = descriptionRes;
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
