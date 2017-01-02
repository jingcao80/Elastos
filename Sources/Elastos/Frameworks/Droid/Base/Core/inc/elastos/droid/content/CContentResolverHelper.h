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

#ifndef __ELASTOS_DROID_CONTENT_CCONTENTRESOLVERHELPER_H__
#define __ELASTOS_DROID_CONTENT_CCONTENTRESOLVERHELPER_H__

#include "_Elastos_Droid_Content_CContentResolverHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CContentResolverHelper)
    , public Singleton
    , public IContentResolverHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI RequestSync(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IBundle* extras);

    CARAPI RequestSync(
        /* [in] */ ISyncRequest* request);

    CARAPI ValidateSyncExtrasBundle(
        /* [in] */ IBundle* extras);

    CARAPI CancelSync(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority);

    CARAPI GetSyncAdapterTypes(
        /* [out, callee] */ ArrayOf<ISyncAdapterType *>** types);

    CARAPI GetSyncAutomatically(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [out] */ Boolean* isSynced);

    CARAPI SetSyncAutomatically(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ Boolean sync);

    CARAPI AddPeriodicSync(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IBundle* extras,
        /* [in] */ Int64 pollFrequency);

    CARAPI RemovePeriodicSync(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IBundle* extras);

    CARAPI CancelSync(
        /* [in] */  ISyncRequest* request);

    CARAPI GetPeriodicSyncs(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [out] */ IList** periodicSyncs);

    CARAPI GetIsSyncable(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [out] */ Int32* isSyncable);

    CARAPI SetIsSyncable(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ Int32 syncable);

    CARAPI GetMasterSyncAutomatically(
        /* [out] */ Boolean* result);

    CARAPI SetMasterSyncAutomatically(
        /* [in] */ Boolean sync);

    CARAPI IsSyncActive(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [out] */ Boolean* isSyncActive);

    CARAPI GetCurrentSync(
        /* [out] */ ISyncInfo** syncInfo);

    CARAPI GetCurrentSyncs(
        /* [out] */ IList** syncInfoList);

    CARAPI GetSyncStatus(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [out] */ ISyncStatusInfo** syncStatusInfo);

    CARAPI IsSyncPending(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [out] */ Boolean* isSyncPending);

    CARAPI AddStatusChangeListener(
        /* [in] */ Int32 mask,
        /* [in] */ ISyncStatusObserver* observerCallback,
        /* [out] */ IInterface** item);

    CARAPI RemoveStatusChangeListener(
        /* [in] */ IInterface* handle);

    CARAPI GetContentService(
        /* [out] */ IIContentService** contentService);

    CARAPI SyncErrorToString(
        /* [in] */ Int32 error,
        /* [out] */ String* str);

    CARAPI GetACTION_SYNC_CONN_STATUS_CHANGED(
        /* [out] */ IIntent** intent);
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CCONTENTRESOLVERHELPER_H__
