//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.External.h"
#include "elastos/droid/settings/DreamBackend.h"
#include "elastos/droid/settings/CDreamSettings.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Service::Dreams::IDreamService;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::R;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::EIID_IComparator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::Logging::Logger;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  DreamBackend::DreamInfo
//===============================================================================

DreamBackend::DreamInfo::DreamInfo()
    : mIsActive(FALSE)
{}

ECode DreamBackend::DreamInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = NULL;

    AutoPtr<StringBuilder> sb = new StringBuilder("DreamInfo");
    sb->AppendChar('[');
    sb->Append(mCaption);
    if (mIsActive) {
        sb->Append(",active");
    }
    sb->AppendChar(',');
    sb->Append(mComponentName);
    if (mSettingsComponentName != NULL) {
        sb->Append("settings=");
        sb->Append(mSettingsComponentName);
    }
    sb->AppendChar(']');

    *str = sb->ToString();
    return NOERROR;
}

//===============================================================================
//                  DreamBackend::DreamInfoComparator
//===============================================================================

CAR_INTERFACE_IMPL(DreamBackend::DreamInfoComparator, Object, IComparator)

DreamBackend::DreamInfoComparator::DreamInfoComparator(
    /* [in] */ IComponentName* defaultDream)
{
    mDefaultDream = defaultDream;
}

ECode DreamBackend::DreamInfoComparator::Compare(
    /* [in] */ IInterface* _lhs,
    /* [in] */ IInterface* _rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<DreamInfo> lhs = (DreamInfo*)IObject::Probe(_lhs);
    AutoPtr<DreamInfo> rhs = (DreamInfo*)IObject::Probe(_rhs);

    *result = SortKey(lhs).Compare(SortKey(rhs));
    return NOERROR;
}

String DreamBackend::DreamInfoComparator::SortKey(
    /* [in] */ DreamInfo* di)
{
    AutoPtr<StringBuilder> sb = new StringBuilder();
    sb->AppendChar(Object::Equals(di->mComponentName, mDefaultDream) ? '0' : '1');
    sb->Append(di->mCaption);
    return sb->ToString();
}

//===============================================================================
//                  DreamBackend
//===============================================================================

const String DreamBackend::TAG("CDreamSettings.Backend");

DreamBackend::DreamBackend(
    /* [in] */ IContext* context)
{
    mContext = context;
    mDreamManager = IIDreamManager::Probe(ServiceManager::GetService(IDreamService::DREAM_SERVICE));
    mComparator = new DreamInfoComparator(GetDefaultDream());
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    resources->GetBoolean(
            Elastos::Droid::R::bool_::config_dreamsEnabledByDefault, &mDreamsEnabledByDefault);
    resources->GetBoolean(
            Elastos::Droid::R::bool_::config_dreamsActivatedOnSleepByDefault, &mDreamsActivatedOnSleepByDefault);
    resources->GetBoolean(
            Elastos::Droid::R::bool_::config_dreamsActivatedOnDockByDefault, &mDreamsActivatedOnDockByDefault);
}

AutoPtr<IList> DreamBackend::GetDreamInfos()
{
    Logd(String("GetDreamInfos()"), NULL);
    AutoPtr<IComponentName> activeDream = GetActiveDream();
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IIntent> dreamIntent;
    CIntent::New(IDreamService::SERVICE_INTERFACE, (IIntent**)&dreamIntent);
    AutoPtr<IList> resolveInfos; //List<ResolveInfo>
    pm->QueryIntentServices(dreamIntent,
            IPackageManager::GET_META_DATA, (IList**)&resolveInfos);
    Int32 size;
    resolveInfos->GetSize(&size);
    AutoPtr<IList> dreamInfos; //List<DreamInfo>
    CArrayList::New(size, (IList**)&dreamInfos);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        resolveInfos->Get(i, (IInterface**)&obj);
        IResolveInfo* resolveInfo = IResolveInfo::Probe(obj);

        AutoPtr<IServiceInfo> serviceInfo;
        resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
        if (serviceInfo == NULL)
            continue;
        AutoPtr<DreamInfo> dreamInfo = new DreamInfo();
        resolveInfo->LoadLabel(pm, (ICharSequence**)&dreamInfo->mCaption);
        resolveInfo->LoadIcon(pm, (IDrawable**)&dreamInfo->mIcon);
        dreamInfo->mComponentName = GetDreamComponentName(resolveInfo);
        dreamInfo->mIsActive = Object::Equals(dreamInfo->mComponentName, activeDream);
        dreamInfo->mSettingsComponentName = GetSettingsComponentName(pm, resolveInfo);
        dreamInfos->Add((IObject*)dreamInfo);
    }
    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    coll->Sort(dreamInfos, mComparator);
    return dreamInfos;
}

