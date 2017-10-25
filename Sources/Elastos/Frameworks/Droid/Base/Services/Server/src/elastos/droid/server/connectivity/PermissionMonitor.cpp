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

#include "elastos/droid/server/connectivity/PermissionMonitor.h"
#include <elastos/droid/Manifest.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Os::CUserManagerHelper;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Connectivity {

const String PermissionMonitor::TAG("PermissionMonitor");
const Boolean PermissionMonitor::DBG = TRUE;
const Boolean PermissionMonitor::SYSTEM = TRUE;
const Boolean PermissionMonitor::NETWORK = FALSE;

//======================================================================
// PermissionMonitor::IntentReceiver
//======================================================================
PermissionMonitor::IntentReceiver::IntentReceiver(
    /* [in] */ PermissionMonitor* host)
    : mHost(host)
{}

ECode PermissionMonitor::IntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    Int32 user, appUid;
    intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, IUserHandle::USER_NULL, &user);
    intent->GetInt32Extra(IIntent::EXTRA_UID, -1, &appUid);
    AutoPtr<IUri> appData;
    intent->GetData((IUri**)&appData);
    String appName;
    if (appData != NULL) {
        appData->GetSchemeSpecificPart(&appName);
    }

    if (IIntent::ACTION_USER_ADDED.Equals(action)) {
        return mHost->OnUserAdded(user);
    }
    else if (IIntent::ACTION_USER_REMOVED.Equals(action)) {
        return mHost->OnUserRemoved(user);
    }
    else if (IIntent::ACTION_PACKAGE_ADDED.Equals(action)) {
        return mHost->OnAppAdded(appName, appUid);
    }
    else if (IIntent::ACTION_PACKAGE_REMOVED.Equals(action)) {
        return mHost->OnAppRemoved(appUid);
    }
    return NOERROR;
}

//======================================================================
// PermissionMonitor
//======================================================================
PermissionMonitor::PermissionMonitor(
    /* [in] */ IContext* context,
    /* [in] */ IINetworkManagementService* netd)
{
    mContext = context;
    context->GetPackageManager((IPackageManager**)&mPackageManager);
    AutoPtr<IUserManagerHelper> umHelper;
    CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&umHelper);
    umHelper->Get(context, (IUserManager**)&mUserManager);
    mNetd = netd;
    mIntentReceiver = new IntentReceiver(this);
}

void PermissionMonitor::StartMonitoring()
{
    AutoLock lock(this);
    Slogger::D(TAG, "Monitoring");

    String nullStr;
    AutoPtr<IIntent> stickyIntent;

    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntent::ACTION_USER_ADDED);
    intentFilter->AddAction(IIntent::ACTION_USER_REMOVED);
    mContext->RegisterReceiverAsUser(mIntentReceiver, UserHandle::ALL,
        intentFilter, nullStr, NULL, (IIntent**)&stickyIntent);

    intentFilter = NULL;
    stickyIntent = NULL;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntent::ACTION_PACKAGE_ADDED);
    intentFilter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
    intentFilter->AddDataScheme(String("package"));
    mContext->RegisterReceiverAsUser(mIntentReceiver, UserHandle::ALL,
        intentFilter, nullStr, NULL, (IIntent**)&stickyIntent);

    AutoPtr<IList> apps;
    mPackageManager->GetInstalledPackages(IPackageManager::GET_PERMISSIONS, (IList**)&apps);
    if (apps == NULL) {
        Slogger::E(TAG, "No apps");
        return;
    }

    AutoPtr<IIterator> it;
    apps->GetIterator((IIterator**)&it);
    Boolean hasNext;
    HashMap<Int32, Boolean>::Iterator pit;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        IPackageInfo* app = IPackageInfo::Probe(obj);

        Int32 uid = -1;
        AutoPtr<IApplicationInfo> appInfo;
        app->GetApplicationInfo((IApplicationInfo**)&appInfo);
        if (appInfo != NULL) {
            appInfo->GetUid(&uid);
        }

        if (uid < 0) {
            continue;
        }

        Boolean isNetwork = HasNetworkPermission(app);
        Boolean isSystem = HasSystemPermission(app);

        if (isNetwork || isSystem) {
            pit = mApps.Find(uid);
            if (pit != mApps.End()) {
                Boolean permission = pit->mSecond;
                // If multiple packages share a UID (cf: android:sharedUserId) and ask for different
                // permissions, don't downgrade (i.e., if it's already SYSTEM, leave it as is).
                if (permission == NETWORK) {
                    mApps[uid] = isSystem;
                }
            }
        }
    }

    AutoPtr<IList> users;
    mUserManager->GetUsers(TRUE, (IList**)&users);  // exclude dying users
    if (users != NULL) {
        it = NULL;
        users->GetIterator((IIterator**)&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            IUserInfo* user = IUserInfo::Probe(obj);
            Int32 id;
            user->GetId(&id);
            mUsers.Insert(id);
        }
    }

    Slogger::D(TAG, "Users: %d, Apps: %d", mUsers.GetSize(), mApps.GetSize());
    Update(&mUsers, &mApps, TRUE);
}

Boolean PermissionMonitor::HasPermission(
    /* [in] */ IPackageInfo* app,
    /* [in] */ const String& permission)
{
    AutoPtr<ArrayOf<String> > permissions;
    app->GetRequestedPermissions((ArrayOf<String>**)&permissions);
    if (permissions != NULL) {
        for (Int32 i = 0; i < permissions->GetLength(); ++i) {
            if (permission.Equals((*permissions)[i])) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

Boolean PermissionMonitor::HasNetworkPermission(
    /* [in] */ IPackageInfo* app)
{
    return HasPermission(app, Manifest::permission::CHANGE_NETWORK_STATE);
}

Boolean PermissionMonitor::HasSystemPermission(
    /* [in] */ IPackageInfo* app)
{
    Int32 flags = 0;
    AutoPtr<IApplicationInfo> appInfo;
    app->GetApplicationInfo((IApplicationInfo**)&appInfo);
    if (appInfo != NULL) {
        appInfo->GetFlags(&flags);
    }
    if ((flags & IApplicationInfo::FLAG_SYSTEM) != 0
        || (flags & IApplicationInfo::FLAG_UPDATED_SYSTEM_APP) != 0) {
        return TRUE;
    }
    return HasPermission(app, Manifest::permission::CONNECTIVITY_INTERNAL);
}

AutoPtr<ArrayOf<Int32> > PermissionMonitor::ToInt32Array(
    /* [in] */ List<Int32>* list)
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(list->GetSize());
    for (Int32 i = 0; i < array->GetLength(); i++) {
        (*array)[i] = (*list)[i];
    }
    return array;
}

ECode PermissionMonitor::Update(
    /* [in] */ HashSet<Int32>* users,
    /* [in] */ HashMap<Int32, Boolean>* apps,
    /* [in] */ Boolean add)
{
    List<Int32> network, system;

    HashSet<Int32>::Iterator sit;
    HashMap<Int32, Boolean>::Iterator mit;
    for (mit = apps->Begin(); mit != apps->End(); ++mit) {
        List<Int32>& list = mit->mSecond ? system : network;
        for (sit = users->Begin(); sit != users->End(); ++sit) {
            list.PushBack(UserHandle::GetUid(*sit, mit->mFirst));
        }
    }

    ECode ec = NOERROR;

    if (add) {
        ec = mNetd->SetPermission(Manifest::permission::CHANGE_NETWORK_STATE, ToInt32Array(&network));
        FAIL_GOTO(ec, _EXIT_)
        ec = mNetd->SetPermission(Manifest::permission::CONNECTIVITY_INTERNAL, ToInt32Array(&system));
        FAIL_GOTO(ec, _EXIT_)
    }
    else {
        ec = mNetd->ClearPermission(ToInt32Array(&network));
        FAIL_GOTO(ec, _EXIT_)
        ec = mNetd->ClearPermission(ToInt32Array(&system));
        FAIL_GOTO(ec, _EXIT_)
    }

_EXIT_:
    Slogger::E(TAG, "Exception when updating permissions: ");
    return ec;
}

ECode PermissionMonitor::OnUserAdded(
    /* [in] */ Int32 user)
{
    AutoLock lock(this);
    if (user < 0) {
        Slogger::E(TAG, "Invalid user in OnUserAdded: %d", user);
        return NOERROR;
    }
    mUsers.Insert(user);

    HashSet<Int32> users;
    users.Insert(user);
    return Update(&users, &mApps, TRUE);
}

ECode PermissionMonitor::OnUserRemoved(
    /* [in] */ Int32 user)
{
    AutoLock lock(this);
    if (user < 0) {
        Slogger::E(TAG, "Invalid user in OnUserRemoved: %d", user);
        return NOERROR;
    }

    HashSet<Int32>::Iterator it = mUsers.Find(user);
    if (it != mUsers.End()) {
        mUsers.Erase(it);
    }

    HashSet<Int32> users;
    users.Insert(user);
    return Update(&users, &mApps, FALSE);
}

ECode PermissionMonitor::OnAppAdded(
    /* [in] */ const String& appName,
    /* [in] */ const Int32 appUid)
{
    AutoLock lock(this);
    if (TextUtils::IsEmpty(appName) || appUid < 0) {
        Slogger::E(TAG, "Invalid app in OnAppAdded: %s | %d", appName.string(), appUid);
        return NOERROR;
    }

    Boolean isNetwork, isSystem, permission;
    ECode ec = NOERROR;
    AutoPtr<IPackageInfo> app;
    ec = mPackageManager->GetPackageInfo(appName, IPackageManager::GET_PERMISSIONS, (IPackageInfo**)&app);
    FAIL_GOTO(ec, _EXIT_)

    isNetwork = HasNetworkPermission(app);
    isSystem = HasSystemPermission(app);
    if (isNetwork || isSystem) {
        HashMap<Int32, Boolean>::Iterator it = mApps.Find(appUid);
        if (it != mApps.End()) {
            permission = it->mSecond;
            // If multiple packages share a UID (cf: android:sharedUserId) and ask for different
            // permissions, don't downgrade (i.e., if it's already SYSTEM, leave it as is).
            if (permission == NETWORK) {
                mApps[appUid] = isSystem;

                HashMap<Int32, Boolean> apps;
                apps[appUid] = isSystem;
                Update(&mUsers, &apps, TRUE);
            }
        }
    }

_EXIT_:
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        Slogger::E(TAG, "NameNotFoundException in OnAppAdded: ");
    }
    return ec;
}

ECode PermissionMonitor::OnAppRemoved(
    /* [in] */ Int32 appUid)
{
    AutoLock lock(this);
    if (appUid < 0) {
        Slogger::E(TAG, "Invalid app in OnAppRemoved: %d", appUid);
        return NOERROR;
    }
    HashMap<Int32, Boolean>::Iterator it = mApps.Find(appUid);
    if (it != mApps.End()) {
        mApps.Erase(it);
    }
    HashMap<Int32, Boolean> apps;
    apps[appUid] = NETWORK; // doesn't matter which permission we pick here
    return Update(&mUsers, &apps, FALSE);
}


} // Connectivity
} // Server
} // Droid
} // Elastos
