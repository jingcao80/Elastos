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

#ifndef __ELASTOS_DROID_CONTENT_ICONTENTSERVICE_H__
#define __ELASTOS_DROID_CONTENT_ICONTENTSERVICE_H__

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>
#include <binder/Binder.h>

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Database::IIContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Content {

class IContentServiceProxy
    : public Object
    , public IIContentService
{
public:
    IContentServiceProxy(
        /* [in] */ android::sp<android::IBinder> remote);

    CAR_INTERFACE_DECL();

    CARAPI UnregisterContentObserver(
        /* [in] */ IIContentObserver* observer);

    CARAPI RegisterContentObserver(
        /* [in] */ IUri* uri,
        /* [in] */ Boolean notifyForDescendants,
        /* [in] */ IIContentObserver* observer,
        /* [in] */ Int32 userHandle);

    CARAPI NotifyChange(
        /* [in] */ IUri* uri,
        /* [in] */ IIContentObserver* observer,
        /* [in] */ Boolean observerWantsSelfNotifications,
        /* [in] */ Boolean syncToNetwork,
        /* [in] */ Int32 userHandle);

    CARAPI RequestSync(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IBundle* extras);

    CARAPI Sync(
        /* [in] */ ISyncRequest* request);

    CARAPI SyncAsUser(
        /* [in] */ ISyncRequest* request,
        /* [in] */ Int32 userId);

    CARAPI CancelSync(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IComponentName* cname);

    CARAPI CancelSyncAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IComponentName* cname,
        /* [in] */ Int32 userId);

    CARAPI CancelRequest(
        /* [in] */ ISyncRequest* request);

    CARAPI GetSyncAutomatically(
        /* [in] */ IAccount* account,
        /* [in] */ const String& providerName,
        /* [out] */ Boolean* result);

    CARAPI GetSyncAutomaticallyAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ const String& providerName,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    CARAPI SetSyncAutomatically(
        /* [in] */ IAccount* account,
        /* [in] */ const String& providerName,
        /* [in] */ Boolean sync);

    CARAPI SetSyncAutomaticallyAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ const String& providerName,
        /* [in] */ Boolean sync,
        /* [in] */ Int32 userId);

    CARAPI GetPeriodicSyncs(
        /* [in] */ IAccount* account,
        /* [in] */ const String& providerName,
        /* [out] */ IList** periodicSyncList);

    CARAPI AddPeriodicSync(
        /* [in] */ IAccount* account,
        /* [in] */ const String& providerName,
        /* [in] */ IBundle* extras,
        /* [in] */ Int64 pollFrequency);

    CARAPI RemovePeriodicSync(
        /* [in] */ IAccount* account,
        /* [in] */ const String& providerName,
        /* [in] */ IBundle* extras);

    CARAPI GetIsSyncable(
        /* [in] */ IAccount* account,
        /* [in] */ const String& providerName,
        /* [out] */ Int32* value);

    CARAPI GetIsSyncableAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ const String& providerName,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* value);

    CARAPI SetIsSyncable(
        /* [in] */ IAccount* account,
        /* [in] */ const String& providerName,
        /* [in] */ Int32 syncable);

    CARAPI SetMasterSyncAutomatically(
        /* [in] */ Boolean flag);

    CARAPI SetMasterSyncAutomaticallyAsUser(
        /* [in] */ Boolean flag,
        /* [in] */ Int32 userId);

    CARAPI GetMasterSyncAutomatically(
        /* [out] */ Boolean* result);

    CARAPI GetMasterSyncAutomaticallyAsUser(
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    CARAPI GetCurrentSyncs(
        /* [out] */ IList** syncInfoList);

    CARAPI GetCurrentSyncsAsUser(
        /* [in] */ Int32 userId,
        /* [out] */ IList** syncInfoList);

    CARAPI GetSyncAdapterTypes(
        /* [out, callee] */ ArrayOf<ISyncAdapterType*>** result);

    CARAPI GetSyncAdapterTypesAsUser(
        /* [in] */ Int32 userId,
        /* [out, callee] */ ArrayOf<ISyncAdapterType*>** result);

    CARAPI IsSyncActive(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IComponentName* name,
        /* [out] */ Boolean* isActive);

    CARAPI GetSyncStatus(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IComponentName* name,
        /* [out] */ ISyncStatusInfo** result);

    CARAPI GetSyncStatusAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IComponentName* name,
        /* [in] */ Int32 userId,
        /* [out] */ ISyncStatusInfo** result);

    CARAPI IsSyncPending(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IComponentName* name,
        /* [out] */ Boolean* isPending);

    CARAPI IsSyncPendingAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IComponentName* name,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* isPending);

    CARAPI AddStatusChangeListener(
        /* [in] */ Int32 mask,
        /* [in] */ IISyncStatusObserver* observer);

    CARAPI RemoveStatusChangeListener(
        /* [in] */ IISyncStatusObserver* observer);

private:
    static const String DESCRIPTOR;
    static const Int32 TRANSACTION_unregisterContentObserver;
    static const Int32 TRANSACTION_registerContentObserver;

    android::sp<android::IBinder> mRemote;
};

} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_ICONTENTSERVICE_H__
