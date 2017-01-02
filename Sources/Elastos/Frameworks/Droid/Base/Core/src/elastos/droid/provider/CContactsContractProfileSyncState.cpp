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

#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsContractProfileSyncState.h"
#include "elastos/droid/provider/ContactsContractProfile.h"
#include "elastos/droid/provider/SyncStateContractHelpers.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Provider::ISyncStateContractHelpers;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractProfileSyncState)

CAR_INTERFACE_IMPL_2(CContactsContractProfileSyncState, Singleton
    , IContactsContractProfileSyncState
    , ISyncStateContractColumns)

ECode CContactsContractProfileSyncState::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUri> auUri;
    FAIL_RETURN(ContactsContractProfile::GetCONTENT_URI((IUri**)&auUri))
    return Uri::WithAppendedPath(auUri, IContactsContractProfileSyncState::CONTENT_DIRECTORY, uri);
}

ECode CContactsContractProfileSyncState::Get(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [out] */ ArrayOf<Byte>** value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<IUri> uri;
    FAIL_RETURN(GetCONTENT_URI((IUri**)&uri))
    return SyncStateContractHelpers::Get(provider, uri, account, value);
}

/**
 * @see android.provider.SyncStateContract.Helpers#get
 */
ECode CContactsContractProfileSyncState::GetWithUri(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [out] */ IPair** value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<IUri> uri;
    GetCONTENT_URI((IUri**)&uri);
    return SyncStateContractHelpers::GetWithUri(provider, uri, account, value);
}

/**
 * @see android.provider.SyncStateContract.Helpers#set
 */
ECode CContactsContractProfileSyncState::Set(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data)
{
    AutoPtr<IUri> uri;
    GetCONTENT_URI((IUri**)&uri);
    return SyncStateContractHelpers::Set(provider, uri, account, data);
}

/**
 * @see android.provider.SyncStateContract.Helpers#newSetOperation
 */
ECode CContactsContractProfileSyncState::NewSetOperation(
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IContentProviderOperation** operation)
{
    VALIDATE_NOT_NULL(operation);

    AutoPtr<IUri> uri;
    GetCONTENT_URI((IUri**)&uri);
    return SyncStateContractHelpers::NewSetOperation(uri, account, data, operation);
}

} //Provider
} //Droid
} //Elastos