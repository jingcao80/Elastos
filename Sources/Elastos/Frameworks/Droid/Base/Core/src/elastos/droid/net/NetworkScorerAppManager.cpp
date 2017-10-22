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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.App.h"
#include <Elastos.Droid.Provider.h>
#include "elastos/droid/net/NetworkScorerAppManager.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/net/CNetwork.h"
#include "elastos/droid/net/CNetworkScorerAppData.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/net/NetworkScoreManager.h"
#include "elastos/droid/net/NetworkScorerAppManager.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Manifest;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Text::TextUtils;

using Elastos::Core::ICharSequence;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {

AutoPtr<IIntent> NetworkScorerAppManager::CreateScoreIntent()
{
    AutoPtr<IIntent> rev;
    CIntent::New((IIntent**)&rev);
    return rev;
}

const AutoPtr<IIntent> NetworkScorerAppManager::SCORE_INTENT = CreateScoreIntent();

const String NetworkScorerAppManager::TAG("NetworkScorerAppManager");

NetworkScorerAppManager::NetworkScorerAppManager()
{}

ECode NetworkScorerAppManager::GetAllValidScorers(
    /* [in] */ IContext* context,
    /* [out] */ ICollection** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IList> scorers;
    CArrayList::New((IList**)&scorers);
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    // Only apps installed under the primary user of the device can be scorers.
    AutoPtr<IList> receivers;
    pm->QueryBroadcastReceivers(SCORE_INTENT, 0 /* flags */, UserHandle::USER_OWNER, (IList**)&receivers);
    AutoPtr<IIterator> it;
    receivers->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        IResolveInfo* receiver = IResolveInfo::Probe(obj);
        // This field is a misnomer, see android.content.pm.ResolveInfo#activityInfo
        /* final */ AutoPtr<IActivityInfo> receiverInfo;
        receiver->GetActivityInfo((IActivityInfo**)&receiverInfo);
        if (receiverInfo == NULL) {
            // Should never happen with queryBroadcastReceivers, but invalid nonetheless.
            continue;
        }
        String permission;
        receiverInfo->GetPermission(&permission);
        if (!Manifest::permission::BROADCAST_SCORE_NETWORKS.Equals(permission)) {
            // Receiver doesn't require the BROADCAST_SCORE_NETWORKS permission, which means
            // anyone could trigger network scoring and flood the framework with score requests.
            continue;
        }
        String packageName;
        IPackageItemInfo::Probe(receiverInfo)->GetPackageName(&packageName);
        Int32 checkPermission;
        pm->CheckPermission(Manifest::permission::SCORE_NETWORKS, packageName, &checkPermission);
        if (checkPermission != IPackageManager::PERMISSION_GRANTED) {
            // Application doesn't hold the SCORE_NETWORKS permission, so the user never
            // approved it as a network scorer.
            continue;
        }
        // Optionally, this package may specify a configuration activity.
        String configurationActivityClassName(NULL);
        AutoPtr<IIntent> intent;
        CIntent::New(INetworkScoreManager::ACTION_CUSTOM_ENABLE, (IIntent**)&intent);
        intent->SetPackage(packageName);
        AutoPtr<IList> configActivities;
        pm->QueryIntentActivities(intent, 0 /* flags */, (IList**)&configActivities);
        Boolean empty;
        if (configActivities->IsEmpty(&empty), !empty) {
            AutoPtr<IInterface> obj;
            configActivities->Get(0, (IInterface**)&obj);
            AutoPtr<IActivityInfo> activityInfo;
            IResolveInfo::Probe(obj)->GetActivityInfo((IActivityInfo**)&activityInfo);
            if (activityInfo != NULL) {
                activityInfo->GetParentActivityName(&configurationActivityClassName);
            }
        }
        // NOTE: loadLabel will attempt to load the receiver's label and fall back to the app
        // label if none is present.
        AutoPtr<ICharSequence> csq;
        IPackageItemInfo::Probe(receiverInfo)->LoadLabel(pm, (ICharSequence**)&csq);
        AutoPtr<INetworkScorerAppData> newNetworkScorerAppData;
        CNetworkScorerAppData::New(packageName, csq,
                configurationActivityClassName, (INetworkScorerAppData**)&newNetworkScorerAppData);
        scorers->Add(newNetworkScorerAppData);
    }
    *result = ICollection::Probe(scorers);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkScorerAppManager::GetActiveScorer(
    /* [in] */ IContext* context,
    /* [out] */ INetworkScorerAppData** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    String scorerPackage;
    Settings::Global::GetString(cr,
            ISettingsGlobal::NETWORK_SCORER_APP, &scorerPackage);
    return GetScorer(context, scorerPackage, result);
}

ECode NetworkScorerAppManager::SetActiveScorer(
    /* [in] */ IContext* context,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    String oldPackageName;
    Settings::Global::GetString(cr,
            ISettingsGlobal::NETWORK_SCORER_APP, &oldPackageName);
    if (TextUtils::Equals(oldPackageName, packageName)) {
        // No change.
        *result = TRUE;
        return NOERROR;
    }
    Logger::I(TAG, "Changing network scorer from %s to %s", oldPackageName.string(), packageName.string());
    if (packageName.IsNull()) {
        Boolean bTemp;
        Settings::Global::PutString(cr,
                 ISettingsGlobal::NETWORK_SCORER_APP, String(NULL), &bTemp);
        *result = TRUE;
        return NOERROR;
    }
    else {
        // We only make the change if the new package is valid.
        AutoPtr<INetworkScorerAppData> networkScorerAppData;
        GetScorer(context, packageName, (INetworkScorerAppData**)&networkScorerAppData);
        if (networkScorerAppData != NULL) {
            Boolean bTemp;
            Settings::Global::PutString(cr,
                     ISettingsGlobal::NETWORK_SCORER_APP, packageName, &bTemp);
            *result = TRUE;
            return NOERROR;
        }
        else {
            Logger::W(TAG, "Requested network scorer is not valid: %s", packageName.string());
            *result = FALSE;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode NetworkScorerAppManager::IsCallerActiveScorer(
    /* [in] */ IContext* context,
    /* [in] */ Int32 callingUid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<INetworkScorerAppData> defaultApp;
    GetActiveScorer(context, (INetworkScorerAppData**)&defaultApp);
    if (defaultApp == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&obj);
    AutoPtr<IAppOpsManager> appOpsMgr = IAppOpsManager::Probe(obj);
    // try {
    String package;
    defaultApp->GetPackageName(&package);
    ECode ec = appOpsMgr->CheckPackage(callingUid, package);
    // } catch (SecurityException e) {
    if (ec == (ECode)E_SECURITY_EXCEPTION) {
        *result = FALSE;
        return NOERROR;
    }
    if (FAILED(ec)) return ec;
    // }
    // To be extra safe, ensure the caller holds the SCORE_NETWORKS permission. It always
    // should, since it couldn't become the active scorer otherwise, but this can't hurt.
    Int32 checkNo;
    context->CheckCallingPermission(Manifest::permission::SCORE_NETWORKS, &checkNo);
    *result = checkNo == IPackageManager::PERMISSION_GRANTED;
    return NOERROR;
}

ECode NetworkScorerAppManager::GetScorer(
    /* [in] */ IContext* context,
    /* [in] */ const String& packageName,
    /* [out] */ INetworkScorerAppData** result)
{
    VALIDATE_NOT_NULL(result)

    if (TextUtils::IsEmpty(packageName)) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<ICollection> applications;
    GetAllValidScorers(context, (ICollection**)&applications);
    AutoPtr<IIterator> it;
    applications->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> app;
        it->GetNext((IInterface**)&app);
        String appPackage;
        INetworkScorerAppData::Probe(app)->GetPackageName(&appPackage);
        if (packageName.Equals(appPackage)) {
            *result = INetworkScorerAppData::Probe(app);
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    *result = NULL;
    return NOERROR;
}

//=====================================================================
// NetworkScorerAppData
//=====================================================================
CAR_INTERFACE_IMPL(NetworkScorerAppData, Object, INetworkScorerAppData)

ECode NetworkScorerAppData::constructor(
    /* [in] */ const String& packageName,
    /* [in] */ ICharSequence* scorerName,
    /* [in] */ /* @Nullable */ const String& configurationActivityClassName)
{
    mScorerName = scorerName;
    mPackageName = packageName;
    mConfigurationActivityClassName = configurationActivityClassName;
    return NOERROR;
}

ECode NetworkScorerAppData::GetPackageName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mPackageName;
    return NOERROR;
}

ECode NetworkScorerAppData::GetScorerName(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mScorerName;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode NetworkScorerAppData::GetConfigurationActivityClassName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mConfigurationActivityClassName;
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