AutoPtr<IComponentName> DreamBackend::GetDefaultDream()
{
    if (mDreamManager == NULL) {
        return NULL;
    }
    AutoPtr<IComponentName> comp;
    ECode ec = mDreamManager->GetDefaultDreamComponent((IComponentName**)&comp);
    if (SUCCEEDED(ec)) {
        return comp;
    }
    else if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::W(TAG, "Failed to get default dream 0x%08x", ec);
        return NULL;
    }
    return NULL;
}

AutoPtr<ICharSequence> DreamBackend::GetActiveDreamName()
{
    AutoPtr<IComponentName> cn = GetActiveDream();
    if (cn != NULL) {
        AutoPtr<IPackageManager> pm;
        mContext->GetPackageManager((IPackageManager**)&pm);
        // try {
        AutoPtr<IServiceInfo> ri;
        ECode ec = pm->GetServiceInfo(cn, 0, (IServiceInfo**)&ri);
        if (SUCCEEDED(ec) && ri != NULL) {
            AutoPtr<ICharSequence> cs;
            IPackageItemInfo::Probe(ri)->LoadLabel(pm, (ICharSequence**)&cs);
            return cs;
        }
        else { //PackageManager.NameNotFoundException
            return NULL; // uninstalled?
        }
    }
    return NULL;
}

Boolean DreamBackend::IsEnabled()
{
    return GetBoolean(ISettingsSecure::SCREENSAVER_ENABLED, mDreamsEnabledByDefault);
}

ECode DreamBackend::SetEnabled(
    /* [in] */ Boolean value)
{
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, CoreUtils::Convert(value));

    Logd(String("SetEnabled(%s)"), args);
    SetBoolean(ISettingsSecure::SCREENSAVER_ENABLED, value);
    return NOERROR;
}

Boolean DreamBackend::IsActivatedOnDock()
{
    return GetBoolean(ISettingsSecure::SCREENSAVER_ACTIVATE_ON_DOCK, mDreamsActivatedOnDockByDefault);
}

ECode DreamBackend::SetActivatedOnDock(
    /* [in] */ Boolean value)
{
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, CoreUtils::Convert(value));

    Logd(String("SetActivatedOnDock(%s)"), args);
    SetBoolean(ISettingsSecure::SCREENSAVER_ACTIVATE_ON_DOCK, value);
    return NOERROR;
}

Boolean DreamBackend::IsActivatedOnSleep()
{
    return GetBoolean(ISettingsSecure::SCREENSAVER_ACTIVATE_ON_SLEEP, mDreamsActivatedOnSleepByDefault);
}

ECode DreamBackend::SetActivatedOnSleep(
    /* [in] */ Boolean value)
{
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, CoreUtils::Convert(value));

    Logd(String("SetActivatedOnSleep(%s)"), args);
    SetBoolean(ISettingsSecure::SCREENSAVER_ACTIVATE_ON_SLEEP, value);
    return NOERROR;
}

Boolean DreamBackend::GetBoolean(
    /* [in] */ const String& key,
    /* [in] */ Boolean def)
{
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 data;
    secure->GetInt32(resolver, key, def ? 1 : 0, &data);
    return data == 1;
}

void DreamBackend::SetBoolean(
    /* [in] */ const String& key,
    /* [in] */ Boolean value)
{
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Boolean res;
    secure->PutInt32(resolver, key, value ? 1 : 0, &res);
}

ECode DreamBackend::SetActiveDream(
    /* [in] */ IComponentName* dream)
{
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, dream);

    Logd(String("SetActiveDream(%s)"), args);
    if (mDreamManager == NULL) {
        return NOERROR;
    }
    AutoPtr< ArrayOf<IComponentName*> > dreams = ArrayOf<IComponentName*>::Alloc(1);
    dreams->Set(0, dream);

    ECode ec = mDreamManager->SetDreamComponents(dream == NULL ? NULL : dreams);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::W(TAG, "Failed to set active dream to %s 0x%08x", TO_CSTR(dream), ec);
        return ec;
    }
    return NOERROR;
}

AutoPtr<IComponentName> DreamBackend::GetActiveDream()
{
    if (mDreamManager == NULL) {
        return NULL;
    }
    AutoPtr< ArrayOf<IComponentName*> > dreams;
    ECode ec = mDreamManager->GetDreamComponents((ArrayOf<IComponentName*>**)&dreams);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::W(TAG, "Failed to get active dream 0x%08x", ec);
        return NULL;
    }
    else if (SUCCEEDED(ec)) {
        if (dreams != NULL && dreams->GetLength() > 0) {
            AutoPtr<IComponentName> comp = (*dreams)[0];
            return comp;
        }
        return NULL;
    }
    return NULL;
}

ECode DreamBackend::LaunchSettings(
    /* [in] */ DreamInfo* dreamInfo)
{
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, (IObject*)dreamInfo);

    Logd(String("LaunchSettings(%s)"), args);
    if (dreamInfo == NULL || dreamInfo->mSettingsComponentName == NULL) {
        return NOERROR;
    }
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetComponent(dreamInfo->mSettingsComponentName);
    mContext->StartActivity(intent);
    return NOERROR;
}

