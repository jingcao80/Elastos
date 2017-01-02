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

#include "elastos/droid/systemui/statusbar/policy/LocationControllerImpl.h"
#include "elastos/droid/systemui/statusbar/policy/CLocationControllerBroadcastReceiver.h"
#include "elastos/droid/systemui/statusbar/policy/CLocationControllerLocationModeChangedReceiver.h"
#include "../R.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/droid/provider/Settings.h>
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/os/Binder.h"
using Elastos::Droid::Os::Binder;

using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::IAppOpsManagerOpEntry;
using Elastos::Droid::App::IAppOpsManagerPackageOps;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const String LocationControllerImpl::LOCATION_STATUS_ICON_PLACEHOLDER("location");
const Int32 LocationControllerImpl::LOCATION_STATUS_ICON_ID = R::drawable::stat_sys_location;
AutoPtr<ArrayOf<Int32> > LocationControllerImpl::mHighPowerRequestAppOpArray = InitStatic();

CAR_INTERFACE_IMPL(LocationControllerImpl, Object, ILocationController)

LocationControllerImpl::LocationControllerImpl()
    : mAreActiveLocationRequests(FALSE)
{
}

LocationControllerImpl::~LocationControllerImpl()
{
    mContext->UnregisterReceiver(mLocationModeChagedReceiver);
    mContext->UnregisterReceiver(mHighPowerRequestChagedReceiver);
}

ECode LocationControllerImpl::constructor(
    /* [in] */ IContext* context)
{
    CArrayList::New((IArrayList**)&mSettingsChangeCallbacks);
    mContext = context;


    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> ALL;
    helper->GetALL((IUserHandle**)&ALL);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(ILocationManager::HIGH_POWER_REQUEST_CHANGE_ACTION);
    CLocationControllerBroadcastReceiver::New(this, (IBroadcastReceiver**)&mHighPowerRequestChagedReceiver);
    AutoPtr<IIntent> i;
    context->RegisterReceiverAsUser(mHighPowerRequestChagedReceiver, ALL,
        filter, String(NULL), NULL, (IIntent**)&i);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&obj);
    mAppOpsManager = IAppOpsManager::Probe(obj);

    obj = NULL;
    context->GetSystemService(IContext::STATUS_BAR_SERVICE, (IInterface**)&obj);
    mStatusBarManager = IStatusBarManager::Probe(obj);

    // Register to listen for changes in location settings.
    i = NULL;
    filter = NULL;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(ILocationManager::MODE_CHANGED_ACTION);
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    CLocationControllerLocationModeChangedReceiver::New(this, (IBroadcastReceiver**)&mLocationModeChagedReceiver);
    context->RegisterReceiverAsUser(mLocationModeChagedReceiver, ALL,
        filter, String(NULL), handler, (IIntent**)&i);

    // Examine the current location state and initialize the status view.
    UpdateActiveLocationRequests();
    RefreshViews();
    return NOERROR;
}

AutoPtr<ArrayOf<Int32> > LocationControllerImpl::InitStatic()
{
    AutoPtr<ArrayOf<Int32> > a = ArrayOf<Int32>::Alloc(1);
    (*a)[0] = IAppOpsManager::OP_MONITOR_HIGH_POWER_LOCATION;
    return a;
}

ECode LocationControllerImpl::AddSettingsChangedCallback(
    /* [in] */ ILocationSettingsChangeCallback* cb)
{
    mSettingsChangeCallbacks->Add(cb);
    LocationSettingsChanged(cb);
    return NOERROR;
}

ECode LocationControllerImpl::RemoveSettingsChangedCallback(
    /* [in] */ ILocationSettingsChangeCallback* cb)
{
    mSettingsChangeCallbacks->Remove(cb);
    return NOERROR;
}

ECode LocationControllerImpl::SetLocationEnabled(
    /* [in] */ Boolean enabled,
    /* [out] */ Boolean* result)
{
    Slogger::I("LocationControllerImpl", " SetLocationEnabled %d", enabled);
    VALIDATE_NOT_NULL(result);
    Int32 currentUserId = 0;
    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    amHelper->GetCurrentUser(&currentUserId);
    if (IsUserLocationRestricted(currentUserId)) {
        Slogger::I("LocationControllerImpl", " location restricted for %d", currentUserId);
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    // When enabling location, a user consent dialog will pop up, and the
    // setting won't be fully enabled until the user accepts the agreement.
    Int32 mode = enabled
            ? ISettingsSecure::LOCATION_MODE_HIGH_ACCURACY : ISettingsSecure::LOCATION_MODE_OFF;
    // QuickSettings always runs as the owner, so specifically set the settings
    // for the current foreground user.
    Boolean tmp = FALSE;
    return Elastos::Droid::Provider::Settings::Secure::PutInt32ForUser(cr,
            ISettingsSecure::LOCATION_MODE, mode, currentUserId, &tmp);
}

ECode LocationControllerImpl::IsLocationEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    // QuickSettings always runs as the owner, so specifically retrieve the settings
    // for the current foreground user.
    Int32 currentUserId = 0;
    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    amHelper->GetCurrentUser(&currentUserId);
    Int32 mode = 0;
    Elastos::Droid::Provider::Settings::Secure::GetInt32ForUser(resolver, ISettingsSecure::LOCATION_MODE,
            ISettingsSecure::LOCATION_MODE_OFF, currentUserId, &mode);
    *result = mode != ISettingsSecure::LOCATION_MODE_OFF;
    return NOERROR;
}

