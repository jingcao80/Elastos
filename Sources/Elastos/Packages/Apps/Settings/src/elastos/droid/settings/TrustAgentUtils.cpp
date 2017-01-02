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
#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.External.h"
#include "elastos/droid/settings/TrustAgentUtils.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

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
using Elastos::Droid::Service::Trust::ITrustAgentService;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Manifest;
using Elastos::Droid::R;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Settings {

const String TrustAgentUtils::TAG("TrustAgentUtils");

const String TrustAgentUtils::TRUST_AGENT_META_DATA = ITrustAgentService::TRUST_AGENT_META_DATA;
const String TrustAgentUtils::PERMISSION_PROVIDE_AGENT = Manifest::permission::PROVIDE_TRUST_AGENT;

Boolean TrustAgentUtils::CheckProvidePermission(
    /* [in] */ IResolveInfo* resolveInfo,
    /* [in] */ IPackageManager* pm)
{
    AutoPtr<IServiceInfo> serviceInfo;
    resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
    String packageName;
    IPackageItemInfo::Probe(serviceInfo)->GetPackageName(&packageName);
    Int32 data;
    pm->CheckPermission(PERMISSION_PROVIDE_AGENT, packageName, &data);
    if (data != IPackageManager::PERMISSION_GRANTED) {
        Logger::W(TAG, "Skipping agent because package %s does not have permission %s.",
                packageName.string(), PERMISSION_PROVIDE_AGENT.string());
        return FALSE;
    }
    return TRUE;
}

AutoPtr<IComponentName> TrustAgentUtils::GetComponentName(
    /* [in] */ IResolveInfo* resolveInfo)
{
    if (resolveInfo == NULL) return NULL;
    AutoPtr<IServiceInfo> serviceInfo;
    resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
    if (serviceInfo == NULL) return NULL;

    IPackageItemInfo* _serviceInfo = IPackageItemInfo::Probe(serviceInfo);
    String packageName;
    _serviceInfo->GetPackageName(&packageName);
    String name;
    _serviceInfo->GetName(&name);

    AutoPtr<IComponentName> cn;
    CComponentName::New(packageName, name, (IComponentName**)&cn);
    return cn;
}

AutoPtr<TrustAgentUtils::TrustAgentComponentInfo> TrustAgentUtils::GetSettingsComponent(
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

    String packageName;
    IPackageItemInfo::Probe(serviceInfo)->GetPackageName(&packageName);

    String cn;
    AutoPtr<TrustAgentComponentInfo> trustAgentComponentInfo = new TrustAgentComponentInfo();
    AutoPtr<IXmlResourceParser> parser;
    ECode ec = NOERROR;
    // Exception caughtException = NULL;
    // try {
        IPackageItemInfo::Probe(serviceInfo)->LoadXmlMetaData(pm, TRUST_AGENT_META_DATA, (IXmlResourceParser**)&parser);
        if (parser == NULL) {
            Logger::W(TAG, "Can't find %s meta-data", TRUST_AGENT_META_DATA.string());
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
        if (!String("trust-agent").Equals(nodeName)) {
            Logger::W(TAG, "Meta-data does not start with trust-agent tag");
            if (parser != NULL) parser->Close();
            return NULL;
        }
        AutoPtr<ArrayOf<Int32> > attrsId = TO_ATTRS_ARRAYOF(R::styleable::TrustAgent);
        AutoPtr<ITypedArray> sa;
        res->ObtainAttributes(attrs, attrsId, (ITypedArray**)&sa);
        sa->GetString(R::styleable::TrustAgent_summary, &trustAgentComponentInfo->mSummary);
        sa->GetString(R::styleable::TrustAgent_title, &trustAgentComponentInfo->mTitle);
        sa->GetString(R::styleable::TrustAgent_settingsActivity, &cn);
        sa->Recycle();
    // } catch (PackageManager.NameNotFoundException e) {
    //     caughtException = e;
    // } catch (IOException e) {
    //     caughtException = e;
    // } catch (XmlPullParserException e) {
    //     caughtException = e;
    // } finally {
        if (parser != NULL) parser->Close();
    // }

    if (ec != NOERROR) {
        Logger::W(TAG, "Error parsing : %s , %0x08x", packageName.string(), ec);
        return NULL;
    }
    if (!cn.IsNull() && cn.IndexOf('/') < 0) {
        StringBuilder builder("");
        builder += packageName;
        builder += "/";
        builder += cn;
        cn = builder.ToString();
    }

    trustAgentComponentInfo->mComponentName = NULL;
    if (!cn.IsNull()) {
        AutoPtr<IComponentNameHelper> helper;
        CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
        AutoPtr<IComponentName> component;
        helper->UnflattenFromString(cn, (IComponentName**)&component);
        trustAgentComponentInfo->mComponentName = component;
    }
    return trustAgentComponentInfo;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
