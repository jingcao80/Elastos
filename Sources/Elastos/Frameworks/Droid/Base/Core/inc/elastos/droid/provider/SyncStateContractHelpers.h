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

#ifndef __ELASTOS_DROID_PROVIDER_SYNCSTATECONTRACTHELPERS_H__
#define __ELASTOS_DROID_PROVIDER_SYNCSTATECONTRACTHELPERS_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Content::IContentProviderClient;
using Elastos::Droid::Content::IContentProviderOperation;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Utility::IPair;

namespace Elastos {
namespace Droid {
namespace Provider {

class SyncStateContractHelpers
{
public:
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
    static CARAPI Get(
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
    static CARAPI Set(
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ IUri* uri,
        /* [in] */ IAccount* account,
        /* [in] */ ArrayOf<Byte>* data);

    static CARAPI Insert(
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ IUri* uri,
        /* [in] */ IAccount* account,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ IUri** retUri);

    static CARAPI Update(
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<Byte>* data);

    static CARAPI GetWithUri(
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
    static CARAPI NewSetOperation(
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
    static CARAPI NewUpdateOperation(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ IContentProviderOperation** operation);

public:
    /**
     * The content:// style URI for this table
     */
    static const AutoPtr<IUri> CONTENT_URI;

private:
    static AutoPtr<ArrayOf<String> > DATA_PROJECTION;
    static const String SELECT_BY_ACCOUNT;
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_SYNCSTATECONTRACTHELPERS_H__