Boolean LocationControllerImpl::IsUserLocationRestricted(
    /* [in] */ Int32 userId)
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    AutoPtr<IUserManager> um = IUserManager::Probe(obj);
    AutoPtr<IUserHandle> uh;
    CUserHandle::New(userId, (IUserHandle**)&uh);
    Boolean result = FALSE;
    um->HasUserRestriction(IUserManager::DISALLOW_SHARE_LOCATION, uh, &result);
    return result;
}

Boolean LocationControllerImpl::AreActiveHighPowerLocationRequests()
{
    AutoPtr<IList> packages;  /*<AppOpsManager.PackageOps*/
    mAppOpsManager->GetPackagesForOps(mHighPowerRequestAppOpArray, (IList**)&packages);
    // AppOpsManager can return NULL when there is no requested data.
    if (packages != NULL) {
        Int32 numPackages = 0;
        packages->GetSize(&numPackages);
        for (Int32 packageInd = 0; packageInd < numPackages; packageInd ++) {
            AutoPtr<IInterface> obj;
            packages->Get(packageInd, (IInterface**)&obj);
            AutoPtr<IAppOpsManagerPackageOps> packageOp = IAppOpsManagerPackageOps::Probe(obj);
            AutoPtr<IList> opEntries;  /*<AppOpsManager.OpEntry*/
            packageOp->GetOps((IList**)&opEntries);
            if (opEntries != NULL) {
                Int32 numOps = 0;
                opEntries->GetSize(&numOps);
                for (Int32 opInd = 0; opInd < numOps; opInd ++) {
                    AutoPtr<IInterface> e;
                    opEntries->Get(opInd, (IInterface**)&e);
                    AutoPtr<IAppOpsManagerOpEntry> opEntry = IAppOpsManagerOpEntry::Probe(e);
                    // AppOpsManager should only return OP_MONITOR_HIGH_POWER_LOCATION because
                    // of the mHighPowerRequestAppOpArray filter, but checking defensively.
                    Int32 op = 0;
                    opEntry->GetOp(&op);
                    if (op == IAppOpsManager::OP_MONITOR_HIGH_POWER_LOCATION) {
                        Boolean r = FALSE;
                        if (opEntry->IsRunning(&r), r) {
                            return TRUE;
                        }
                    }
                }
            }
        }
    }

    return FALSE;
}

void LocationControllerImpl::RefreshViews()
{
    if (mAreActiveLocationRequests) {
        String value;
        mContext->GetString(R::string::accessibility_location_active, &value);
        mStatusBarManager->SetIcon(LOCATION_STATUS_ICON_PLACEHOLDER, LOCATION_STATUS_ICON_ID, 0, value);
    }
    else {
        mStatusBarManager->RemoveIcon(LOCATION_STATUS_ICON_PLACEHOLDER);
    }
}

void LocationControllerImpl::UpdateActiveLocationRequests()
{
    Boolean hadActiveLocationRequests = mAreActiveLocationRequests;
    mAreActiveLocationRequests = AreActiveHighPowerLocationRequests();
    if (mAreActiveLocationRequests != hadActiveLocationRequests) {
        RefreshViews();
    }
}

void LocationControllerImpl::LocationSettingsChanged()
{
    Boolean isEnabled = FALSE;
    IsLocationEnabled(&isEnabled);

    AutoPtr<IIterator> ator;
    mSettingsChangeCallbacks->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> cb;
        ator->GetNext((IInterface**)&cb);
        ILocationSettingsChangeCallback::Probe(cb)->OnLocationSettingsChanged(isEnabled);
    }
}

void LocationControllerImpl::LocationSettingsChanged(
    /* [in] */ ILocationSettingsChangeCallback* cb)
{
    Boolean tmp = FALSE;
    IsLocationEnabled(&tmp);
    cb->OnLocationSettingsChanged(tmp);
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
