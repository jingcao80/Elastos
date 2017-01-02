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

#ifndef __ELASTOS_DROID_PROVIDER_CSYNCSTATECONTRACTHELPERS_H__
#define __ELASTOS_DROID_PROVIDER_CSYNCSTATECONTRACTHELPERS_H__

#include "_Elastos_Droid_Provider_CSyncStateContractHelpers.h"
#include "elastos/droid/provider/SyncStateContractHelpers.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CSyncStateContractHelpers)
    , public Singleton
    , public ISyncStateContractHelpers
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Get the sync state that is associated with the account or null.
     * @param provider the {@link ContentProviderClient} that is to be used to communicate
     * with the {@link android.content.ContentProvider} that contains the sync state.
     * @param uri the uri of the sync state
     * @param account the {@link Account} whose sync state should be returned
     * @return the sync state or null if there is no sync state associated with the account
     * @throws RemoteException if there is a failure communicating with the remote
     * {@link android.content.ContentProvider}
     */
    CARAPI Get(
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ IUri* uri,
        /* [in] */ IAccount* account,
        /* [out, callee] */ ArrayOf<Byte>** value);

    /**
     * Assigns the data array as the sync state for the given account.
     * @param provider the {@link ContentProviderClient} that is to be used to communicate
     * with the {@link android.content.ContentProvider} that contains the sync state.
     * @param uri the uri of the sync state
     * @param account the {@link Account} whose sync state should be set
     * @param data the byte[] that contains the sync state
     * @throws RemoteException if there is a failure communicating with the remote
     * {@link android.content.ContentProvider}
     */
    CARAPI Set(
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ IUri* uri,
        /* [in] */ IAccount* account,
        /* [in] */ ArrayOf<Byte>* data);

    CARAPI Insert(
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ IUri* uri,
        /* [in] */ IAccount* account,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ IUri** retUri);

    CARAPI Update(
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<Byte>* data);

    CARAPI GetWithUri(
       /* [in] */ IContentProviderClient* provider,
       /* [in] */ IUri* uri,
       /* [in] */ IAccount* account,
       /* [out] */ IPair** value);

    /**
     * Creates and returns a ContentProviderOperation that assigns the data array as the
     * sync state for the given account.
     * @param uri the uri of the sync state
     * @param account the {@link Account} whose sync state should be set
     * @param data the byte[] that contains the sync state
     * @return the new ContentProviderOperation that assigns the data array as the
     * account's sync state
     */
    CARAPI NewSetOperation(
        /* [in] */ IUri* uri,
        /* [in] */ IAccount* account,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ IContentProviderOperation** operation);

    /**
     * Creates and returns a ContentProviderOperation that assigns the data array as the
     * sync state for the given account.
     * @param uri the uri of the specific sync state to set
     * @param data the byte[] that contains the sync state
     * @return the new ContentProviderOperation that assigns the data array as the
     * account's sync state
     */
    CARAPI NewUpdateOperation(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ IContentProviderOperation** operation);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CSYNCSTATECONTRACTHELPERS_H__
