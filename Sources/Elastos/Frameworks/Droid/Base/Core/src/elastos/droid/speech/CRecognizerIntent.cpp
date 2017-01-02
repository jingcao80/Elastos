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

#include "elastos/droid/speech/CRecognizerIntent.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/pm/CActivityInfo.h"
#include "elastos/droid/content/CComponentName.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::CActivityInfo;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;

namespace Elastos {
namespace Droid {
namespace Speech {

CAR_INTERFACE_IMPL(CRecognizerIntent, Singleton, IRecognizerIntent);

CAR_SINGLETON_IMPL(CRecognizerIntent)

ECode CRecognizerIntent::GetVoiceDetailsIntent(
    /* [in] */ IContext* pContext,
    /* [out] */ IIntent** ppRet)
{
    VALIDATE_NOT_NULL(ppRet);

    AutoPtr<IIntent> voiceSearchIntent;
    CIntent::New(ACTION_WEB_SEARCH, (IIntent**)&voiceSearchIntent);

    AutoPtr<IPackageManager> pm;
    pContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IResolveInfo> ri;
    pm->ResolveActivity(voiceSearchIntent, IPackageManager::GET_META_DATA, (IResolveInfo**)&ri);

    AutoPtr<IActivityInfo> activityInfo;
    ri->GetActivityInfo((IActivityInfo**)&activityInfo);
    AutoPtr<IBundle> metaData;
/*
    if (activityInfo == NULL) {
        activityInfo->GetMetaData((IBundle**)&metaData);
    }
    if (ri == NULL || (activityInfo == NULL || metaData == NULL)) {
*/
    if (ri == NULL || activityInfo == NULL) {
        *ppRet = NULL;
        return NOERROR;
    }

    String className;
    metaData->GetString(DETAILS_META_DATA, &className);
    if (className.IsNull()) {
        *ppRet = NULL;
        return NOERROR;
    }

    AutoPtr<IIntent> detailsIntent;
    CIntent::New(ACTION_GET_LANGUAGE_DETAILS, (IIntent**)&detailsIntent);
    String packageName;
    ((CActivityInfo*)( activityInfo.Get() ) )->GetPackageName(&packageName);
    AutoPtr<IComponentName> cn;
    CComponentName::New(packageName, className, (IComponentName**)&cn);
    detailsIntent->SetComponent(cn.Get());

    *ppRet = detailsIntent;
    REFCOUNT_ADD(*ppRet)
    return NOERROR;
}

} // namespace Speech
} // namespace Droid
} // namespace Elastos