ECode DreamBackend::Preview(
    /* [in] */ DreamInfo* dreamInfo)
{
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, (IObject*)dreamInfo);

    Logd(String("Preview(%s)"), args);
    if (mDreamManager == NULL || dreamInfo == NULL || dreamInfo->mComponentName == NULL) {
        return NOERROR;
    }
    ECode ec = mDreamManager->TestDream(dreamInfo->mComponentName);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::W(TAG, "Failed to preview %s 0x%08x", TO_CSTR(dreamInfo), ec);
    }
    return NOERROR;
}

ECode DreamBackend::StartDreaming()
{
    Logd(String("StartDreaming()"), NULL);
    if (mDreamManager == NULL) {
        return NOERROR;
    }
    ECode ec = mDreamManager->Dream();
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::W(TAG, "Failed to dream 0x%08x", ec);
    }
    return NOERROR;
}

AutoPtr<IComponentName> DreamBackend::GetDreamComponentName(
    /* [in] */ IResolveInfo* resolveInfo)
{
    AutoPtr<IServiceInfo> serviceInfo;
    if (resolveInfo == NULL || (resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo), serviceInfo) == NULL) {
        return NULL;
    }

    IPackageItemInfo* pii = IPackageItemInfo::Probe(serviceInfo);
    String packageName, name;
    pii->GetPackageName(&packageName);
    pii->GetName(&name);
    AutoPtr<IComponentName> comp;
    CComponentName::New(packageName, name, (IComponentName**)&comp);
    return comp;
}

AutoPtr<IComponentName> DreamBackend::GetSettingsComponentName(
    /* [in] */ IPackageManager* pm,
    /* [in] */ IResolveInfo* resolveInfo)
{
    if (resolveInfo == NULL) return NULL;
    AutoPtr<IServiceInfo> serviceInfo;
    resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
    if (serviceInfo == NULL) return NULL;
    AutoPtr<IBundle> metaData;
    IPackageItemInfo::Probe(serviceInfo)->GetMetaData((IBundle**)&metaData);
    if (metaData == NULL) return NULL;

    String cn;
    AutoPtr<IXmlResourceParser> parser;
    ECode ec = NOERROR;
    // Exception caughtException = NULL;
    // try {
        IPackageItemInfo::Probe(serviceInfo)->LoadXmlMetaData(pm,
                IDreamService::DREAM_META_DATA, (IXmlResourceParser**)&parser);
        if (parser == NULL) {
            Logger::W(TAG, "No %s meta-data", IDreamService::DREAM_META_DATA.string());
            return NULL;
        }
        AutoPtr<IApplicationInfo> applicationInfo;
        IComponentInfo::Probe(serviceInfo)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        AutoPtr<IResources> res;
        pm->GetResourcesForApplication(applicationInfo, (IResources**)&res);
        IXmlPullParser* _parser = IXmlPullParser::Probe(parser);
        AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(_parser);
        Int32 type;
        while (( _parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
                && type != IXmlPullParser::START_TAG) {
        }
        String nodeName;
        _parser->GetName(&nodeName);
        if (!String("dream").Equals(nodeName)) {
            Logger::W(TAG, "Meta-data does not start with dream tag");
            if (parser != NULL) parser->Close();
            return NULL;
        }
        AutoPtr<ArrayOf<Int32> > attrsId = TO_ATTRS_ARRAYOF(Elastos::Droid::R::styleable::Dream);
        AutoPtr<ITypedArray> sa;
        res->ObtainAttributes(attrs, attrsId, (ITypedArray**)&sa);
        sa->GetString(Elastos::Droid::R::styleable::Dream_settingsActivity, &cn);
        sa->Recycle();
    // } catch (NameNotFoundException e) {
    //     caughtException = e;
    // } catch (IOException e) {
    //     caughtException = e;
    // } catch (XmlPullParserException e) {
    //     caughtException = e;
    // } finally {
        if (parser != NULL) parser->Close();
    // }
    if (ec != NOERROR) {
        String packageName;
        IPackageItemInfo::Probe(serviceInfo)->GetPackageName(&packageName);
        Logger::W(TAG, "Error parsing : %s , %0x08x", packageName.string(), ec);
        return NULL;
    }
    if (!cn.IsNull() && cn.IndexOf('/') < 0) {
        String packageName;
        IPackageItemInfo::Probe(serviceInfo)->GetPackageName(&packageName);

        StringBuilder builder("");
        builder += packageName;
        builder += "/";
        builder += cn;
        cn = builder.ToString();
    }

    AutoPtr<IComponentName> comp;
    if (!cn.IsNull()) {
        AutoPtr<IComponentNameHelper> helper;
        CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
        helper->UnflattenFromString(cn, (IComponentName**)&comp);
    }
    return comp;
}

void DreamBackend::Logd(
    /* [in] */ const String& msg,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    if (CDreamSettings::DEBUG) {
        Logger::D(TAG, args == NULL || args->GetLength() == 0 ? msg : StringUtils::Format(msg, args));
    }
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos