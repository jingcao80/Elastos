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

#include "Elastos.Droid.Utility.h"
#include <Elastos.CoreLibrary.External.h>
#include "elastos/droid/service/voice/VoiceInteractionServiceInfo.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/content/pm/ServiceInfo.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/R.h"

using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::ServiceInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::Xml;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Droid::Manifest;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Voice {

const String VoiceInteractionServiceInfo::TAG("VoiceInteractionServiceInfo");

CAR_INTERFACE_IMPL(VoiceInteractionServiceInfo, Object, IVoiceInteractionServiceInfo)

VoiceInteractionServiceInfo::VoiceInteractionServiceInfo()
{
}

ECode VoiceInteractionServiceInfo::constructor(
    /* [in] */ IPackageManager* pm,
    /* [in] */ IComponentName* comp)
{
    AutoPtr<IServiceInfo> si;
    pm->GetServiceInfo(comp, IPackageManager::GET_META_DATA, (IServiceInfo**)&si);
    return constructor(pm, si.Get());
}

ECode VoiceInteractionServiceInfo::constructor(
    /* [in] */ IPackageManager* pm,
    /* [in] */ IComponentName* comp,
    /* [in] */ Int32 userHandle)
{
    AutoPtr<IIPackageManager> ipm = AppGlobals::GetPackageManager();
    AutoPtr<IServiceInfo> si;
    ipm->GetServiceInfo(comp, IPackageManager::GET_META_DATA, userHandle, (IServiceInfo**)&si);
    return constructor(pm, si.Get());
}

ECode VoiceInteractionServiceInfo::constructor(
    /* [in] */ IPackageManager* pm,
    /* [in] */ IServiceInfo* si)
{
    AutoPtr<ServiceInfo> _si = (ServiceInfo*)si;
    if (!Manifest::permission::BIND_VOICE_INTERACTION.Equals(_si->mPermission)) {
        mParseError = String("Service does not require permission ")
                + Manifest::permission::BIND_VOICE_INTERACTION;
        return E_NULL_POINTER_EXCEPTION;
    }

    // try {
    AutoPtr<IPackageItemInfo> pii = IPackageItemInfo::Probe(si);
    AutoPtr<IXmlResourceParser> parser;
    pii->LoadXmlMetaData(pm, IVoiceInteractionService::SERVICE_META_DATA, (IXmlResourceParser**)&parser);

    if (parser == NULL) {
        mParseError = String("No ") + IVoiceInteractionService::SERVICE_META_DATA
                + " meta-data for " + _si->mPackageName;
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<IResources> res;
    pm->GetResourcesForApplication(_si->mApplicationInfo, (IResources**)&res);

    AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(IXmlPullParser::Probe(parser));


    Int32 type;
    IXmlPullParser::Probe(parser)->Next(&type);
    while (type != IXmlPullParser::END_DOCUMENT && type != IXmlPullParser::START_TAG) {
    }

    String nodeName;
    IXmlPullParser::Probe(parser)->GetName(&nodeName);
    if (!nodeName.Equals("voice-interaction-service")) {
        mParseError = String("Meta-data does not start with voice-interaction-service tag");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::VoiceInteractionService);
    AutoPtr<ITypedArray> array;
    res->ObtainAttributes(attrs, attrIds, (ITypedArray**)&array);
    array->GetString(
       R::styleable::VoiceInteractionService_sessionService, &mSessionService);
    array->GetString(
       R::styleable::VoiceInteractionService_recognitionService, &mRecognitionService);
    array->GetString(
       R::styleable::VoiceInteractionService_settingsActivity, &mSettingsActivity);
    array->Recycle();
    if (mSessionService == NULL) {
        mParseError = String("No sessionService specified");
        return E_NULL_POINTER_EXCEPTION;
    }
    /* Not yet time
    if (mRecognitionService == null) {
        mParseError = "No recogitionService specified";
        return;
    } */
    assert(0);
    //TODO
    // } catch (XmlPullParserException e) {
    //     mParseError = "Error parsing voice interation service meta-data: " + e;
    //     Log.w(TAG, "error parsing voice interaction service meta-data", e);
    //     return;
    // } catch (IOException e) {
    //     mParseError = "Error parsing voice interation service meta-data: " + e;
    //     Log.w(TAG, "error parsing voice interaction service meta-data", e);
    //     return;
    // } catch (PackageManager.NameNotFoundException e) {
    //     mParseError = "Error parsing voice interation service meta-data: " + e;
    //     Log.w(TAG, "error parsing voice interaction service meta-data", e);
    //     return;
    // } finally {
    if (parser != NULL) parser->Close();
    // }
    mServiceInfo = si;
    return NOERROR;
}

ECode VoiceInteractionServiceInfo::GetParseError(
    /* [out] */ String* error)
{
    VALIDATE_NOT_NULL(error)
    *error = mParseError;
    return NOERROR;
}

ECode VoiceInteractionServiceInfo::GetServiceInfo(
    /* [out] */ IServiceInfo** info)
{
    VALIDATE_NOT_NULL(info)
    *info = mServiceInfo;
    REFCOUNT_ADD(*info)
    return NOERROR;
}

ECode VoiceInteractionServiceInfo::GetSessionService(
    /* [out] */ String* service)
{
    VALIDATE_NOT_NULL(service)
    *service = mSessionService;
    return NOERROR;
}

ECode VoiceInteractionServiceInfo::GetRecognitionService(
    /* [out] */ String* service)
{
    VALIDATE_NOT_NULL(service)
    *service = mRecognitionService;
    return NOERROR;
}

ECode VoiceInteractionServiceInfo::GetSettingsActivity(
    /* [out] */ String* activity)
{
    VALIDATE_NOT_NULL(activity)
    *activity = mSettingsActivity;
    return NOERROR;
}

} // namespace Voice
} // namespace Service
} // namepsace Droid
} // namespace Elastos
